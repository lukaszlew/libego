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
}


Admin::~Admin ()
{
}


void Admin::Run ()
{
  Gtp::Repl gtp;
  gtp.Register ("add_engine_search_path", this, &Admin::CAddEngineSearchPath);
  gtp.Register ("add_engine",     this, &Admin::CAddEngine);
  gtp.Register ("add_game_setup", this, &Admin::CAddGameSetup);
  gtp.Register ("add_games",      this, &Admin::CAddGames);
  gtp.Register ("add_experiment", this, &Admin::CAddExperiment);
  gtp.Run (std::cin, std::cout);
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
  QString config_file = QString::fromStdString (io.Read<std::string>());
  QString command_line = QString::fromStdString (io.ReadLine());
  io.CheckEmpty();
  if (!AddEngine (name, config_file, command_line))
    io.SetError ("");
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


void Admin::CAddExperiment (Gtp::Io& io)
{
  QString name = QString::fromStdString (io.Read<std::string>());
  QString game_setup = QString::fromStdString (io.Read<std::string>());
  QString description = QString::fromStdString (io.ReadLine());
  io.CheckEmpty();
  if (!db.AddExperiment (name, game_setup, description))
    io.SetError ("");
}


void Admin::CAddGames (Gtp::Io& io)
{
  QString experiment = QString::fromStdString (io.Read<std::string>());
  QString first_engine = QString::fromStdString (io.Read<std::string>());
  QString second_engine = QString::fromStdString (io.Read<std::string>());
  int game_count = io.Read<int>();
  io.CheckEmpty();
  
  int game_ok = 0;
  for (int i = 0; i < game_count; i++) {
    if (!db.AddGame (experiment, first_engine, second_engine))
      break;
      
    game_ok += 1;
    qSwap (first_engine, second_engine);
  }

  if (game_ok != game_count)
    io.SetError ("");
}


bool Admin::AddEngine (QString name, QString config_file, QString command_line)
{
  QString send_gtp_config = "";
  QVariant gtp_name;
  QVariant gtp_version;

  if (config_file != "no_config") {
    QFile config (config_file);
    if (!config.open (QIODevice::ReadOnly | QIODevice::Text)) {
      std::cerr << "can't open config" << std::endl;
      return false;
    }
    send_gtp_config += config.readAll ();
  }

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
                                 send_gtp_config);
  if (!db_add_ok) {
    std::cerr << "can't add engine to database" << std::endl;
    return false;
  }

  return true;
}
