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
#include <gtp_gogui.h>
#include <ego.h>

// local hard-include
#include "stat.h"
#include "logger.h"

#include "mcts_tree.h"
#include "mcts_playout.h"
#include "mcts_engine.h"
#include "mcts_gtp.h"


#include "experiments.cpp"

// TODO automatize through CMake (and add git SHA1)
#ifndef VERSION
#define VERSION unknown
#endif

void Cbenchmark (Gtp::Io& io) {
  uint n = io.Read<uint> (100000);
  io.CheckEmpty ();
  io.out << Benchmark::run (n);
}

int main(int argc, char** argv) {
  // no buffering to work well with gogui
  setbuf (stdout, NULL);
  setbuf (stderr, NULL);

  Gtp::ReplWithGogui gtp;

  gtp.RegisterStatic("name", "Libego");
  gtp.RegisterStatic("version", STRING(VERSION));
  gtp.RegisterStatic("protocol_version", "2");
  gtp.Register ("benchmark", Cbenchmark);

  MctsEngine mcts_engine;
  MctsGtp mcts_gtp (gtp, mcts_engine);

  reps (ii, 1, argc) {
    string arg = argv[ii];
    if (arg == "-") {
      gtp.Run (cin, cout);
      continue;
    }

    ifstream in (arg.c_str());
    if (in) {
      gtp.Run (in, cerr);
      in.close ();
    } else {
      cerr << "? Warning: GTP file not found :\"" << arg << "\"." << endl;
    }
  }

  if (argc == 1) {
    gtp.Run (cin, cout);
  }

  return 0;
}
