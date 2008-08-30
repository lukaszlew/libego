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


enum playout_status_t { pass_pass, mercy, too_long };


template <typename policy_t> class playout_t {
public:
  board_t*  board;
  policy_t  policy[1];

  playout_t (board_t*  board_) : board(board_) {}

  all_inline
  void play_move () {
    policy->prepare_vertex ();
    
    while (true) {
      player_t   act_player = board->act_player ();
      vertex_t   v          = policy->next_vertex ();

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
    
    policy->begin_playout (board);
    while (true) {
      play_move ();
      
      if (board->both_player_pass ()) {
        policy->end_playout (pass_pass);
        return pass_pass;
      }
      
      if (board->move_no >= max_playout_length) {
        policy->end_playout (too_long);
        return too_long;
      }
      
      if (use_mercy_rule && uint (abs (board->approx_score ())) > mercy_threshold) {
        policy->end_playout (mercy);
        return mercy;
      }
    }
  }
  
};


random_pm_t pm(123); // TODO seed it when class

class simple_policy_t {
protected:

  uint start_evi;
  uint act_evi;
  uint end_evi;
  board_t* board;
  player_t act_player;

public:

  simple_policy_t () : board (NULL) { }

  void begin_playout (board_t* board_) { 
    board = board_;
  }

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
          return vertex_t::pass ();
        }
      }
      
      v = board->empty_v [act_evi++];
      if (board->is_eyelike (act_player, v)) continue;
      return v;
    }
  }

  void bad_vertex (vertex_t v) {
  }

  void played_vertex (vertex_t v) { 
  }

  void end_playout (playout_status_t status) { 
  }

};



namespace simple_playout_benchmark {

  board_t                 mc_board [1];

  vertex_t::map_t <int>   vertex_score;
  player_t::map_t <uint>  win_cnt;
  uint                    playout_ok_cnt;
  int                     playout_ok_score;
  cc_clock_t              cc_clock;

  template <bool score_per_vertex> 
  void run (board_t const * start_board, 
            uint playout_cnt, 
            ostream& out) 
  {
    playout_status_t status;
    player_t winner;
    int score;

    playout_ok_cnt   = 0;
    playout_ok_score = 0;

    player_for_each (pl) 
      win_cnt [pl] = 0;

    vertex_for_each_all (v) 
      vertex_score [v] = 0;

    cc_clock.reset ();

    playout_t<simple_policy_t> playout (mc_board);

    cc_clock.start ();
    float seconds_begin = get_seconds ();

    rep (ii, playout_cnt) {
      mc_board->load (start_board);
      status = playout.run ();
      switch (status) {
      case pass_pass:
        playout_ok_cnt += 1;

        score = mc_board -> score ();
        playout_ok_score += score;

        winner = player_t (score <= 0);  // mc_board->winner ()
        win_cnt [winner] ++;

        if (score_per_vertex) {
          vertex_for_each_faster (v)
            vertex_score [v] += mc_board->vertex_score (v);
        }
        break;
      case mercy:
        out << "Mercy rule should be off for benchmarking" << endl;
        return;
        //win_cnt [mc_board->approx_winner ()] ++;
        //break;
      case too_long:
        break;
      }
    }
    
    float seconds_end = get_seconds ();
    cc_clock.stop ();
    
    out << "Initial board:" << endl;
    out << "komi " << start_board->get_komi () << " for white" << endl;
    
    out << start_board->to_string ();
    out << endl;
    
    if (score_per_vertex) {
      vertex_t::map_t <float> black_own;
      vertex_for_each_all (v) 
        black_own [v] = float(vertex_score [v]) / float (playout_ok_cnt);

      out << "P(black owns vertex) rescaled to [-1, 1] (p*2 - 1): " << endl 
          << black_own.to_string_2d () 
          << endl;
    }

    out << "Black wins    = " << win_cnt [player_t::black ()] << endl
        << "White wins    = " << win_cnt [player_t::white ()] << endl
        << "P(black win)  = " 
        << float (win_cnt [player_t::black ()]) / 
           float (win_cnt [player_t::black ()] + win_cnt [player_t::white ()]) 
        << endl;

    float avg_score = float (playout_ok_score) / float (playout_ok_cnt);

    out << "E(score)      = " 
        << avg_score - 0.5 
        << " (without komi = " << avg_score - mc_board->komi << ")" << endl << endl;

    float seconds_total = seconds_end - seconds_begin;
    float cc_per_playout = cc_clock.ticks () / double (playout_cnt);

    out << "Performance: " << endl
        << "  " << playout_cnt << " playouts" << endl
        << "  " << seconds_total << " seconds" << endl
        << "  " << float (playout_cnt) / seconds_total / 1000.0 << " kpps" << endl
        << "  " << cc_per_playout << " ccpp (clock cycles per playout)" << endl
        << "  " << 1000000.0 / cc_per_playout  << " kpps/GHz (clock independent)" << endl
      ;
  }
}
