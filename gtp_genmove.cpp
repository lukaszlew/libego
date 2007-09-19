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

class gtp_genmove_t : public gtp_engine_t {

  stack_board_t& board;

public:

  gtp_genmove_t (stack_board_t& board_, uct_t& uct_) : board (board_) {} //, uct (uct_) { }

  virtual vector <string> get_command_names () const {
    vector <string> commands;
    commands.push_back ("genmove");
    return commands;
  };


  virtual gtp_status_t exec_command (string command, istream& params, ostream& response) {

    if (command == "genmove") {
      player_t  player;
      vertex_t   v;
      if (!(params >> player)) return gtp_syntax_error;
  
      uct_t* uct = new uct_t (board); // TODO Why we need to allocate?
      v = uct->genmove (player);
      delete uct;

      cerr << v << endl;
      if (!board.try_play (player, v)) fatal_error ("genmove: generated illegal move");
      response << v;

      return gtp_success;
    }

    fatal_error ("wrong command in gtp_genmove_t::exec_command");
    return gtp_panic; // formality 
  } 
  // end of exec_command
};
