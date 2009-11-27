#include <QHostInfo>
#include <QSettings>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QString>
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


bool Database::AddEngineSearchPath (QString path)
{
  QSqlQuery q (db);
  CHECK (q.prepare ("INSERT INTO engine_search_path (path) VALUES (?)"));
  q.addBindValue (path);
  return q.exec ();
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


bool Database::AddExperiment (QString name, QString description) {
  QSqlQuery q (db);
  CHECK (q.prepare ("INSERT INTO experiment (name, description) "
                    "VALUES (?, ?)"));
  q.addBindValue (name);
  q.addBindValue (description);
  return q.exec ();
}


bool Database::AddGame (QString experiment,
                        QString game_setup,
                        QString first_engine,
                        QString second_engine)
{
  QSqlQuery q (db);
  QString query = 
    "INSERT INTO game "
    "  (experiment_id, game_setup_id, black_engine_id, "
    "   white_engine_id, created_at) "
    "VALUES ("
    "  (SELECT id FROM experiment WHERE name = ?),"
    "  (SELECT id FROM game_setup WHERE name = ?),"
    "  (SELECT id FROM engine WHERE name = ?),"
    "  (SELECT id FROM engine WHERE name = ?),"
    "  NOW()"
    ")";
  CHECK (q.prepare (query));
  q.addBindValue (experiment);
  q.addBindValue (game_setup);
  q.addBindValue (first_engine);
  q.addBindValue (second_engine);
  bool ok = q.exec ();
  if (!ok) {
    qDebug() << q.lastError();
  }
  return ok;
}


QStringList Database::EngineSearchPath()
{
  QSqlQuery q (db);
  QStringList list;
  CHECK (q.exec ("SELECT path FROM engine_search_path"));
  while (q.next()) {
    list.append (q.value(0).toString());
  }
  return list;
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


  CHECK (q.prepare ("SELECT game_setup_id, "
                    "       black_send_gtp_config, white_send_gtp_config, "
                    "       black_engine_id, white_engine_id "
                    "FROM game WHERE id = ?"));
  q.addBindValue (id);
  CHECK (q.exec ());
  CHECK (q.next ());
  int game_setup_id = q.record().value ("game_setup_id").toInt();
  black_gtp_config = q.record().value ("black_gtp_config").toString();
  white_gtp_config = q.record().value ("white_gtp_config").toString();
  int black_id = q.record().value ("black_engine_id").toInt();
  int white_id = q.record().value ("white_engine_id").toInt();
  CHECK (!q.next());

  black.Get (db, black_id);
  white.Get (db, white_id);
  
  CHECK (q.prepare ("SELECT board_size, komi FROM game_setup WHERE id = ?"));
  q.addBindValue (game_setup_id);
  CHECK (q.exec ());
  CHECK (q.next ());
  rule_set = q.record().value ("rule_set").toString();
  board_size = q.record().value ("board_size").toInt();
  komi = q.record().value ("komi").toDouble();
  CHECK (!q.next());

  return true;
}


void DbGame::Finish (QSqlDatabase db,
                     bool black_won,
                     QVariant black_gtp_game_report,
                     QVariant white_gtp_game_report,
                     QString sgf)
{
  QSqlQuery q (db);
  CHECK (q.prepare ("UPDATE game SET "
                    "  black_won = ?, "
                    "  sgf = ?, "
                    "  black_gtp_game_report = ?, "
                    "  white_gtp_game_report = ?, "
                    "  finished_at = NOW() "
                    "WHERE id = ?"));
  q.addBindValue (black_won);
  q.addBindValue (sgf);
  q.addBindValue (black_gtp_game_report);
  q.addBindValue (white_gtp_game_report);
  q.addBindValue (id);
  CHECK (q.exec ());
}
