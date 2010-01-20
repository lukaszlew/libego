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

const float kSureWinUpdate = 1.0; // TODO increase this

// -----------------------------------------------------------------------------

namespace Param {
  bool use_mcts_in_playout = false; // TODO true
}

class MctsPlayout {
  static const bool kCheckAsserts = false;
public:
  MctsPlayout (FastRandom& random_) : random (random_), v_seen (0) {
  }

  void DoOnePlayout (MctsNode& playout_root, const Board& board, Player first_player) {
    // Prepare simulation board and tree iterator.
    play_board.Load (board);
    play_board.SetActPlayer (first_player);
    trace.clear();
    trace.push_back (&playout_root);
    move_history.clear ();
    move_history.push_back (playout_root.GetMove());
    v_seen.SetToZero();
    all_mcmc.NewPlayout ();

    bool tree_phase = Param::use_mcts_in_playout;

    // do the playout
    while (true) {
      if (play_board.BothPlayerPass()) break;
      if (move_history.size() >= 3*Board::kArea) return;
      Player pl = play_board.ActPlayer ();
      Vertex v;

      if (tree_phase) {
        v = ChooseTreeMove (pl);
        if (!v.IsValid()) {
          tree_phase = false;
          continue;
        }
      } else {
        v = all_mcmc.Choose8Move (play_board, v_seen, random);
      }

      ASSERT (play_board.IsLegal (pl, v));
      play_board.PlayLegal (pl, v);
      move_history.push_back (Move(pl, v));
      v_seen [v] += 1;
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
    return move_history;
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
    if (Param::mcmc_update) all_mcmc.Update (play_board);
  }

  void UpdateTraceRegular (float score) {
    BOOST_FOREACH (MctsNode* node, trace) {
      node->stat.update (score);
    }
  }

  void UpdateTraceRave (float score) {
    // TODO configure rave blocking through options


    uint last_ii  = move_history.size () * 7 / 8; // TODO 

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
  vector <Move> move_history;
  NatMap <Vertex, uint> v_seen;
public:
  AllMcmc all_mcmc;
};
