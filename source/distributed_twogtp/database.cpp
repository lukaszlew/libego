#include <QHostInfo>
#include <QSettings>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QString>
#include <QVariantList>
#include <QtDebug>

#include <unistd.h> // TODO use Qt sleep

#include "test.hpp"

#include "database.hpp"

static const QString sql_connection = "sql_connection";

Database::Database ()
{
  db = QSqlDatabase::addDatabase ("QMYSQL", sql_connection);
}


Database::~Database ()
{
  db = QSqlDatabase ();
  QSqlDatabase::removeDatabase (sql_connection);
}


bool Database::ProcessSettingsFile (QString file_name)
{
  QSettings db_settings (file_name, QSettings::IniFormat);

  if (!db_settings.contains ("client/host") ||
      !db_settings.contains ("client/user") ||
      !db_settings.contains ("client/pass")  ||
      !db_settings.contains ("client/database"))
  {
    db_settings.setValue ("client/host", "example_host");
    db_settings.setValue ("client/user", "example_user");
    db_settings.setValue ("client/pass", "example_password");
    db_settings.setValue ("client/database", "example_database");

    qWarning()
      << "Created exemplary config file: "
      << db_settings.fileName().toLatin1() << "\n"
      << "Please fill example_* values." << "\n";

    return false;
  }

  db.setHostName (db_settings.value ("client/host").toString());
  db.setUserName (db_settings.value ("client/user").toString());
  db.setPassword (db_settings.value ("client/pass").toString());
  db.setDatabaseName (db_settings.value ("client/database").toString());

  return true;
}


bool Database::Connect ()
{
  if (!db.open ()) {
    qCritical() << db.lastError().text();
    return false;
  }
  return true;
}


bool Database::AddEngine (QString name,
                          QString command_line,
                          QVariant gtp_name,
                          QVariant gtp_version,
                          QString send_gtp_config)
{
  QSqlQuery q (db);
  CHECK (q.prepare
         ("INSERT INTO engine "
          "(name, command_line, gtp_name, gtp_version, send_gtp_config) "
          "VALUES (?, ?, ?, ?, ?)"));
  q.addBindValue (name);
  q.addBindValue (command_line);
  q.addBindValue (gtp_name);
  q.addBindValue (gtp_version);
  q.addBindValue (send_gtp_config);
  bool ok = q.exec ();
  if (!ok) {
    qCritical() << db.lastError().text() << " : " << q.lastError();
  }
  return ok;
}


bool Database::AddGameSetup (QString name, int board_size, float komi) {
  QSqlQuery q (db);
  CHECK (q.prepare ("INSERT INTO game_setup (name, rule_set, board_size, komi)"
                    "VALUES (?, ?, ?, ?)"));
  q.addBindValue (name);
  q.addBindValue ("cgos");
  q.addBindValue (board_size);
  q.addBindValue (komi);
  return q.exec ();
}


bool Database::AddExperiment (QString name,
                              QString game_setup_name,
                              QString first_engine_name,
                              QString second_engine_name,
                              QString description,
                              QStringList experiment_params) {
  QSqlQuery q (db);
  CHECK (q.prepare ("INSERT INTO experiment ("
                    "  name, "
                    "  game_setup_id, "
                    "  first_engine_id,"
                    "  second_engine_id, "
                    "  description "
                    ") VALUES ("
                    "  ?, "
                    "  (SELECT id FROM game_setup WHERE name = ?), "
                    "  (SELECT id FROM engine WHERE name = ?),"
                    "  (SELECT id FROM engine WHERE name = ?),"
                    "  ? "
                    ")"));
  q.addBindValue (name);
  q.addBindValue (game_setup_name);
  q.addBindValue (first_engine_name);
  q.addBindValue (second_engine_name);
  q.addBindValue (description);
  CHECK (q.exec ());
  CHECK (q.lastInsertId() != QVariant());
  int id = q.lastInsertId().toInt();

  CHECK (q.prepare ("INSERT INTO param (experiment_id, name) VALUES (?, ?)"));
  QVariantList ids;
  QVariantList param_names;  
  foreach (QString param, experiment_params) {
    ids << id;
    param_names << param;
  }
  q.addBindValue (ids);
  q.addBindValue (param_names);
  CHECK (q.execBatch ());
  return true;
}


int Database::AddGame (QString experiment, bool first_is_black)
{
  QSqlQuery q (db);
  QString query = 
    "INSERT INTO game (experiment_id, first_is_black, created_at) "
    "VALUES ("
    "  (SELECT id FROM experiment WHERE name = ?), "
    "  ?, "
    "  NOW()"
    ")";
  CHECK (q.prepare (query));

  q.addBindValue (experiment);
  q.addBindValue (first_is_black);
  CHECK (q.exec ());
  CHECK (q.lastInsertId() != QVariant());
  return q.lastInsertId().toInt();
}

bool Database::AddEngineParam (int game_id,
                               bool for_first,
                               QString name,
                               QString value)
{
  QSqlQuery q (db);
  CHECK (q.prepare (
    "INSERT INTO engine_param (game_id, name, value, for_first) "
    "VALUES (?, ?, ?, ?)"
  ));

  q.addBindValue (game_id);
  q.addBindValue (name);
  q.addBindValue (value);
  q.addBindValue (for_first);
  CHECK (q.exec ());
  return true;
}


bool Database::DumpCsv (QString experiment_name,
                        QTextStream& out,
                        bool first_engine)
{
  QSqlQuery q (db);

  // get param list 
  CHECK (q.prepare ("SELECT param.name "
                    "FROM experiment "
                    "JOIN param ON param.experiment_id = experiment.id "
                    "WHERE experiment.name = ? "));
  q.addBindValue (experiment_name);
  CHECK (q.exec ());

  QStringList params;
  while (q.next()) params.append (q.value(0).toString());
    
  out << "game.id, ";
  foreach (QString s, params) {
    out << s << ", ";
  }
  out << "first_won" << endl;

  CHECK (q.prepare ("SELECT game.id, game.first_won "
                    "FROM experiment "
                    "JOIN game ON game.experiment_id = experiment.id "
                    "WHERE experiment.name = ? AND game.first_won IS NOT NULL"));
  q.addBindValue (experiment_name);
  CHECK (q.exec ());  

  while (q.next ()) {
    int game_id = q.value (0).toInt();
    int first_won = q.value (1).toInt();

    QVector<QString> param_values (params.size());

    //qDebug () << "A " << game_id << " " << first_won;
    QSqlQuery q2 (db);
    CHECK (q2.prepare ("SELECT name, value, for_first "
                       "FROM engine_param "
                       "WHERE game_id = ?"));
    q2.addBindValue (game_id);
    CHECK (q2.exec ());
    while (q2.next()) {
      QString name   = q2.record().value ("name").toString();
      bool for_first = q2.record().value ("for_first").toInt();
      QString value  = q2.record().value ("value").toString();

      int i = params.indexOf (name);
      if (i == -1 || first_engine != for_first) continue;

      param_values [i] = value;
    }


    out << game_id << ", ";
    foreach (QString s, param_values) {
      out << s << ", ";
    }
    out << first_won << "\n";
  }
  return true;
}

// -----------------------------------------------------------------------------


void DbEngine::Get (QSqlDatabase db, int player_id) {
  QSqlQuery q (db);
  CHECK (q.prepare ("SELECT "
                    "name, command_line, gtp_name, gtp_version, send_gtp_config "
                    "FROM engine WHERE id = ?"));
  q.addBindValue (player_id);
  CHECK (q.exec());
  CHECK (q.next());

  name            = q.record().value ("name").toString();
  command_line    = q.record().value ("command_line").toString();
  gtp_name        = q.record().value ("gtp_name").toString();
  gtp_version     = q.record().value ("gtp_version").toString();
  send_gtp_config = q.record().value ("send_gtp_config").toString();

  CHECK (!q.next());
}

// -----------------------------------------------------------------------------

bool DbGame::GetUnclaimed (QSqlDatabase db) {
  // TODO atomicity
  QSqlQuery q (db);

  CHECK (q.exec ("SELECT id FROM game "
                 "WHERE claimed_by IS NULL "
                 "LIMIT 1 FOR UPDATE"));
  if (!q.next()) return false;
  id = q.record().value (0).toInt();


  CHECK (q.prepare ("UPDATE game "
                    "SET claimed_by = ?, claimed_at = NOW() "
                    "WHERE id = ?"));
  q.addBindValue ("pid" + QString::number (getpid()) +
                  "@"   + QHostInfo::localHostName());
  q.addBindValue (id);
  CHECK (q.exec());


  CHECK (q.prepare ("SELECT "
                    "  first_engine_id, second_engine_id, "
                    "  first_is_black, "
                    "  board_size, rule_set, komi "
                    "FROM game  "
                    "JOIN experiment ON game.experiment_id = experiment.id "
                    "JOIN game_setup ON experiment.game_setup_id = game_setup.id "
                    "WHERE game.id = ?"));

  q.addBindValue (id);
  CHECK (q.exec ());
  CHECK (q.next ());
  int first_id = q.record().value ("first_engine_id").toInt();
  int second_id = q.record().value ("second_engine_id").toInt();
  first_is_black = q.record().value ("first_is_black").toBool();
  board_size = q.record().value ("board_size").toInt();
  rule_set = q.record().value ("rule_set").toString();
  komi = q.record().value ("komi").toDouble();
  CHECK (!q.next());

  first.Get (db, first_id);
  second.Get (db, second_id);
  first_params.clear();
  second_params.clear();

  CHECK (q.prepare ("SELECT name, value, for_first "
                    "FROM engine_param "
                    "WHERE game_id = ?"));
  q.addBindValue (id);
  CHECK (q.exec ());

  while (q.next ()) {
    QString param_name = q.record().value ("name").toString();
    QString param_value = q.record().value ("value").toString();
    CHECK (param_name != QString());
    CHECK (param_value != QString());
    if (q.record().value ("for_first").toInt()) {
      first_params.append (qMakePair (param_name, param_value));
    } else {
      second_params.append (qMakePair (param_name, param_value));
    }
  }
  
  return true;
}


void DbGame::Finish (QSqlDatabase db,
                     bool first_won,
                     QVariant first_report,
                     QVariant second_report,
                     QString sgf)
{
  QSqlQuery q (db);
  CHECK (q.prepare ("UPDATE game SET "
                    "  first_won = ?, "
                    "  sgf = ?, "
                    "  first_report = ?, "
                    "  second_report = ?, "
                    "  finished_at = NOW() "
                    "WHERE id = ?"));
  q.addBindValue (first_won);
  q.addBindValue (sgf);
  q.addBindValue (first_report);
  q.addBindValue (second_report);
  q.addBindValue (id);
  CHECK (q.exec ());
}
