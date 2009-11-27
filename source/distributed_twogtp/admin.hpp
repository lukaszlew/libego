#ifndef ADMIN_HPP_
#define ADMIN_HPP_

#include "database.hpp"
#include "gtp.hpp"

class Admin {
public:
  Admin (Database& db);
  ~Admin ();
  void Run ();

private:
  void CAddEngineSearchPath (Gtp::Io& io);
  void CAddEngine (Gtp::Io& io);
  void CAddGameSetup (Gtp::Io& io);
  void CAddExperiment (Gtp::Io& io);
  void CAddGames (Gtp::Io& io);

  bool AddEngine (QString name, QString config_file, QString command_line);

private:
  Database& db;
};

#endif
