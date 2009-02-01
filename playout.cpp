/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                           *
 *  This file is part of Library of Effective GO routines - EGO library      *
 *                                                                           *
 *  Copyright 2006 and onwards, Lukasz Lew                                   *
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

enum playout_status_t { pass_pass, mercy, too_long };

template <typename Policy> class Playout {
public:
  Policy*  policy;
  Board*   board;
  Move*    move_history;
  uint     move_history_length;

  Playout (Policy* policy_, Board*  board_) : policy (policy_), board (board_) {}

  all_inline
  void play_move () {
    policy->prepare_vertex ();
    
    while (true) {
      Player   act_player = board->act_player ();
      Vertex   v          = policy->next_vertex ();

      if (board->is_pseudo_legal (act_player, v) == false) {
        policy->bad_vertex (v);
        continue;
      } else {
        board->play_legal (act_player, v);
        policy->played_vertex (v);
        break;
      }
    }
  }

  all_inline
  playout_status_t run () {
    uint begin_move_no = board->move_no;
    move_history = board->move_history + board->move_no;
    
    policy->begin_playout (board);
    while (true) {
      play_move ();
      
      if (board->both_player_pass ()) {
        move_history_length = board->move_no - begin_move_no;
        policy->end_playout (pass_pass);
        return pass_pass;
      }
      
      if (board->move_no >= max_playout_length) {
        move_history_length = board->move_no - begin_move_no;
        policy->end_playout (too_long);
        return too_long;
      }
      
      if (use_mercy_rule && uint (abs (board->approx_score ())) > mercy_threshold) {
        move_history_length = board->move_no - begin_move_no;
        policy->end_playout (mercy);
        return mercy;
      }
    }
  }
  
};

// ----------------------------------------------------------------------

class SimplePolicy {
protected:

  static FastRandom fr; // TODO make it non-static

  uint to_check_cnt;
  uint act_evi;

  Board* board;
  Player act_player;

public:

  SimplePolicy () : board (NULL) { }

  void begin_playout (Board* board_) { 
    board = board_;
  }

  void prepare_vertex () {
    act_player     = board->act_player ();
    to_check_cnt   = board->empty_v_cnt;
    act_evi        = fr.rand_int (board->empty_v_cnt); 
  }

  Vertex next_vertex () {
    Vertex v;
    while (true) {
      if (to_check_cnt == 0) return Vertex::pass ();
      to_check_cnt--;
      v = board->empty_v [act_evi];
      act_evi++;
      if (act_evi == board->empty_v_cnt) act_evi = 0;
      if (!board->is_eyelike (act_player, v)) return v;
    }
  }

  void bad_vertex (Vertex) {
  }

  void played_vertex (Vertex) { 
  }

  void end_playout (playout_status_t) { 
  }

};

FastRandom SimplePolicy::fr(123);
