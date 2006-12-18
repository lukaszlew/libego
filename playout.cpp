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

#ifndef PLAYOUT_TEST
#pragma once
#endif

#include "board.cpp"


// playout_t_

static const bool playout_print = false;

#ifdef NDEBUG
static const bool playout_ac = false;
#endif


#ifdef DEBUG
static const bool playout_ac = true;
#endif

class playout_t { // board with incremental fast random move generator

public:

  board_t*    board;

  v::t        empty_v[board_area];
  uint        empty_v_cnt;

  v::t        rejected_v[board_area];
  uint        rejected_v_cnt;

  void check () {
    if (!playout_ac) return;

    bool noticed[v::cnt];

    assert    (empty_v_cnt <= board_area);
    assert (rejected_v_cnt <= board_area);

    rep (v, v::cnt) noticed[v] = false;

    rep (k, empty_v_cnt) {
      assert (noticed[empty_v[k]] == false);
      noticed[empty_v[k]] = true;
    }

    rep (k, rejected_v_cnt) {
      assert (noticed[rejected_v[k]] == false);
      noticed[rejected_v[k]] = true;
    }

    v_for_each (v) // TODO on_board
      assert ((board->color_at[v] == color::empty) == noticed[v]);
  }

  playout_t (board_t* board) { // copying constructor makes no improvement as randomization is a key
    this->board = board;

    init ();
  }

  void init () {
    empty_v_cnt    = 0;
    rejected_v_cnt = 0;

    v_for_each (v)              // TODO ...on_board
      if (board->color_at[v] == color::empty) 
        add_empty_v (v);
  }

  void add_empty_v (v::t v) {
    uint ii;

    empty_v_cnt++;
    ii = pm::rand () % empty_v_cnt;         // TODO improve speed here
    empty_v[empty_v_cnt-1] = empty_v[ii];
    empty_v[ii] = v;
    assertc (playout_ac, empty_v_cnt <= board_area);
  }

  void run (player::t first_pl) {
    v::t  v;
    bool  was_pass[player::cnt];

    player_for_each (pl) was_pass [pl] = 0;



    #define pp(player) {                    \
      v = play_one (player);                \
      if (playout_print) {                  \
        cout << endl;                       \
        board->print(v);                    \
        getchar();                          \
      }                                     \
      was_pass[player] = (v == v::pass);    \
    } 
    
    if (first_pl == player::white) pp (player::white);

    rep (move_no, board_area) { // to avoid rare loopy playouts
      assertc (playout_ac, board->captured_cnt == 0);

      player_for_each (pl) pp (pl);

      if (was_pass [player::black] & was_pass [player::white]) break;  // TODO condition here
    }
  }

  v::t play_one (player::t player) {
    v::t                   v;

    // TODO  too many if's inside
    while (empty_v_cnt > 0) {   // TODO neutralize this "if"
      v = empty_v [--empty_v_cnt];

      // TODO try two phases 1. any eye is bad, play_no_eye 2. play eyes      
      if (board->is_eyelike (player, v) || 
          board->play (player, v) == board_t::play_ss_suicide) 
      // then
      {
        rejected_v[rejected_v_cnt++] = v;
        if (playout_print) {
          qq ("REJECT\n");
          board->print (v);
          getchar ();
        }
      } else {
        while (board->captured_cnt > 0) // TODO integrate captured into "play"
          add_empty_v (board->captured[--board->captured_cnt]);
        
        if (playout_print) qq ("OK");

        return v;
      }
    }

    memcpy (empty_v, rejected_v, sizeof(v::cnt) * rejected_v_cnt);
    empty_v_cnt = rejected_v_cnt;
    rejected_v_cnt = 0;

    if (playout_print) 
      cout << "MEMCPY " << empty_v_cnt << endl;

    while (empty_v_cnt > 0) {   // TODO neutralize this "if"
      v = empty_v [--empty_v_cnt];

      // TODO try two phases 1. any eye is bad, play_no_eye 2. play eyes      
      if (board->is_eyelike (player, v) || 
          board->play (player, v) == board_t::play_ss_suicide) 
      // then
      {
        rejected_v[rejected_v_cnt++] = v;
        if (playout_print) {
          qq ("REJECT\n");
          board->print (v);
          getchar ();
        }
      } else {
        while (board->captured_cnt > 0) // TODO integrate captured into "play"
          add_empty_v (board->captured[--board->captured_cnt]);

        if (playout_print) qq ("OK 2");

        return v;
      }
    }

    board->check_no_more_legal (player); // powerfull check

    return v::pass;
  }



};


#ifdef PLAYOUT_TEST

board_t mc_board;
board_t arch_board;

int main () { 
  uint win_cnt [player::cnt];
  
  player_for_each (pl) win_cnt [pl] = 0;

  arch_board.clear ();
  arch_board.set_komi (-2);
  
  rep (ii, 200000) {
    mc_board.load (&arch_board);

    playout_t playout (&mc_board);
    playout.run (player::black);

    win_cnt [mc_board.winner ()] ++; // TODO this is a way too costly // (we have empty tab, there is a better way)
  }

  cout << "black wins = " << win_cnt [player::black] << endl
       << "white wins = " << win_cnt [player::white] << endl;

  return 0;
}

#endif


/*

  Mam bardzo rozgaleziony algorytm, dlaczego?
1. limit na dlugosc playoutu
2. 2 pass
3. szukamy ruchu while empty_v_cnt > 0
4. eyelike state
5. play result
6. while captured

Potrzebujemy:
  Dobrego kryterium konca gry.

Idealny algorytm:
 1. good_move_cnt > 0 ?  
    
*/
