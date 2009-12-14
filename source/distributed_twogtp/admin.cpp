#include <iostream>
#include <string>

#include <QDir>
#include <QString>
#include <QVariant>
#include <QtDebug>


#include "gtp.hpp"
#include "gtp_process.hpp"
#include "test.hpp"

#include "CBAST.h"

#include "admin.hpp"

Admin::Admin (Database& db) : db (db)
{
  config = "";
  command_line = "";
  first_engine = "";
  second_engine = "";
  experiment_description = "";
  experiment_params.clear();
  experiment_id = -1;
}


Admin::~Admin ()
{
}


void Admin::Run ()
{
  Gtp::Repl gtp;

  gtp.Register ("add_engine_config_line", this, &Admin::CAddEngineConfigLine);
  gtp.Register ("set_engine_cmd", this, &Admin::CSetEngineCommandLine);
  gtp.Register ("add_engine",     this, &Admin::CAddEngine);

  gtp.Register ("add_game_setup", this, &Admin::CAddGameSetup);

  gtp.Register ("set_experiment_engine", this, &Admin::CSetExperimentEngine);
  gtp.Register ("set_experiment_description", this, &Admin::CSetExperimentDescription);
  gtp.Register ("add_experiment_param", this, &Admin::CAddExperimentParam);
  gtp.Register ("add_experiment", this, &Admin::CAddExperiment);
  gtp.Register ("close_all_experiments", this, &Admin::CCloseAllExperiments);

  gtp.Register ("loop_add_games", this, &Admin::CLoopAddGames);

  gtp.Register ("extract_csv", this, &Admin::CExtractCsv);

  gtp.Run (std::cin, std::cout);
}


void Admin::CSetEngineCommandLine (Gtp::Io& io) {
  command_line = QString::fromStdString (io.ReadLine());
}

void Admin::CAddEngineConfigLine (Gtp::Io& io) {
  config += "\n" + QString::fromStdString (io.ReadLine ());
  io.CheckEmpty ();
}

void Admin::CAddEngine (Gtp::Io& io)
{
  QString name = QString::fromStdString (io.Read<std::string>());
  io.CheckEmpty();
  if (!AddEngine (name, config, command_line))
    io.SetError ("");
  name = "";
  config = "";
  command_line = "";
}


void Admin::CAddGameSetup (Gtp::Io& io)
{
  QString name = QString::fromStdString (io.Read<std::string>());
  int board_size = io.Read<int>();
  float komi = io.Read<float>();
  io.CheckEmpty();
  if (!db.AddGameSetup (name, board_size, komi))
    io.SetError ("");
}


void Admin::CSetExperimentEngine (Gtp::Io& io)
{
  int num = io.Read<int> ();
  if (num != 1 && num != 2) {
    io.SetError ("wronge engine number");
    return;
  }
  QString name = QString::fromStdString (io.Read<std::string>());
  (num == 1 ? first_engine : second_engine) = name;
}

void Admin::CSetExperimentDescription (Gtp::Io& io) {
  experiment_description = QString::fromStdString (io.ReadLine());
}

void Admin::CAddExperimentParam (Gtp::Io& io) {
  Param p;
  p.name = QString::fromStdString (io.Read<std::string>());
  p.min_value = io.Read<double>();
  p.max_value = io.Read<double>();
  p.SetFun (QString::fromStdString (io.Read<std::string>()));
  io.CheckEmpty();
  experiment_params.append (p);
}

void Admin::CAddExperiment (Gtp::Io& io)
{
  QString game_setup = QString::fromStdString (io.Read<std::string>());
  io.CheckEmpty();
  QStringList ps;
  foreach (const Param& param, experiment_params) ps.append (param.name);
  experiment_id = db.AddExperiment (game_setup, first_engine, second_engine,
                                    experiment_description, ps);
  CHECK (experiment_id > 0);
  first_engine = "";
  second_engine = "";
  experiment_description = "";
  // experiment_params.clear(); // TODO solve it better
}

void Admin::CCloseAllExperiments (Gtp::Io& io) {
  io.CheckEmpty();
  db.CloseAllExperiments();
}


void Admin::SetPvBastFirst (CBAST& bast, int bast_id) {
  pv_first.clear();
  pv_second.clear();

  std::vector<double> bast_v = bast.NextSample (bast_id);
  uint ii = 0;

  foreach (Param param, experiment_params) {
    CHECK (ii < bast_v.size());
    pv_first.append (qMakePair (param.name,
                                QString::number (param.OfBast(bast_v[ii]))));
    qDebug () << pv_first.last();
    ii += 1;
  }

  CHECK (ii == bast_v.size());
}


void Admin::CLoopAddGames (Gtp::Io& io)
{
  io.CheckEmpty();

  CHECK (experiment_params.size() > 0);
  CBAST bast (experiment_params.size(), 1.0);
  QMap <int, int> bast_id_of_game_id;
  int bast_id = 0;

  int goal = 10;
  QString last_finished_at = "1982";

  while (true) {
    QList <GameResult> results =
      db.GetNewGameResults (experiment_id, true, &last_finished_at);

    foreach (const GameResult& r, results) {
      qDebug () << "New results:" << r.ToString().c_str();
      CHECK (bast_id_of_game_id.contains (r.id));
      bast.OnOutcome (r.victory ? CResults::Win : CResults::Loss,
                      bast_id_of_game_id [r.id]);
    }

    int unclaimed_games = db.GetUnclaimedGameCount (experiment_id);
    if (unclaimed_games < goal / 2) goal *= 2;
    qDebug() << "unclaimed / goal = " << unclaimed_games << " / " << goal;

    int add_games = goal - unclaimed_games;
    if (add_games < 0) add_games = 0;
    add_games = add_games / 2 * 2; // parity

    for (int i = 0; i < add_games; i+=1) {
      bast_id += 1;
      //SetPvBoth ();
      SetPvBastFirst (bast, bast_id);
      int game_id = db.AddGame (experiment_id, i%2, pv_first, pv_second);
      bast_id_of_game_id [game_id] = bast_id;
      qDebug () << "new game; id = " << game_id;
      if (game_id < 0) {
        io.SetError ("Can't add game");
        return;
      }
    }
    sleep (10);
  }
}


bool Admin::AddEngine (QString name, QString config, QString command_line)
{
  QString send_gtp_config = "";
  QVariant gtp_name;
  QVariant gtp_version;

  GtpProcess process;
  if (!process.Start (name, command_line)) {
    std::cerr << "can't start the process" << std::endl;
    return false;
  }

  gtp_name = process.TryCommand ("name");
  gtp_version = process.TryCommand ("version");


  bool db_add_ok = db.AddEngine (name,
                                 command_line,
                                 gtp_name,
                                 gtp_version,
                                 config);
  if (!db_add_ok) {
    std::cerr << "can't add engine to database" << std::endl;
    return false;
  }

  return true;
}


void Admin::CExtractCsv (Gtp::Io& io) {
  QString experiment = QString::fromStdString (io.Read<std::string>());
  int num = io.Read<int> ();
  if (num != 1 && num != 2) {
    io.SetError ("wronge engine number");
    return;
  }
  QString file_name  = QString::fromStdString (io.Read<std::string>());
  io.CheckEmpty();

  QFile file(file_name);
  if (!file.open (QIODevice::WriteOnly | QIODevice::Text)) {
    io.SetError (qPrintable ("can't open file: " + file_name));
    return;
  }

  QTextStream out(&file);
  if (!db.DumpCsv (experiment, out, num == 1)) {
    io.SetError ("can't extract CSV");
    return;
  }
}
