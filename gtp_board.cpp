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





class gtp_board_t : public gtp_engine_t {

  stack_board_t& board;

public:

  gtp_board_t (stack_board_t& board_) : board (board_) { }

  virtual vector <string> get_command_names () const {
    vector <string> commands;
    commands.push_back ("boardsize");
    commands.push_back ("clear_board");
    commands.push_back ("komi");
    commands.push_back ("load_position");
    commands.push_back ("play");
    commands.push_back ("undo");
    commands.push_back ("showboard");
    commands.push_back ("playout_benchmark");
    return commands;
  };


  virtual gtp_status_t exec_command (string command, istream& params, ostream& response) {


    if (command == "boardsize") {
      int new_board_size;;
      if (!(params >> new_board_size)) return gtp_syntax_error;
      if (new_board_size != int (board_size)) { 
        response << "unacceptable size"; 
        return gtp_failure; 
      }
      return gtp_success;
    }


    if (command == "clear_board") {
      board.clear ();
      return gtp_success;
    }


    if (command == "komi") {
      float new_komi;
      if (!(params >> new_komi)) return gtp_syntax_error;
      board.set_komi (new_komi);
      return gtp_success;
    }


    if (command == "load_position") {
      string file_name;
      if (!(params >> file_name)) return gtp_syntax_error;

      ifstream fin (file_name.data ()); // TODO cant use string directly ??

      if (!fin)        { response << "no such file: " << file_name; return gtp_failure; }
      if (!board.load (fin)) { response << "wrong file format";           return gtp_failure; }
      return gtp_success;
    }


    if (command == "play") {
      player_t  pl;
      vertex_t   v;
      if (!(params >> pl >> v)) return gtp_syntax_error;
  
      if (!board.try_play (pl, v)) {
        response << "illegal move";
        return gtp_failure;
      }

      return gtp_success;
    }


    if (command == "undo") {
      if (!board.try_undo ()) {
        response << "too many undo";
        return gtp_failure;
      }
      return gtp_success;
    }
    

    if (command == "showboard") {
      response << endl << board.act_board ()->to_string();
      return gtp_success;
    }

    if (command == "playout_benchmark") {
      uint playout_cnt;
      if (!(params >> playout_cnt)) return gtp_syntax_error;
      simple_playout_benchmark::run (board.act_board (), playout_cnt, player_black, response);
      return gtp_success;
    }

    fatal_error ("wrong command in gtp_board_t::exec_command");
    return gtp_panic; // formality 
  } 
  // end of exec_command

};

