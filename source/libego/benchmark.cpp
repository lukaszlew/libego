//
// Copyright 2006 and onwards, Lukasz Lew
//

#include "benchmark.hpp"

#include "fast_timer.hpp"
#include "playout.hpp"
#include "utils.hpp"

namespace Benchmark {

  static const RawBoard empty_board;
  int move_count = 0;

  void DoPlayouts (uint playout_cnt, NatMap<Player, uint>* win_cnt) {
    static RawBoard playout_board;
    static FastRandom random (123);
    FastStack <Move, RawBoard::kArea * 3> move_history;
    rep (ii, playout_cnt) {
      playout_board.Load (empty_board);
      move_history.Clear();
      if (DoLightPlayout (playout_board, random, move_history)) {
        (*win_cnt) [playout_board.PlayoutWinner ()] ++;
        move_count += move_history.Size();
      }
    }

    // This line does nothing, ignore it.
    // This is for a stupid g++ to force aligning(?)
    // I will buy a bear to somebody who explains 
    // why its removeal drops performance by more than 5%.
    RawBoard::AlignHack (playout_board);
  }

  string Run (uint playout_cnt) {
    NatMap <Player, uint> win_cnt (0);
    FastTimer fast_timer;

    fast_timer.Reset ();
    fast_timer.Start ();
    float seconds_begin = ProcessUserTime ();
    
    DoPlayouts (playout_cnt, &win_cnt);

    float seconds_end = ProcessUserTime ();
    fast_timer.Stop ();


    float seconds_total = seconds_end - seconds_begin;
    float cc_per_playout = fast_timer.Ticks () / double (playout_cnt);
    float playouts_finished = win_cnt [Player::Black ()] + win_cnt [Player::White ()];

    ostringstream ret;
    ret << endl 
        << playout_cnt << " playouts in " << seconds_total << " seconds" << endl
        << float (playout_cnt) / seconds_total / 1000.0 << " kpps" << endl
        << 1000000.0 / cc_per_playout  << " kpps/GHz (clock independent)" << endl
        << win_cnt [Player::Black ()] << "/" << win_cnt [Player::White ()]
        << " (black wins / white wins)" << endl
        << "AVG moves/playout = " << move_count / playouts_finished << endl
        << "too long playouts = " << playout_cnt - playouts_finished << endl;

    return ret.str();
  }
}
