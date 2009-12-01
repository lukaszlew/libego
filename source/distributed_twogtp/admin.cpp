#include <iostream>
#include <string>

#include <QDir>
#include <QString>
#include <QVariant>
#include <QtDebug>


#include "gtp.hpp"
#include "gtp_process.hpp"
#include "test.hpp"

#include "admin.hpp"

Admin::Admin (Database& db) : db (db)
{
  config = "";
  command_line = "";
  first_engine = "";
  second_engine = "";
  experiment_description = "";
}


Admin::~Admin ()
{
}


void Admin::Run ()
{
  Gtp::Repl gtp;
  gtp.Register ("add_engine_search_path",  this, &Admin::CAddEngineSearchPath);

  gtp.Register ("add_engine_config_line", this, &Admin::CAddEngineConfigLine);
  gtp.Register ("set_engine_cmd", this, &Admin::CSetEngineCommandLine);
  gtp.Register ("add_engine",     this, &Admin::CAddEngine);

  gtp.Register ("add_game_setup", this, &Admin::CAddGameSetup);

  gtp.Register ("set_experiment_engine", this, &Admin::CSetExperimentEngine);
  gtp.Register ("set_experiment_description", this, &Admin::CSetExperimentDescription);
  gtp.Register ("add_experiment", this, &Admin::CAddExperiment);

  gtp.Register ("add_param", this, &Admin::CAddParam);

  gtp.Register ("add_games",      this, &Admin::CAddGames);
  gtp.Run (std::cin, std::cout);
}


void Admin::CSetEngineCommandLine (Gtp::Io& io) {
  command_line = QString::fromStdString (io.ReadLine());
}

void Admin::CAddEngineConfigLine (Gtp::Io& io) {
  config += "\n" + QString::fromStdString (io.ReadLine ());
  io.CheckEmpty ();
}

void Admin::CAddEngineSearchPath (Gtp::Io& io)
{
  QString path = QString::fromStdString (io.Read<std::string>());
  io.CheckEmpty();
  if (!db.AddEngineSearchPath (path))
    io.SetError ("");
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

void Admin::CAddExperiment (Gtp::Io& io)
{
  QString name = QString::fromStdString (io.Read<std::string>());
  QString game_setup = QString::fromStdString (io.Read<std::string>());
  io.CheckEmpty();
  if (!db.AddExperiment (name, game_setup, first_engine, second_engine,
                         experiment_description))
  {
    io.SetError ("");
  }
  first_engine = "";
  second_engine = "";
  experiment_description = "";
}

void Admin::CAddParam (Gtp::Io& io) {
  int num = io.Read<int> ();
  if (num != 1 && num != 2) {
    io.SetError ("wronge engine number");
    return;
  }
  QString name  = QString::fromStdString (io.Read<std::string>());
  bool for_first_engine = num == 1;
  while (true) {
    QString value = QString::fromStdString (io.Read<std::string>(""));
    if (value == "") break;
    params [qMakePair (for_first_engine, name)] . append (value);
  }
}

void Admin::CAddGames (Gtp::Io& io)
{
  QString experiment = QString::fromStdString (io.Read<std::string>());
  int game_count = io.Read<int>();
  io.CheckEmpty();
  
  int game_ok = 0;
  for (int i = 0; i < game_count; i++) {
    int game_id = db.AddGame (experiment, i % 2 == 0);
    if (game_id < 0) break;
    QPair <bool, QString> param;
    foreach (param, params.keys()) {
      QString value = params [param] [rand() % params [param].size()];
      db.AddEngineParam (game_id, param.first, param.second, value);
    }
    game_ok += 1;
  }

  if (game_ok != game_count)
    io.SetError ("");
}


bool Admin::AddEngine (QString name, QString config, QString command_line)
{
  QString send_gtp_config = "";
  QVariant gtp_name;
  QVariant gtp_version;

  GtpProcess process;
  if (!process.Start (name, command_line, db.EngineSearchPath())) {
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
