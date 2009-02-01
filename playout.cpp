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


// ----------------------------------------------------------------------
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

// ----------------------------------------------------------------------
namespace simple_playout_benchmark {

  Board                  mc_board [1];

  FastMap<Vertex, int>   vertex_score;
  FastMap<Player, uint>  win_cnt;
  uint                   playout_ok_cnt;
  int                    playout_ok_score;
  FastTimer              fast_timer;

  template <bool score_per_vertex> 
  void run (Board const * start_board, 
            uint playout_cnt, 
            ostream& out) 
  {
    playout_status_t status;
    Player winner;
    int score;

    playout_ok_cnt   = 0;
    playout_ok_score = 0;

    player_for_each (pl) 
      win_cnt [pl] = 0;

    vertex_for_each_all (v) 
      vertex_score [v] = 0;

    fast_timer.reset ();

    SimplePolicy policy;
    Playout<SimplePolicy> playout(&policy, mc_board);

    fast_timer.start ();
    float seconds_begin = get_seconds ();

    rep (ii, playout_cnt) {
      mc_board->load (start_board);
      status = playout.run ();
      switch (status) {
      case pass_pass:
        playout_ok_cnt += 1;

        score = mc_board -> score ();
        playout_ok_score += score;

        winner = Player (score <= 0);  // mc_board->winner ()
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
    fast_timer.stop ();
    
    out << "Initial board:" << endl;
    out << "komi " << start_board->get_komi () << " for white" << endl;
    
    out << start_board->to_string ();
    out << endl;
    
    if (score_per_vertex) {
      FastMap<Vertex, float> black_own;
      vertex_for_each_all (v) 
        black_own [v] = float(vertex_score [v]) / float (playout_ok_cnt);

      out << "P(black owns vertex) rescaled to [-1, 1] (p*2 - 1): " << endl 
          << to_string_2d (black_own) 
          << endl;
    }

    out << "Black wins    = " << win_cnt [Player::black ()] << endl
        << "White wins    = " << win_cnt [Player::white ()] << endl
        << "P(black win)  = " 
        << float (win_cnt [Player::black ()]) / 
           float (win_cnt [Player::black ()] + win_cnt [Player::white ()]) 
        << endl;

    float avg_score = float (playout_ok_score) / float (playout_ok_cnt);

    out << "E(score)      = " 
        << avg_score - 0.5 
        << " (without komi = " << avg_score - mc_board->komi << ")" << endl << endl;

    float seconds_total = seconds_end - seconds_begin;
    float cc_per_playout = fast_timer.ticks () / double (playout_cnt);

    out << "Performance: " << endl
        << "  " << playout_cnt << " playouts" << endl
        << "  " << seconds_total << " seconds" << endl
        << "  " << float (playout_cnt) / seconds_total / 1000.0 << " kpps" << endl
        << "  " << cc_per_playout << " ccpp (clock cycles per playout)" << endl
        << "  " << 1000000.0 / cc_per_playout  << " kpps/GHz (clock independent)" << endl
      ;
  }
}
