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

gtp_command* gtp_commands;      // list_commands needs this


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

static int gtp_quit (char *s) {
  unused (s);
  gtp_success ("");
  return GTP_QUIT;
}

static int gtp_showboard (char* s) {
  unused (s);
  ostringstream ss;
  
  gtp_board.print (ss);

  gtp_start_response(GTP_SUCCESS);
  gtp_printf ("\n%s", ss.str ().data ());
  return gtp_finish_response();
}

static int gtp_list_commands (char *s)
{
  unused (s);

  gtp_start_response(GTP_SUCCESS);

  gtp_printf("\n");

  for (gtp_command* gc = gtp_commands; gc->name != NULL; gc++)
    gtp_printf("%s\n", gc->name);

  gtp_printf("\n");             // we already have one \n
  fflush(stdout);
  return GTP_OK;
}

static int gtp_name (char *s)
{
  unused (s);

  gtp_start_response(GTP_SUCCESS);
  gtp_printf("libgoboard (part of ZG-1 bot)");
  return gtp_finish_response ();
}

static int gtp_protocol_version (char *s)
{
  unused (s);

  gtp_start_response(GTP_SUCCESS);
  gtp_printf("2");
  return gtp_finish_response ();
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

static int gtp_echo (char *s)
{
  unused (s);

  gtp_start_response(GTP_SUCCESS);
  gtp_printf("%s", s);
  return gtp_finish_response ();
}

gtp_command gtp_commands_static [] = {
  { "boardsize",         gtp_boardsize },
  { "clear_board",       gtp_clear_board },
  { "help",              gtp_list_commands },
  { "komi",              gtp_komi },
  { "list_commands",     gtp_list_commands },
  { "load_position",     gtp_load_position },
  { "name",              gtp_name },
  { "play",              gtp_play },
  { "protocol_version",  gtp_protocol_version },
  { "quit",              gtp_quit },
  { "showboard",         gtp_showboard },
  { "playout_benchmark", gtp_playout_benchmark },
  { "echo",              gtp_echo },
  { NULL, NULL }
};

// main

int main () { 

  gtp_commands = gtp_commands_static; // what an crude language C++ is ...
  pm::srand (123);

  setvbuf (stdout, (char *)NULL, _IONBF, 0);
  setvbuf (stderr, (char *)NULL, _IONBF, 0);

  gtp_internal_set_boardsize (board_size);

  FILE* gtp_default = fopen ("automagic.gtp", "r");

  if (gtp_default) gtp_main_loop (gtp_commands, gtp_default, stdout, NULL);

  gtp_main_loop (gtp_commands, stdin, stdout, NULL);

  return 0;
}

