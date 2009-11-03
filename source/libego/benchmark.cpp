//
// Copyright 2006 and onwards, Lukasz Lew
//

#include "benchmark.hpp"

#include "fast_timer.hpp"
#include "playout.hpp"
#include "utils.hpp"

namespace Benchmark {

  static const Board empty_board;

  void DoPlayouts (uint playout_cnt, NatMap<Player, uint>* win_cnt) {
    static Board playout_board;
    static FastRandom random (123);

    LightPlayout playout (&playout_board, random);

    rep (ii, playout_cnt) {
      playout_board.Load (&empty_board);
      if (playout.Run ()) {
        (*win_cnt) [playout_board.playout_winner ()] ++;
      }
    }

    // ignore this line, this is for a stupid g++ to force aligning(?)
    int xxx = playout_board.empty_v_cnt;
    unused(xxx);
  }

  string Run (uint playout_cnt) {
    NatMap <Player, uint> win_cnt (0);
    FastTimer fast_timer;

    fast_timer.Reset ();
    fast_timer.Start ();
    float seconds_begin = process_user_time ();
    
    DoPlayouts (playout_cnt, &win_cnt);

    float seconds_end = process_user_time ();
    fast_timer.Stop ();


    float seconds_total = seconds_end - seconds_begin;
    float cc_per_playout = fast_timer.Ticks () / double (playout_cnt);
    
    ostringstream ret;
    ret << endl 
        << playout_cnt << " playouts in " << seconds_total << " seconds" << endl
        << float (playout_cnt) / seconds_total / 1000.0 << " kpps" << endl
        << 1000000.0 / cc_per_playout  << " kpps/GHz (clock independent)" << endl
        << win_cnt [Player::Black ()] << "/" << win_cnt [Player::White ()]
        << " (black wins / white wins)" << endl;

    return ret.str();
  }
}
