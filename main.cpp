/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                           *
 *  This file is part of Library of Effective GO routines - EGO library      *
 *                                                                           *
 *  Copyright 2006, 2007 Lukasz Lew                                          *
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
#include <fstream>
#include <sstream>

#include <cassert>
#include <cmath>
#include <cstdarg>
#include <cctype>

#include <vector>
#include <map>

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

using namespace std;

#include "config.cpp"
#include "utils.cpp"
#include "basic_go_types.cpp"
#include "board.cpp"
#include "stack_board.cpp"
#include "playout.cpp"
#include "uct.cpp"

#include "gtp.cpp"
#include "gtp_board.cpp"
#include "gtp_genmove.cpp"

#include "experiments.cpp"

// main

int main () { 
  setvbuf (stdout, (char *)NULL, _IONBF, 0);
  setvbuf (stderr, (char *)NULL, _IONBF, 0);

  // setup GTP machinery
  
  stack_board_t stack_board;
  uct_t         uct (stack_board);

  gtp_t         gtp;

  gtp_static_commands_t sc;
  sc.add ("protocol_version", "2");
  sc.add ("name", "libEGO");
  sc.add ("gogui_analyze_commands", ""); // to be extended

  gtp.register_engine (sc);
  
  gtp_board_t   gtp_board   (stack_board);
  gtp.register_engine (gtp_board);

  gtp_genmove_t gtp_genmove (stack_board, uct);
  gtp.register_engine (gtp_genmove);

  all_as_first_t aaf (stack_board, sc);
  gtp.register_engine (aaf);

  gtp.run_file ("automagic.gtp");
  gtp.run_loop ();

  return 0;
}
