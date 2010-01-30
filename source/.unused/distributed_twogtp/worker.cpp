#include <QString>
#include <QtDebug>
#include <QVariant>

#include "test.hpp"
#include "full_board.hpp"
#include "gtp_process.hpp"
#include "database.hpp"

#include "worker.hpp"

Worker::Worker (Database& db) : db (db)
{
}


Worker::~Worker ()
{
}


void Worker::Run ()
{
  while (true) {
    if (!GrabJob()) {
      qWarning() << "Waiting for job ...";
      sleep (30);
      continue;
    }
  }
}

void StartPlayer (GtpProcess& p, DbGame& db_game, bool first) {
  DbEngine& db_player = first ? db_game.first : db_game.second;
  //QString& config = black ? db_game.black_gtp_config : db_game.white_gtp_config;

  qDebug() << "---------------------------------------------------------";
  qDebug() << "STARTING " << (first ? "FIRST: " : "SECOND: ");
  qDebug() << db_player.name.toLatin1().data();
  qDebug() << "command_line:" << db_player.command_line.trimmed().toLatin1().data();

  CHECK (p.Start (db_player.name, db_player.command_line));
  CHECK (p.TryCommand ("name").toString () == db_player.gtp_name);
  CHECK (p.TryCommand ("version").toString () == db_player.gtp_version);
  p.TryCommand ("go_rules " + db_game.rule_set);

  qDebug () << "engine.send_gtp_config:";
  foreach (QString line, db_player.send_gtp_config.split ("\n")) {
    qDebug () << "  " << line.toLatin1().data();
    CHECK (!p.TryCommand (line).isNull());
  }

  if (first)
  {
    CHECK (p.TryCommand ("known_command d2gtp-params") == "true");
    QPair<QString, QString> param;
    foreach (param, db_game.first_params) {
      QString cmd = "d2gtp-params " + param.first + " " + param.second;
      qDebug () << cmd.toLatin1().data();
      CHECK (p.TryCommand (cmd) != QVariant());
    }
  }

  CHECK (p.ClearBoard (db_game.board_size, db_game.komi));
}


const int genmove_timeout_ms = 600000;

bool Worker::GrabJob ()
{
  // claim job
  if (!db_game.GetUnclaimed (db.db)) return false;

  // do the work
  GtpProcess first;
  GtpProcess second;

  StartPlayer (first,  db_game, true);
  StartPlayer (second, db_game, false);

  // TODO send seed
  
  board.Clear ();
  GtpProcess* act   = &first;
  GtpProcess* other = &second;
  if (!db_game.first_is_black) qSwap (act, other);

  vector <Move> history;
  bool first_won;
  Player act_player = Player::Black();
  bool by_resignation;

  while (true) {
    Vertex v;
    CHECK (act->Genmove (act_player, &v, genmove_timeout_ms));

    if (v == Vertex::Invalid ()) { // resignation
      first_won = (other == &first);
      qDebug() << "Player " << act_player.ToGtpString().c_str()
               << " (" << act->Name() << ") resigned.";
      by_resignation = true;
      break;
    }

    Move move = Move (act_player, v);
    history.push_back (move);

    CHECK (board.Play (move));
    
    qDebug() << board.GetBoard ().ToAsciiArt (move.GetVertex()).c_str()
             << move.ToGtpString().c_str()
             << " (" << act->Name().toLatin1().data() << ")\n";

    CHECK (other->Play (move));

    if (board.GetBoard().BothPlayerPass()) {
      Player winner = board.GetBoard().TrompTaylorWinner();
      first_won = (winner == Player::Black ()) == db_game.first_is_black;
      qDebug() << "Both player pass. Winner is "
               << QString::fromStdString (winner.ToGtpString()) << endl;
      break;
    }

    act_player = act_player.Other ();
    qSwap (act, other);
  }

  qDebug () << (first_won ? "First" : "Second") << " player won. \n\n" ;

  // update finish
  QString sgf = "";

  db_game.Finish (db.db,
                  first_won,
                  first.TryCommand ("game_report"),
                  second.TryCommand ("game_report"),
                  sgf);
  
  return true;
}

