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





class GtpBoard : public GtpEngine {

  Board& board;

public:

  GtpBoard (Gtp& gtp, Board& board_) : board (board_) { 
    gtp.add_gtp_command (this, "boardsize");
    gtp.add_gtp_command (this, "clear_board");
    gtp.add_gtp_command (this, "komi");
    gtp.add_gtp_command (this, "load_position");
    gtp.add_gtp_command (this, "play");
    gtp.add_gtp_command (this, "undo");
    gtp.add_gtp_command (this, "showboard");
    gtp.add_gtp_command (this, "benchmark");
  }

  virtual GtpStatus exec_command (string command, istream& params, ostream& response) {


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
      if (!board.load_from_ascii (fin)) { response << "wrong file format";           return gtp_failure; }
      return gtp_success;
    }


    if (command == "play") {
      Player pl;
      Vertex v;
      if (!(params >> pl >> v)) return gtp_syntax_error;
  
      if (v != Vertex::resign () && board.try_play (pl, v) == false) {
        response << "illegal move";
        return gtp_failure;
      }

      return gtp_success;
    }


    if (command == "undo") {
      if (board.undo () == false) {
        response << "too many undo";
        return gtp_failure;
      }
      return gtp_success;
    }
    

    if (command == "showboard") {
      response << endl << board.to_string();
      return gtp_success;
    }

    if (command == "benchmark") {
      uint playout_cnt;
      string p;
      if (!(params >> playout_cnt)) return gtp_syntax_error;
      if (!(params >> p) || p != "+") {
        simple_playout_benchmark::run<false> (&board, playout_cnt, response);
      } else {
        simple_playout_benchmark::run<true>  (&board, playout_cnt, response);
      }
      return gtp_success;
    }

    fatal_error ("wrong command in GtpBoard::exec_command");
    return gtp_panic; // formality 
  } 
  // end of exec_command

};

