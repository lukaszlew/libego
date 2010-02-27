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
#include <iomanip>
#include <sstream>
#include <vector>

// private libraries
#include "gtp_gogui.hpp"
#include "ego.hpp"
#include "to_string.hpp"

Gtp::ReplWithGogui gtp;

static const bool kCheckAsserts = false;

// local hard-include
#include "param.hpp"
#include "stat.hpp"
#include "logger.hpp"

#include "time_control.hpp"
#include "mcts_tree.hpp"
#include "mcts_playout.hpp"
#include "engine.hpp"
#include "mcts_gtp.hpp"

#include "all_hash3x3.hpp"
All2051Hash3x3 all_hash3x3;

#include "mm.hpp"
#include "mm_train.hpp"

// TODO automatize through CMake (and add git SHA1)
#ifndef VERSION
#define VERSION unknown
#endif

void GtpBenchmark (Gtp::Io& io) {
  uint n = io.Read<uint> (100000);
  io.CheckEmpty ();
  io.out << Benchmark::Run (n);
}

void GtpBoardTest (Gtp::Io& io) {
  bool print_moves = io.Read<bool> (false);
  io.CheckEmpty ();
  PlayoutTest (print_moves);
}

void GtpSamplerTest (Gtp::Io& io) {
  bool print_moves = io.Read<bool> (false);
  io.CheckEmpty ();
  SamplerPlayoutTest (print_moves);
}

void GtpMmTest (Gtp::Io& io) {
  io.CheckEmpty ();
  Mm::Test ();
}

int main(int argc, char** argv) {
  // no buffering to work well with gogui
  setbuf (stdout, NULL);
  setbuf (stderr, NULL);
  srand48 (123);

  gtp.RegisterStatic("name", "Libego");
  gtp.RegisterStatic("version", STRING(VERSION));
  gtp.RegisterStatic("protocol_version", "2");
  gtp.Register ("benchmark", GtpBenchmark);
  gtp.Register ("board_test", GtpBoardTest);
  gtp.Register ("sampler_test", GtpSamplerTest);
  gtp.Register ("mm_test", GtpMmTest);

  Engine& engine = *(new Engine());
  MctsGtp mcts_gtp (engine);

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

  delete &engine;

  return 0;
}
