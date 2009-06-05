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

#include <iostream>

#include "ego.h"
#include "benchmark.h"
#include "uct.cpp"
#include "experiments.cpp"

Gtp      gtp;
FullBoard   board;
SgfTree  sgf_tree;

BasicGtp    basic_gtp (gtp, board);
SgfGtp      sgf_gtp   (gtp, sgf_tree, board);
AllAsFirst  aaf (gtp, board);

GenmoveGtp<Uct>  genmove_gtp (gtp, board);

int main(int argc, char** argv) {
  // no buffering to work well with gogui
  setbuf (stdout, NULL);
  setbuf (stderr, NULL);

  reps (ii, 1, argc) {
    string arg = argv[ii];

    if (arg == "--seed") {
      uint new_seed;
      if (ii+1 < (uint)argc &&
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
      if (ii+1 < (uint)argc) {
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
      if (ii+1 == (uint)argc) {
        cerr << "Fatal: no config file given" << endl;
        return 1;
      }
      ii += 1;
      string config = argv[ii];
      if (gtp.run_file (config, cout) == false) {
        cerr << "Fatal: GTP file not found: " << config << endl;
        return 1;
      }
      continue;
    }

    cerr << "Fatal: unknown option: " << arg << endl;
    return 1;
  }

  gtp.run_loop (cin, cout);

  return 0;
}
