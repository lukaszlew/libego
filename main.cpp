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
#include "gtp.h"

#include "utils.cpp"
#include "board.cpp"
#include "stack_board.cpp"
#include "playout.cpp"
#include "gtp_board.cpp"



// general gtp_* functions

const int max_gtp_commands = 256;

gtp_command gtp_commands [max_gtp_commands];      // list_commands needs this here



static int gtp_quit (char *s) {
  unused (s);
  gtp_success ("");
  return GTP_QUIT;
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
  gtp_printf("Effective Go Board Library");
  return gtp_finish_response ();
}

static int gtp_protocol_version (char *s)
{
  unused (s);

  gtp_start_response(GTP_SUCCESS);
  gtp_printf("2");
  return gtp_finish_response ();
}

static int gtp_echo (char *s)
{
  unused (s);

  gtp_start_response(GTP_SUCCESS);
  gtp_printf("%s", s);
  return gtp_finish_response ();
}

gtp_command gtp_general_commands [] = {
  { "help",              gtp_list_commands },
  { "list_commands",     gtp_list_commands },
  { "name",              gtp_name },
  { "protocol_version",  gtp_protocol_version },
  { "quit",              gtp_quit },
  { "echo",              gtp_echo },
  { NULL, NULL }
};


// main

int main () { 
  gtp_append_commands (gtp_commands, gtp_general_commands);
  gtp_append_commands (gtp_commands, gtp_board_commands);

  pm::srand (123);

  setvbuf (stdout, (char *)NULL, _IONBF, 0);
  setvbuf (stderr, (char *)NULL, _IONBF, 0);

  gtp_internal_set_boardsize (board_size);

  FILE* gtp_default = fopen ("automagic.gtp", "r");

  if (gtp_default) gtp_main_loop (gtp_commands, gtp_default, stdout, NULL);

  gtp_main_loop (gtp_commands, stdin, stdout, NULL);

  return 0;
}
