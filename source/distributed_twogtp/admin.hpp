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
  void CAddGameSetup (Gtp::Io& io);
  void CAddEngineSearchPath (Gtp::Io& io);

  void CSetEngineCommandLine (Gtp::Io& io);
  void CSetEngineConfig (Gtp::Io& io);
  void CAddEngine (Gtp::Io& io);


  void CSetExperimentDescription (Gtp::Io& io);
  void CSetExperimentEngine (Gtp::Io& io);
  void CAddExperiment (Gtp::Io& io);

  void CAddParamValue (Gtp::Io& io);
  void CAddGames (Gtp::Io& io);

  bool AddEngine (QString name, QString config_file, QString command_line);

private:
  Database& db;

  QString config_file;
  QString command_line;

  QString first_engine;
  QString second_engine;

  QString experiment_description;

  QMap <QString, QString> params[2];
};

#endif
