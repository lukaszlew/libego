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

random_pm_t pm(123); // TODO seed it when class


class simple_policy_t {

  uint start_evi;
  uint act_evi;
  uint end_evi;

  board_t* board;
  player_t act_player;

public:

  simple_policy_t (board_t* board_) : board (board_) { }
  
  void prepare_vertex () {
    act_player     = board->act_player ();
    start_evi      = pm.rand_int (board->empty_v_cnt); 
    act_evi        = start_evi;
    end_evi        = board->empty_v_cnt;
  }

  vertex_t next_vertex () {
    vertex_t v;
    while (true) {
      if (act_evi == end_evi) {
        if (end_evi != start_evi) {
          act_evi = 0;
          end_evi = start_evi;
          continue;
        } else {
          return vertex_pass;
        }
      }
      
      v = board->empty_v [act_evi++];
      if (board->is_eyelike (act_player, v)) continue;
      return v;
    }
  }

};

template <typename policy_t> class playout_t {
public:
  move_t   history [max_playout_length];
  board_t* board;

  policy_t& policy;

  playout_t (board_t* board_, policy_t& policy_) 
    : policy (policy_)
  {
    board    = board_; 
  }


  all_inline 
  playout_status run () {

    do {
      vertex_t v;
      play_ret_t status;
      player_t act_player = board->act_player ();

      policy.prepare_vertex ();

      do {
        v = policy.next_vertex ();
        status = board->play (act_player, v);
      } while (status >= play_ss_suicide);

      history [board->move_no] = move_t (act_player, v);

      if (board->both_player_pass ())           
        return playout_ok;

      if (board->move_no >= max_playout_length) 
        return playout_too_long;

      if (use_mercy_rule && uint (abs (board->approx_score ())) > mercy_threshold)  
        return playout_mercy;

    } while (true);

  }

};


namespace simple_playout_benchmark {


  board_t    mc_board[1];
  player_map_t <uint> win_cnt;
  
  void run (board_t const * start_board, 
                   uint playout_cnt, 
                   ostream& out) 
  {
    float      seconds_begin;
    float      seconds_end;
    float      seconds_total;
    
    playout_status  status;
    
    player_for_each (pl) win_cnt [pl] = 0;

    simple_policy_t policy (mc_board);
    playout_t<simple_policy_t> playout (mc_board, policy);

    seconds_begin = get_seconds ();

    rep (ii, playout_cnt) {
      mc_board->load (start_board);
      status = playout.run ();
      
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
    
    out << "Black wins = " << win_cnt [player_black] << endl
        << "White wins = " << win_cnt [player_white] << endl
        << "P(black win) = " << float (win_cnt [player_black]) / float (win_cnt [player_black] + win_cnt [player_white]) << endl;
  }
}
