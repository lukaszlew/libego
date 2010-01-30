#include <QDir>
#include <QString>
#include <QtDebug>

#include "database.hpp"

#include "worker.hpp"

int main () {
  srand (time (0));

  Database db;
  if (!db.ProcessSettingsFile (QDir::homePath() + "/.my.cnf") || !db.Connect ()) {
    return 1;
  }

  Worker worker (db);
  worker.Run ();
  return 0;
}
