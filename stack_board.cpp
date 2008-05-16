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


// class stack_board_t


class stack_board_t {

public:

  board_t board [1];//   stack [max_stack_size];

public: 
  
  void check () {
    if (!stack_board_ac) return;
  }

  stack_board_t () { }

  void clear () {
    board->clear ();
  }

  board_t* const act_board () { 
    return board;
  }

  bool try_play (player_t player, vertex_t v) {
    return board->try_play (player, v);
  }

  bool try_undo () {
    return board->undo ();
  }

  bool is_legal (player_t player, vertex_t v) {
    return board->is_strict_legal (player, v);
  }

  void set_komi (float komi) {  // TODO is it proper semantics ?
    board->set_komi (komi);
  }

  bool load (istream& ifs) {
    return board->load_from_ascii (ifs);
  }
};

