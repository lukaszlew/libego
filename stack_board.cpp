/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                           *
 *  This file is part of GoBoard library,                                    *
 *                                                                           *
 *  Copyright 2006, 2007 Lukasz Lew                                          *
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


const bool stack_board_ac = true;

class stack_board_t {

public:

  board_t  board_history[max_game_length];
  uint     move_cnt;

public: 
  
  void check () {
    if (!stack_board_ac) return;
    assert (move_cnt < max_game_length);
  }

  stack_board_t () {
    clear ();
  }

  void clear () {
    move_cnt = 0;  
    act_board()->clear ();
  }

  board_t* act_board () { return board_history + move_cnt; }

  bool try_play (player::t player, v::t v) {
    player::check (player);
    v::check (v);

    record_state (); // for undo

    if (v == v::pass) return true;

    if (act_board()->color_at[v] != color::empty) 
      { revert_state (); return false; }

    if (act_board()->play (player, v) != play_ok) 
      { revert_state (); return false; }

    if (is_hash_repeated ()) 
      { revert_state (); return false; }

    return true;
  }

  bool try_undo () {
    if (move_cnt <= 0) return false;
    revert_state ();
    return true;
  }

  void record_state () {
    (act_board () + 1)->load (act_board ()); // for undo-ing
    move_cnt++;
    check ();
  }

  void revert_state () {
    move_cnt--;
    check ();
  }

  bool is_hash_repeated () {
    rep (ii, move_cnt)          // TODO dseq
      if (board_history[ii].hash == act_board ()->hash) 
        return true;

    return false;
  }

};

