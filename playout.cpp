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


// namespace simple_playout_t


enum playout_status { playout_ok, playout_mercy, playout_too_long };

namespace simple_playout {

  all_inline 
  vertex_t play_one (board_t* board, player_t player) {

    vertex_t v;
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
    return vertex_pass;
  }


  all_inline 
  static playout_status run (board_t* board, player_t first_player) {

    vertex_t   v;
    vertex_t   last_v [player_aux::cnt];
    uint       move_no;
    player_t  act_player;

    player_for_each (pl)
      last_v [pl.idx] = vertex_no_v;

    act_player  = first_player;
    move_no     = 0;

    do {
      v = play_one (board, act_player);

      last_v [act_player.idx] = v;
      act_player = act_player.other ();
      move_no++;

      if ((last_v [player_black.idx] == vertex_pass) & (last_v [player_white.idx] == vertex_pass))    return playout_ok;
      if (move_no >= max_playout_length)                          return playout_too_long;
      if (uint (abs (board->approx_score ())) > mercy_threshold)  return playout_mercy; // TODO introduce constatn
    } while (true);

  }


  board_t    mc_board[1];
  board_t    mc_board_copy[1];
  
  static void benchmark (board_t const * start_board, 
                  uint playout_cnt, 
                  player_t first_player, 
                  ostream& out) 
  {
    float      seconds_begin;
    float      seconds_end;
    float      seconds_total;
    
    playout_status  status;
    uint            win_cnt [player_aux::cnt];
    
    player_for_each (pl) win_cnt [pl.idx] = 0;

    mc_board->load (start_board);
    memcpy (mc_board_copy, mc_board, sizeof (mc_board));

    seconds_begin = get_seconds ();
    
    rep (ii, playout_cnt) {
      memcpy (mc_board, mc_board_copy, sizeof (mc_board));
      status = simple_playout::run (mc_board, first_player);
      
      switch (status) {
      case playout_ok:
        win_cnt [mc_board->winner ().idx] ++;
        break;
      case playout_mercy:
        win_cnt [mc_board->approx_winner ().idx] ++;
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
    
    out << "Black wins = " << win_cnt [player_black.idx] << endl
        << "White wins = " << win_cnt [player_white.idx] << endl
        << "P(black win) = " << float (win_cnt [player_black.idx]) / float (win_cnt [player_black.idx] + win_cnt [player_white.idx]) << endl;
    
  }
  
}
