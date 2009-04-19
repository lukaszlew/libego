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

#include "benchmark.h"

#include "fast_timer.h"
#include "playout.h"

namespace Benchmark {

  Board                  empty_board [1];
  Board                  mc_board [1];

  FastMap<Vertex, int>   vertex_score;
  FastMap<Player, uint>  win_cnt;
  uint                   playout_ok_cnt;
  int                    playout_ok_score;
  FastTimer              fast_timer;

  void do_playouts (uint playout_cnt) {
    playout_status_t status;
    Player winner;
    int score;

    SimplePolicy policy;
    Playout<SimplePolicy> playout(&policy, mc_board);

    rep (ii, playout_cnt) {
      mc_board->load (empty_board);
      status = playout.run ();
      switch (status) {
      case pass_pass:
        playout_ok_cnt += 1;

        score = mc_board -> score ();
        playout_ok_score += score;

        winner = Player (score <= 0);  // mc_board->winner ()
        win_cnt [winner] ++;

        break;
      case mercy:
        assert(false); // Mercy rule should be off for benchmarking
        return;
        //win_cnt [mc_board->approx_winner ()] ++;
        //break;
      case too_long:
        break;
      }
    }
    // ignore this line
    score += mc_board->empty_v_cnt; // for a stupid g++ to force
  }

  void run (uint playout_cnt, ostream& out) {
    playout_ok_cnt   = 0;
    playout_ok_score = 0;
    player_for_each (pl) win_cnt [pl] = 0;
    vertex_for_each_all (v) vertex_score [v] = 0;

    fast_timer.reset ();
    fast_timer.start ();
    float seconds_begin = get_seconds ();
    
    do_playouts(playout_cnt);

    float seconds_end = get_seconds ();
    fast_timer.stop ();


    float seconds_total = seconds_end - seconds_begin;
    float cc_per_playout = fast_timer.ticks () / double (playout_cnt);

    out << endl 
        << playout_cnt << " playouts in " << seconds_total << " seconds" << endl
        << float (playout_cnt) / seconds_total / 1000.0 << " kpps" << endl
        << 1000000.0 / cc_per_playout  << " kpps/GHz (clock independent)" << endl
        << win_cnt [Player::black ()] << "/" << win_cnt [Player::white ()]
        << " (black wins / white wins)" << endl;
  }
}
