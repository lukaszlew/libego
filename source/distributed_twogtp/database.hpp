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
  
  bool AddEngine (QString name,
                  QString command_line,
                  QVariant gtp_name,
                  QVariant gtp_version,
                  QString send_gtp_config);
  bool AddGameSetup (QString name, int board_size, float komi);
  bool AddExperiment (QString name,
                      QString game_setup_name,
                      QString first_engine_name,
                      QString second_engine_name,
                      QString description);
  int AddGame (QString experiment, bool first_is_black);

  bool AddEngineParam (int game_id, bool for_first ,QString name, QString value);

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
               bool first_won,
               QVariant first_report,
               QVariant second_report,
               QString sgf);

  int id;
  QString rule_set;
  int board_size;
  float komi;
  bool first_is_black;

  DbEngine first;
  DbEngine second;
  QList <QPair<QString, QString> > first_params;
  QList <QPair<QString, QString> > second_params;
};

#endif
