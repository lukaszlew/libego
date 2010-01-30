#ifndef ADMIN_HPP_
#define ADMIN_HPP_

#include <cmath>

#include "database.hpp"
#include "gtp.hpp"
#include "CBAST.h"

struct Param {
  QString name;
  double min_value;
  double max_value;
  enum Function {
    Id,
    Pow10,
    Log10,
    Sqrt,
    Square
  } fun;

  bool SetFun (QString f) {
    if (f == "id")     { fun = Id;     return true; }
    if (f == "pow10")  { fun = Pow10;  return true; }
    if (f == "log10")  { fun = Log10;  return true; }
    if (f == "sqrt")   { fun = Sqrt;   return true; }
    if (f == "square") { fun = Square; return true; }
    return false;
  }
  
  double OfBast (double bval) {
    double val = (max_value - min_value) * (bval + 1.0) / 2.0 + min_value;
    switch (fun) {
    case Pow10:  val = pow (10, val); break;
    case Log10:  val = log10(val); break;
    case Sqrt:   val = sqrt(val); break;
    case Square: val = val * val; break;
    case Id: break;
    default: break;
    }
    return val;
  }
};



class Admin {
public:
  Admin (Database& db);
  ~Admin ();
  void Run ();

private:
  void CAddGameSetup (Gtp::Io& io);
  void CAddEngineSearchPath (Gtp::Io& io);

  void CSetEngineCommandLine (Gtp::Io& io);
  void CAddEngineConfigLine (Gtp::Io& io);
  void CAddEngine (Gtp::Io& io);


  void CSetExperimentDescription (Gtp::Io& io);
  void CSetExperimentEngine (Gtp::Io& io);
  void CAddExperimentParam (Gtp::Io& io);
  void CAddExperiment (Gtp::Io& io);

  void CCloseAllExperiments (Gtp::Io& io);

  void SetPvBoth ();
  void SetPvBastFirst (CBAST& bast, int bast_id);
  void CLoopAddGames (Gtp::Io& io);

  bool AddEngine (QString name, QString config_file, QString command_line);

  void CExtractCsv (Gtp::Io& io);

private:
  Database& db;

  QString config;
  QString command_line;

  QString first_engine;
  QString second_engine;

  ParamsValues pv_first, pv_second;

  QString experiment_description;
  QList <Param> experiment_params;

  int experiment_id;
  // (for_first_engine, param_name) -> list of values
};

#endif
