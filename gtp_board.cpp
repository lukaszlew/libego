/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                           *
 *  This file is part of GoBoard library,                                    *
 *                                                                           *
 *  Copyright 2006 Lukasz Lew                                                *
 *                                                                           *
 *  GoBoard library is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by     *
 *  the Free Software Foundation; either version 2 of the License, or        *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  GoBoard library is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public License        *
 *  along with GoBoard library; if not, write to the Free Software           *
 *  Foundation, Inc., 51 Franklin St, Fifth Floor,                           *
 *  Boston, MA  02110-1301  USA                                              *
 *                                                                           *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "playout.cpp"
#include "gtp.h"



// state of engine is in this board

board_t gtp_board;



// gtp_* functions


static int gtp_boardsize (char* s) {
  int new_board_size;
  decode_int (s, new_board_size);

  if (new_board_size != int (board_size)) return gtp_failure ("size not supported");
  return gtp_success("");
}

static int gtp_komi (char *s) {
  float new_komi;
  decode_float (s, new_komi);

  gtp_board.set_komi (-new_komi);
  return gtp_success("");
}


static int gtp_clear_board (char* s) {
  unused (s);
  gtp_board.clear ();
  return gtp_success("");
}

static int gtp_load_position (char* s) {
  char f_name[1000];

  decode_str (s, f_name);
  ifstream fin (f_name);

  if (!fin)                   return gtp_failure ("no such file \"%s\"", f_name);
  if (!gtp_board.load (fin))  return gtp_failure ("wrong file format");

  return gtp_success("");
}


static int gtp_play (char* s) {

  move::t move;
  decode_move (s, move, return gtp_failure ("syntax error"));

  if (!gtp_board.slow_is_legal (move))
    return gtp_failure ("illegal move");
  
  if (gtp_board.play (move) != board_t::play_ok) 
    gtp_panic (); // should not happen
  
  return gtp_success("");
}

static int gtp_showboard (char* s) {
  unused (s);
  ostringstream ss;
  
  gtp_board.print (ss);

  gtp_start_response(GTP_SUCCESS);
  gtp_printf ("\n%s", ss.str ().data ());
  return gtp_finish_response();
}

static int gtp_playout_benchmark (char *s)
{
  int playout_cnt;

  decode_int (s, playout_cnt);

  ostringstream ss;
  simple_playout::benchmark (&gtp_board, playout_cnt, player::black, ss);

  gtp_start_response(GTP_SUCCESS);
  gtp_printf ("\n%s", ss.str ().data ());
  return gtp_finish_response();
}

gtp_command gtp_board_commands [] = {
  { "boardsize",         gtp_boardsize },
  { "clear_board",       gtp_clear_board },
  { "komi",              gtp_komi },
  { "load_position",     gtp_load_position },
  { "play",              gtp_play },
  { "showboard",         gtp_showboard },
  { "playout_benchmark", gtp_playout_benchmark },
  { NULL, NULL }
};
