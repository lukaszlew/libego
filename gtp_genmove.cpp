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

template <typename engine_t>
class GtpGenmove : public GtpEngine {

  Board& board;

public:

  GtpGenmove (Gtp& gtp, Board& board_, engine_t& engine_) : board (board_) { //, engine (engine_) { }
    gtp.add_gtp_command (this, "genmove");
  } 

  virtual GtpResult exec_command (string command, istream& params) {

    if (command == "genmove") {
      Player  player;
      Vertex   v;
      if (!(params >> player)) return GtpResult::syntax_error ();
  
      engine_t* engine = new engine_t (board); // TODO Why we need to allocate?
      v = engine->genmove (player);
      delete engine;
      
      

      if (v != Vertex::resign () &&
          board.try_play (player, v) == false)
        fatal_error ("genmove: generated illegal move");

      return GtpResult::success (to_string (v));
    }

    fatal_error ("wrong command in GtpGenmove::exec_command");
    assert (false);
  } 
  // end of exec_command
};
