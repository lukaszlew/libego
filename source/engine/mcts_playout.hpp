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

const bool mcts_ac = true;
const float kSureWinUpdate = 1.0; // TODO increase this

// -----------------------------------------------------------------------------

namespace Param {
  static bool  update_mcmc = true;
}

class Mcmc {
public:
  Mcmc () : move_stats (Stat()), rave_move_stats (Stat()) {
    Reset ();
  }

  void Reset () {
    ForEachNat (Move, m) {
      // TODO prior
      // stat.reset      (Param::prior_update_count,
      //                  player.SubjectiveScore (Param::prior_mean));
      move_stats [m].reset (1.0, 0.0);
      rave_move_stats [m].reset (1.0, 0.0);
    }
  }

  void Update (const Move* tab, int n, float score) {
    rep (ii, n) {
      ASSERT (tab[ii].IsValid());
      rave_move_stats [tab[ii]].update (score);
    }
    if (n >= 3) move_stats [tab[2]].update (score);
    if (n >= 2) move_stats [tab[1]].update (score); // TODO test it
  }

  NatMap <Move, Stat> move_stats;
  NatMap <Move, Stat> rave_move_stats;

  static const bool kCheckAsserts = false;
};

class AllMcmc {
public:
  AllMcmc () : mcmc (Mcmc()) {
  }

  void Reset () {
    ForEachNat (Move, m) mcmc [m].Reset ();
  }

  template <uint stack_size> 

  void Update (float score, const FastStack<Move, stack_size>& history) {
    uint last_ii  = history.Size () * 7 / 8; // TODO 
    reps (ii, 1, last_ii) {
      Move m1 = history[ii];
      ASSERT (m1.IsValid());
      mcmc [m1] . Update (history.Data() + ii, (last_ii - ii) / 6, score);
    }
  }


  void FillGfx (Move pre_move, Player player, const Board& board, Gtp::GoguiGfx* gfx) {
    Stat stat(0.0, 0.0);

    ForEachNat (Vertex, v) {
      if (board.ColorAt (v) == Color::Empty () && v != pre_move.GetVertex()) {
        Move move = Move (player, v);
        float mean = mcmc [pre_move].move_stats [move].mean ();
        stat.update (mean);
      }
    }

    ForEachNat (Vertex, v) {
      if (board.ColorAt (v) == Color::Empty () &&
          v != pre_move.GetVertex()) {
        Move move = Move (player, v);
        float mean = mcmc [pre_move].move_stats [move].mean ();
        gfx->SetInfluence
          (v.ToGtpString (), (mean - stat.mean()) / stat.std_dev () / 4);
      }
    }
  }

  NatMap <Move, Mcmc> mcmc;
};
// -----------------------------------------------------------------------------

class MctsPlayout {
  static const bool kCheckAsserts = false;
public:
  MctsPlayout (FastRandom& random_) : random (random_) {
  }

  void DoOnePlayout (MctsNode& playout_root, const Board& board, Player first_player) {
    // Prepare simulation board and tree iterator.
    play_board.Load (board);
    play_board.SetActPlayer (first_player);
    trace.clear();
    trace.push_back (&playout_root);
    move_history.Clear ();
    move_history.Push (playout_root.GetMove());

    bool tree_phase = true;

    // do the playout
    while (true) {
      if (play_board.BothPlayerPass()) break;
      if (move_history.IsFull ()) return;

      Player pl = play_board.ActPlayer ();
      Vertex v;

      if (tree_phase) {
        v = ChooseTreeMove (pl);
        if (!v.IsValid()) {
          tree_phase = false;
          continue;
        }
      } else {
        v = play_board.RandomLightMove (pl, random);
      }

      ASSERT (play_board.IsLegal (pl, v));
      play_board.PlayLegal (pl, v);
      move_history.Push (play_board.LastMove ());
    }

    float score;
    if (tree_phase) {
      score = play_board.TrompTaylorWinner().ToScore() * kSureWinUpdate;
    } else {
      int sc = play_board.PlayoutScore();
      score = Player::WinnerOfBoardScore (sc).ToScore (); // +- 1
      score += float(sc) / 10000.0; // small bonus for bigger win.
    }
    UpdateTrace (score);
  }

  vector<Move> LastPlayout () {
    return move_history.AsVector ();
  }

private:

  Vertex ChooseTreeMove (Player pl) {
    if (!ActNode().has_all_legal_children [pl]) {
      if (!ActNode().ReadyToExpand ()) {
        return Vertex::Invalid ();
      }
      ASSERT (pl == ActNode().player.Other());
      ActNode().EnsureAllLegalChildren (pl, play_board);
    }

    MctsNode& uct_child = ActNode().BestRaveChild (pl);
    trace.push_back (&uct_child);
    return uct_child.v;
  }

  void UpdateTrace (int score) {
    UpdateTraceRegular (score);
    if (Param::update_rave) UpdateTraceRave (score);
    if (Param::update_mcmc) all_mcmc.Update (score, move_history);
  }

  void UpdateTraceRegular (float score) {
    BOOST_FOREACH (MctsNode* node, trace) {
      node->stat.update (score);
    }
  }

  void UpdateTraceRave (float score) {
    // TODO configure rave blocking through options


    uint last_ii  = move_history.Size () * 7 / 8; // TODO 

    rep (act_ii, trace.size()) {
      // Mark moves that should be updated in RAVE children of: trace [act_ii]
      NatMap <Move, bool> do_update (false);
      NatMap <Move, bool> do_update_set_to (true);

      // TODO this is the slow and too-fixed part
      // TODO Change it to weighting with flexible masking.
      reps (jj, act_ii+1, last_ii) {
        Move m = move_history [jj];
        do_update [m] = do_update_set_to [m];
        do_update_set_to [m] = false;
        do_update_set_to [m.OtherPlayer()] = false;
      }

      // Do the update.
      BOOST_FOREACH (MctsNode& child, trace[act_ii]->Children()) {
        if (do_update [child.GetMove()]) {
          child.rave_stat.update (score);
        }
      }
    }
  }

  MctsNode& ActNode() {
    ASSERT (trace.size() > 0);
    return *trace.back ();
  }

private:
  friend class MctsGtp;
  
  // playout
  Board play_board;
  FastRandom& random;
  vector <MctsNode*> trace;               // nodes in the path
  FastStack <Move, Board::kArea * 3> move_history;
public:
  AllMcmc all_mcmc;
};
