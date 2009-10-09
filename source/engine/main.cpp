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

#define FOREACH BOOST_FOREACH

// local hard-include
#include "stat.h"

#include "mcts_tree.h"
#include "mcts.cpp"
#include "mcts_gtp.cpp"

#include "playout_gfx.cpp"

#include "experiments.cpp"

// TODO automatize through CMake (and add git SHA1)
#ifndef VERSION
#define VERSION unknown
#endif

Gtp::ReplWithGogui gtp;

FullBoard board;
SgfTree sgf_tree;

BasicGtp    basic_gtp (gtp, board);
//SgfGtp      sgf_gtp   (gtp, sgf_tree, board);

Mcts    mcts      (board);
MctsGtp mcts_gtp  (gtp, board, mcts);

PlayoutGfx<Mcts> playout_gfx(gtp, mcts, "MCTS.");

// Uncomment this to enable All-As-First experiment (new GTP commands).
// AllAsFirst  aaf (gtp, board);

int main(int argc, char** argv) {
  // no buffering to work well with gogui
  setbuf (stdout, NULL);
  setbuf (stderr, NULL);

  reps (ii, 1, argc) {
    string arg = argv[ii];

    if (arg == "--seed") {
      uint new_seed;
      if (ii+1 < argc &&
          string_to<uint>(argv[ii+1], &new_seed)) {
          ii += 1;
          global_random.set_seed(new_seed);
          continue;
      } else {
        cerr << "Fatal: no seed value given" << endl;
        return 1;
      }
    }

    if (arg == "--benchmark" || arg == "-b") {
      int playout_cnt = 200;
      if (ii+1 < argc) {
        if (string_to<int>(argv[ii+1], &playout_cnt)) {
          ii += 1;
        }
      }
      playout_cnt *= 1000;
      cout << "Benchmarking, please wait ..." << flush;
      cout << Benchmark::run (playout_cnt) << endl;
      return 0;
    }
    
    if (arg == "--run-gtp-file" || arg == "-r") {
      if (ii+1 == argc) {
        cerr << "Fatal: no config file given" << endl;
        return 1;
      }
      ii += 1;
      string config = argv[ii];

      ifstream in (argv[ii]);
      if (in) {
        gtp.Run (in, cerr);
        in.close ();
        continue;
      } else {
        return false;
      }
    }

    cerr << "Fatal: unknown option: " << arg << endl;
    return 1;
  }

  gtp.RegisterStatic("name", "Libego");
  gtp.RegisterStatic("version", STRING(VERSION));
  gtp.RegisterStatic("protocol_version", "2");

  gtp.Run (cin, cout);

  return 0;
}
