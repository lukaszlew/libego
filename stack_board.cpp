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

  vector <board_t> stack;//   stack [max_stack_size];

public: 
  
  void check () {
    if (!stack_board_ac) return;
    assertc (stack_board_ac, stack.size () >= 1);
  }

  stack_board_t () {
    clear ();
  }

  void clear () {
    stack.resize (1);
  }

  board_t* const act_board () { 
    return &(stack.back ()); 
  }

  bool try_play (player_t player, vertex_t v) {

    record_state (); // for undo

    if (v == vertex_pass) return true;

    if (v == vertex_resign) return true;

    if (act_board ()->color_at[v] != color::empty) 
      { revert_state (); return false; }

    if (act_board ()->play_no_pass (player, v) != play_ok)
      { revert_state (); return false; }

    if (is_hash_repeated ())    // superko test
      { revert_state (); return false; }

    return true;
  }

  bool try_undo () {
    if (stack.size () <= 1) return false;
    revert_state ();
    return true;
  }

  bool is_legal (player_t pl, vertex_t v) {            // slow function
    bool undo_res;
    if (!try_play (pl, v)) return false;
    undo_res = try_undo ();
    assertc (stack_board_ac, undo_res == true);

    return true;
  }

  void record_state () {
    stack.resize (stack.size () + 1);
    stack.back ().load (&stack [stack.size () - 2]); // for undo-ing
    check ();
  }

  void revert_state () {
    stack.pop_back ();
    check ();
  }

  bool is_hash_repeated () {
    dseq (ii, stack.size () - 2, 0) {
      if (stack[ii].hash == act_board ()->hash) return true;      
    }
    return false;
  }

  void set_komi (float komi) {  // TODO is it proper semantics ?
    for_each (board_it, stack) 
      board_it->set_komi (komi);
  }

  bool load (istream& ifs) {
    board_t tmp[1];
    if (!tmp->load (ifs)) return false;
    clear ();
    act_board ()->load (tmp);
    return true;
  }
};

