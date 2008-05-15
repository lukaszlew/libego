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

    if (v == vertex_t::resign ()) assert (false); // TODO


    if (v != vertex_t::pass () && board->color_at [v] != color_t::empty ()) 
      return false; 

    if (!board->is_legal (player,v))
      return false;

    board->play_legal (player, v);

    if (board->last_move_status != play_ok)
      { board->undo (); return false; } // TODO assert success of undo

    if (board->is_hash_repeated ())    // superko test
      { board->undo (); return false; } // -------||---------

    return true;
  }

  bool try_undo () {
    return board->undo ();
  }

  bool is_legal (player_t pl, vertex_t v) {            // slow function
    bool undo_res;
    if (!try_play (pl, v)) return false;
    undo_res = board->undo ();
    assertc (stack_board_ac, undo_res == true);

    return true;
  }

  void set_komi (float komi) {  // TODO is it proper semantics ?
    board->set_komi (komi);
  }

  bool load (istream& ifs) {
    board_t tmp[1];
    if (!tmp->load (ifs)) return false;
    clear ();
    board->load (tmp);
    return true;
  }
};

