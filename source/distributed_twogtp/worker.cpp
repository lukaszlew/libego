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

void StartPlayer (GtpProcess& p, DbGame& db_game, bool black, QStringList paths) {
  DbEngine& db_player = black ? db_game.black : db_game.white;
  QString& config = black ? db_game.black_gtp_config : db_game.white_gtp_config;

  qDebug() << "";
  qDebug()
    << "STARTING " << (black ? "black " : "white ")
    << db_player.name << " cmd:" <<  db_player.command_line;

  CHECK (p.Start (db_player.name, db_player.command_line, paths));
  CHECK (p.TryCommand ("name").toString () == db_player.gtp_name);
  CHECK (p.TryCommand ("version").toString () == db_player.gtp_version);

  qDebug () << "  config:";

  foreach (QString line, db_player.send_gtp_config.split ("\n")) {
    qDebug () << line;
    CHECK (!p.TryCommand (line).isNull());
  }

  p.TryCommand ("go_rules " + db_game.rule_set);

  CHECK (p.ClearBoard (db_game.board_size, db_game.komi));

  qDebug () << "  more config:";

  foreach (QString line, config.split ("\n")) {
    CHECK (!p.TryCommand (line).isNull());
  }
}

const int genmove_timeout_ms = 600000;

bool Worker::GrabJob ()
{
  // claim job
  if (!db_game.GetUnclaimed (db.db)) return false;
  QStringList paths = db.EngineSearchPath();

  // do the work
  GtpProcess black;
  GtpProcess white;

  StartPlayer (black, db_game, true, paths);
  StartPlayer (white, db_game, false, paths);

  // TODO send seed
  
  board.Clear ();
  GtpProcess* act   = &black;
  GtpProcess* other = &white;
  vector <Move> history;
  Player winner = Player::Invalid();
  Player act_player = Player::Black();
  bool by_resignation;

  while (true) {
    Vertex v;
    CHECK (act->Genmove (act_player, &v, genmove_timeout_ms));

    if (v == Vertex::Invalid ()) { // resignation
      winner = (other == &black ? Player::Black() : Player::White());
      qDebug() << "Player " << act->Name()
               << " resigned. Winner = " << winner.ToGtpString().c_str() << "\n\n";
      by_resignation = true;
      break;
    }

    Move move = Move (act_player, v);
    history.push_back (move);

    CHECK (board.Play (move));
    
    qDebug() << QString::fromStdString (move.ToGtpString())
             << " (" << act->Name() << ")";
    qDebug() << QString::fromStdString (board.GetBoard().ToAsciiArt(move.GetVertex()));

    CHECK (other->Play (move));

    if (board.GetBoard().BothPlayerPass()) {
      winner = board.GetBoard().TrompTaylorWinner();
      qDebug() << "Both player pass. Winner is "
               << QString::fromStdString (winner.ToGtpString()) << endl;
      break;
    }

    act_player = act_player.Other ();
    qSwap (act, other);
  }

  // update finish
  QString sgf = "";

  db_game.Finish (db.db,
                  winner == Player::Black(),
                  black.TryCommand ("game_report"),
                  white.TryCommand ("game_report"),
                  sgf);
  
  return true;
}

