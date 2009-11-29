#ifndef DATABASE_HPP_
#define DATABASE_HPP_

#include <QString>
#include <QSqlDatabase>

class Database {
public:
  Database ();
  ~Database ();

  bool ProcessSettingsFile (QString file_name);
  bool Connect ();

  // for worker
  
  bool AddEngineSearchPath (QString path);
  bool AddEngine (QString name,
                  QString command_line,
                  QVariant gtp_name,
                  QVariant gtp_version,
                  QString send_gtp_config);
  bool AddGameSetup (QString name, int board_size, float komi);
  bool AddExperiment (QString name, QString description);
  bool AddGame (QString experiment,
                QString game_setup,
                QString first_engine,
                QString second_engine);

  QStringList EngineSearchPath();

  QString settings_file_name;
  QSqlDatabase db;
};


class DbEngine {
public:
  void Get (QSqlDatabase db, int player_id);

  int id;
  QString name;
  QString command_line;
  QString gtp_name;
  QString gtp_version;
  QString send_gtp_config;
};

class DbGame {
public:
  bool GetUnclaimed (QSqlDatabase db);
  void Finish (QSqlDatabase db,
               bool black_won,
               QVariant black_gtp_game_report,
               QVariant white_gtp_game_report,
               QString sgf);

  int id;
  QString rule_set;
  int board_size;
  float komi;

  DbEngine black;
  DbEngine white;
  // QString black_send_gtp_config;
  // QString white_send_gtp_config;
};

#endif
