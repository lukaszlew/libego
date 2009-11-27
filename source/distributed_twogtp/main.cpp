#include <QString>
#include <QtDebug>

#include "database.hpp"

#include "admin.hpp"
#include "worker.hpp"

void usage (const char* file) {
  qWarning ()
    << "Usage: " << file << " [--worker | --admin]";
}

int main (int argc, char** argv) {
  if (argc != 2) {
    usage (argv[0]);
    return 1;
  }

  Database db;
  if (!db.ProcessSettingsFile ("~/.my.cnf") || !db.Connect ()) {
    return 1;
  }

 if (QString (argv[1]) == "--admin") {
    Admin admin (db);
    admin.Run ();
    return 0;
  }

  if (QString (argv[1]) == "--worker") {
    Worker worker (db);
    worker.Run ();
    return 0;
  }
}
