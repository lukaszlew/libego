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

#define declare_gtp_fun(fun) static int fun(char *s)

declare_gtp_fun (gtp_boardsize);
declare_gtp_fun (gtp_clear_board);
declare_gtp_fun (gtp_list_commands);
declare_gtp_fun (gtp_komi);
declare_gtp_fun (gtp_load_position);
declare_gtp_fun (gtp_name);
declare_gtp_fun (gtp_play);
declare_gtp_fun (gtp_protocol_version);
declare_gtp_fun (gtp_quit);
declare_gtp_fun (gtp_showboard);

declare_gtp_fun (gtp_playout_benchmark);

gtp_command gtp_commands[] = {
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
  { NULL, NULL }
};

static int gtp_boardsize (char* s) {
  uint new_board_size;
  if (sscanf (s, "%d", &new_board_size) < 1) return gtp_failure ("syntax error");
  if (new_board_size != board_size) return gtp_failure ("size not supported");
  return gtp_success("");
}

static int gtp_komi (char *s) {
  float new_komi;
  if (sscanf(s, "%f", &new_komi) < 1) return gtp_failure("syntax error");
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

  sscanf(s, "%s", f_name);
  ifstream fin (f_name);

  if (!fin)                   return gtp_failure ("no such file \"%s\"", f_name);
  if (!gtp_board.load (fin))  return gtp_failure ("wrong file format");

  return gtp_success("");
}


static int gtp_play (char* s) {

  int       gtp_color;
  coord::t  r;
  coord::t  c;

  if (!gtp_decode_move(s, &gtp_color, &r, &c)) 
    return gtp_failure("syntax error");

  player::t  player;
  v::t       v;
  move::t    move;

  player  = (player::t) (2 - gtp_color);
  v       = v::of_rc (r, c);
  move    = move::of_pl_v (player, v);

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
  uint playout_cnt;

  if (sscanf(s, "%u", &playout_cnt) < 1) return gtp_failure("syntax error");

  ostringstream ss;
  playout_benchmark (&gtp_board, playout_cnt, player::black, ss); // TODO

  gtp_start_response(GTP_SUCCESS);
  gtp_printf ("\n%s", ss.str ().data ());
  return gtp_finish_response();
}



// main

int main () { 

  pm::srand(123);

  setvbuf(stdout, (char *)NULL, _IONBF, 0);
  setvbuf(stderr, (char *)NULL, _IONBF, 0);

  gtp_internal_set_boardsize (board_size);

  FILE* gtp_default = fopen("gtp.automagic", "r");

  if (gtp_default) gtp_main_loop (gtp_commands, gtp_default, stdout, NULL);

  gtp_main_loop (gtp_commands, stdin, stdout, NULL);

  return 0;
}
