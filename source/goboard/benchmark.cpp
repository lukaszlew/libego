//
// Copyright 2006 and onwards, Lukasz Lew
//

#include "benchmark.hpp"

#include "fast_timer.hpp"
#include "utils.hpp"

namespace Benchmark {

  uint move_count = 0;
  RawBoard empty_board;
  RawBoard board;
  FastRandom random (123);
  //Sampler sampler (board);

  void DoPlayouts (uint playout_cnt, NatMap<Player, uint>* win_cnt) {
    rep (ii, playout_cnt) {
      board.Load (empty_board);
      while (!board.BothPlayerPass ()) {
        Player pl = board.ActPlayer ();
        Vertex v  = board.RandomLightMove (pl, random);
        //Vertex v = sampler.SampleMove ();
        board.PlayLegal (pl, v);
      }

      (*win_cnt) [board.PlayoutWinner ()] ++;
      move_count += board.MoveCount();
    }
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
    float cc_per_move    = fast_timer.Ticks () / double (move_count);
    float playouts_finished = win_cnt [Player::Black ()] + win_cnt [Player::White ()];

    ostringstream ret;
    ret << endl 
        << playout_cnt << " playouts in " << seconds_total << " seconds" << endl
        << float (playout_cnt) / seconds_total / 1000.0 << " kpps" << endl
        << 1000000.0 / cc_per_playout  << " kpps/GHz (clock independent)" << endl
        << cc_per_move  << " CC/move (clock independent)" << endl
        << win_cnt [Player::Black ()] << "/" << win_cnt [Player::White ()]
        << " (black wins / white wins)" << endl
        << "AVG moves/playout = " << move_count / playouts_finished << endl;

    return ret.str();
  }
}
