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
#include "utils.h"

namespace Benchmark {

  static const Board empty_board;

  void do_playouts (uint playout_cnt, FastMap<Player, uint>* win_cnt) {
    static Board playout_board;
    SimplePolicy policy;
    Playout<SimplePolicy> playout(&policy, &playout_board);

    rep (ii, playout_cnt) {
      playout_board.load (&empty_board);
      if (playout.run () == pass_pass) {
        (*win_cnt) [playout_board.winner ()] ++;
      }
    }

    // ignore this line, this is for a stupid g++ to force aligning(?)
    int xxx = playout_board.empty_v_cnt;
    unused(xxx);
  }

  string run (uint playout_cnt) {
    FastMap<Player, uint>  win_cnt;
    FastTimer              fast_timer;

    player_for_each (pl) win_cnt [pl] = 0;

    fast_timer.reset ();
    fast_timer.start ();
    float seconds_begin = process_user_time ();
    
    do_playouts(playout_cnt, &win_cnt);

    float seconds_end = process_user_time ();
    fast_timer.stop ();


    float seconds_total = seconds_end - seconds_begin;
    float cc_per_playout = fast_timer.ticks () / double (playout_cnt);
    
    ostringstream ret;
    ret << endl 
        << playout_cnt << " playouts in " << seconds_total << " seconds" << endl
        << float (playout_cnt) / seconds_total / 1000.0 << " kpps" << endl
        << 1000000.0 / cc_per_playout  << " kpps/GHz (clock independent)" << endl
        << win_cnt [Player::black ()] << "/" << win_cnt [Player::white ()]
        << " (black wins / white wins)" << endl;

    return ret.str();
  }
}
