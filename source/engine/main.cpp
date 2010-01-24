/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                           *
 *  This file is part of Library of Effective GO routines - EGO library      *
 *                                                                           *
 *  Copyright 2006 and onwards, Lukasz Lew                                   *
 *                                                                           *
 *  EGO library is free software; you can redistribute it and/or modify      *
 *  it under the terms of the GNU General Public License as published by     *
 *  the Free Software Foundation; either version 2 of the License, or        *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  EGO library is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public License        *
 *  along with EGO library; if not, write to the Free Software               *
 *  Foundation, Inc., 51 Franklin St, Fifth Floor,                           *
 *  Boston, MA  02110-1301  USA                                              *
 *                                                                           *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <algorithm>
#include <boost/foreach.hpp>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

// private libraries
#include "gtp_gogui.hpp"
#include "ego.hpp"
#include "to_string.hpp"


static const bool kCheckAsserts = false;

// local hard-include
#include "param.hpp"
#include "stat.hpp"
#include "logger.hpp"

#include "mcts_tree.hpp"
#include "mcts_mcmc.hpp"
#include "mcts_playout.hpp"
#include "mcts_engine.hpp"
#include "mcts_gtp.hpp"

#include "experiments.cpp"

// TODO automatize through CMake (and add git SHA1)
#ifndef VERSION
#define VERSION unknown
#endif

void Cbenchmark (Gtp::Io& io) {
  uint n = io.Read<uint> (100000);
  io.CheckEmpty ();
  io.out << Benchmark::Run (n);
}

int main(int argc, char** argv) {
  // no buffering to work well with gogui
  setbuf (stdout, NULL);
  setbuf (stderr, NULL);
  srand48 (123);

  Gtp::ReplWithGogui gtp;

  gtp.RegisterStatic("name", "Libego");
  gtp.RegisterStatic("version", STRING(VERSION));
  gtp.RegisterStatic("protocol_version", "2");
  gtp.Register ("benchmark", Cbenchmark);

  MctsEngine mcts_engine;
  MctsGtp mcts_gtp (gtp, mcts_engine);

  reps (ii, 1, argc) {
    if (ii == argc-1 && string (argv[ii]) == "gtp") continue;
    string response;
    switch (gtp.RunOneCommand (argv[ii], &response)) {
    case Gtp::Repl::Success:
      cerr << response << endl;
      break;
    case Gtp::Repl::Failure:
      cerr << "Command: \"" << argv[ii] << "\" failed." << endl;
      return 1;
    case Gtp::Repl::NoOp:
      break;
    case Gtp::Repl::Quit:
      cerr << response << endl;
      return 0;
    }
  }

  if (argc == 1 || string(argv[argc-1]) == "gtp") {
    gtp.Run (cin, cout);
  }

  return 0;
}
