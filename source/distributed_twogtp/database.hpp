#ifndef DATABASE_HPP_
#define DATABASE_HPP_

#include <vector>

#include <QString>
#include <QSqlDatabase>

typedef QList< QPair<QString, QString> > ParamsValues;

struct GameResult {
  int id;
  std::vector <float> params;
  bool victory; // 1 = win 0 = loss
  std::string ToString() const;
};

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

  int AddExperiment (QString game_setup_name,
                     QString first_engine_name,
                     QString second_engine_name,
                     QString description,
                     QStringList experiment_params);

  void CloseAllExperiments ();

  int AddGame (int experiment_id, bool first_is_black,
               const ParamsValues& pv_first);

  bool AddGameParam (int game_id, QString name, QString value);

  int GetUnclaimedGameCount (int experiment_id);

  QStringList GetParams (int experiment_id);

  QList <GameResult> GetNewGameResults (int expeiment_id,
                                        QString* last_finished_at,
                                        QStringList params);

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
};

#endif
