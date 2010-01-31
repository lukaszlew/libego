#include <QDir>
#include <QString>
#include <QtDebug>

#include "database.hpp"

#include "admin.hpp"

int main () {
  srand (time (0));

  Database db;
  if (!db.ProcessSettingsFile (QDir::homePath() + "/.my.cnf") || !db.Connect ()) {
    return 1;
  }

  Admin admin (db);
  admin.Run ();
  return 0;
}
