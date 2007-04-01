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


#ifdef NDEBUG
const bool playout_ac = false;
#endif


#ifdef DEBUG
const bool playout_ac = false;
#endif


const bool playout_print       = false;
const uint max_playout_length  = board_area * 2;
const uint mercy_threshold     = 25;


// namespace simple_playout_t


enum playout_status { playout_ok, playout_mercy, playout_too_long };

namespace simple_playout {

  all_inline 
  v::t play_one (board_t* board, player::t player) {

    v::t v;
    uint start;

    // find random place in vector of empty vertices
    start = pm::rand_int (board->empty_v_cnt); 

    // search for a move in start ... board->empty_v_cnt-1 interval
    for (uint ev_i = start; ev_i != board->empty_v_cnt; ev_i++) {   
      v = board->empty_v [ev_i];
      if (!board->is_eyelike (player, v) &&
          board->play_no_pass (player, v) < play_ss_suicide) return v;
    }

    // search for a move in 0 ... start interval
    for (uint ev_i = 0; ev_i != start; ev_i++) {
      v = board->empty_v [ev_i];
      if (!board->is_eyelike (player, v) &&
          board->play_no_pass (player, v) < play_ss_suicide) return v;
    }

    board->check_no_more_legal (player); // powerfull check
    return v::pass;
  }


  all_inline 
  static playout_status run (board_t* board, player::t first_player) {

    v::t       v;
    bool       was_pass [player::cnt];
    uint       move_no;
    player::t  act_player;

    player_for_each (pl)
      was_pass [pl] = false;

    act_player  = first_player;
    move_no     = 0;

    do {
      v = play_one (board, act_player);

      was_pass [act_player] = (v == v::pass);
      act_player = player::other (act_player);
      move_no++;

      if (was_pass [player::black] & was_pass [player::white])    return playout_ok;
      if (move_no >= max_playout_length)                          return playout_too_long;
      if (uint (abs (board->approx_score ())) > mercy_threshold)  return playout_mercy;
    } while (true);

  }


  board_t    mc_board[1];
  board_t    mc_board_copy[1];
  
  static void benchmark (board_t const * start_board, 
                  uint playout_cnt, 
                  player::t first_player, 
                  ostream& out) 
  {
    float      seconds_begin;
    float      seconds_end;
    float      seconds_total;
    
    playout_status  status;
    uint            win_cnt [player::cnt];
    
    player_for_each (pl) win_cnt [pl] = 0;

    mc_board->load (start_board);
    memcpy (mc_board_copy, mc_board, sizeof (mc_board));

    seconds_begin = get_seconds ();
    
    rep (ii, playout_cnt) {
      memcpy (mc_board, mc_board_copy, sizeof (mc_board));
      status = simple_playout::run (mc_board, first_player);
      
      switch (status) {
      case playout_ok:
        win_cnt [mc_board->winner ()] ++;
        break;
      case playout_mercy:
        win_cnt [mc_board->approx_winner ()] ++;
        break;
      case playout_too_long:
        break;
      }
    }
    
    seconds_end = get_seconds ();
    
    out << "Initial board:" << endl;
    out << "komi " << start_board->get_komi () << endl;
    
    out << start_board->to_string ();
    
    seconds_total = seconds_end - seconds_begin;
    
    out << "Performance: " << endl
        << "  " << playout_cnt << " playouts" << endl
        << "  " << seconds_total << " seconds" << endl
        << "  " << float (playout_cnt) / seconds_total / 1000.0 << " kpps" << endl;
    
    out << "Black wins = " << win_cnt [player::black] << endl
        << "White wins = " << win_cnt [player::white] << endl
        << "P(black win) = " << float (win_cnt [player::black]) / float (win_cnt [player::black] + win_cnt [player::white]) << endl;
    
  }
  
}
