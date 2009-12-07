#ifndef WORKER_HPP_
#define WORKER_HPP_

#include "database.hpp"
#include "full_board.hpp"

class Worker {
public:
  Worker (Database& db);
  ~Worker ();
  void Run ();

private:
  bool ConnectDatabase ();
  bool GrabJob ();

private:
  Database& db;
  DbGame db_game;
  FullBoard board;
};

#endif
