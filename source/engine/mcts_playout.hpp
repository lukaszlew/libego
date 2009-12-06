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

// -----------------------------------------------------------------------------

class MctsBestChildFinder {
  static const bool kCheckAsserts = false;
public:
  MctsBestChildFinder () {
    uct_explore_coeff = 1.0;
    bias_stat = 0.0000001;
    bias_rave = 0.001;
    use_rave = true;
  }

  MctsNode& Find (Player pl, MctsNode& node) {
    MctsNode* best_child = NULL;
    float best_urgency = -100000000000000.0; // TODO infinity
    const float log_val = log (node.stat.update_count());

    ASSERT (node.has_all_legal_children [pl]);

    BOOST_FOREACH (MctsNode& child, node.Children()) {
      if (child.player != pl) continue;
      float child_urgency = NodeSubjectiveValue (child, pl, log_val);
      if (child_urgency > best_urgency) {
        best_urgency = child_urgency;
        best_child   = &child;
      }
    }

    ASSERT (best_child != NULL); // at least pass
    return *best_child;
  }

private:

  float NodeSubjectiveValue (MctsNode& node, Player pl, float log_val) {
    float value;

    if (use_rave) {
      value = Stat::Mix (node.stat, bias_stat, node.rave_stat, bias_rave);
    } else {
      value = node.stat.mean ();
    }

    return
      pl.SubjectiveScore (value) +
      uct_explore_coeff * sqrt (log_val / node.stat.update_count());
  }

private:
  friend class MctsGtp;

  float uct_explore_coeff;
  float bias_stat;
  float bias_rave;
  bool  use_rave;
};

// -----------------------------------------------------------------------------

class MctsPlayout {
  static const bool kCheckAsserts = false;
public:
  MctsPlayout (FastRandom& random_) : random (random_) {
    mature_update_count  = 100.0;
    update_rave = true;
  }

  void DoOnePlayout (MctsNode& playout_root, const Board& board, Player first_player) {
    // Prepare simulation board and tree iterator.
    play_board.Load (board);
    play_board.SetActPlayer (first_player);
    trace.clear();
    trace.push_back (&playout_root);
    move_history.Clear ();

    // descent the MCTS tree
    while(ActNode().has_all_legal_children [play_board.ActPlayer()]) {
      if (!DoTreeMove ()) return;
    }

    if (play_board.BothPlayerPass()) {
      // sure fast win/loss
      UpdateTrace (play_board.TrompTaylorWinner().ToScore() * 1000);
      return;
    }
    
    // Is leaf is ready to expand ?
    if (ActNode().stat.update_count() > mature_update_count) {
      Player pl = play_board.ActPlayer();
      ASSERT (pl == ActNode().player.Other());

      ActNode().EnsureAllPseudoLegalChildren (pl, play_board);

      if (!DoTreeMove ()) return; // Descend one more level.
    }
    // TODO check for pass x 2 here as well

    // Finish with regular playout.
    LightPlayout (&play_board, random).Run (move_history);
    
    // Update score.
    int score = play_board.PlayoutScore();
    float squashed = Player::WinnerOfBoardScore (score).ToScore (); // +- 1
    squashed += float(score) / 10000.0; // small bonus for bigger win.

    UpdateTrace (squashed);
  }

  vector<Move> LastPlayout () {
    return move_history.AsVector ();
  }

private:

  bool DoTreeMove () {
    Player pl = play_board.ActPlayer ();
    MctsNode& uct_child = best_child_finder.Find (pl, ActNode());

    ASSERT (play_board.IsPseudoLegal (pl, uct_child.v));

    // Try to play it on the board
    if (!play_board.PlayPseudoLegal (pl, uct_child.v)) { // large suicide
      ASSERT (!uct_child.has_all_legal_children [pl.Other()]);
      ASSERT (uct_child.stat.update_count() == Stat::prior_update_count);
      // Remove in case of large suicide.
      ActNode().RemoveChild (&uct_child);
      return false;
    }

    // Update tree itreatror.
    trace.push_back (&uct_child);
    move_history.Push (play_board.LastMove ());
    return true;
  }

  void UpdateTrace (int score) {
    UpdateTraceRegular (score);
    if (update_rave) UpdateTraceRave (score);
  }

  void UpdateTraceRegular (float score) {
    BOOST_FOREACH (MctsNode* node, trace) {
      node->stat.update (score);
    }
  }

  void UpdateTraceRave (float score) {
    // TODO configure rave blocking through options


    uint last_ii  = move_history.Size () * 7 / 8;

    rep (act_ii, trace.size()) {
      // Mark moves that should be updated.
      NatMap <Move, bool> do_update (false);
      NatMap <Move, bool> do_update_set_to (true);

      // TODO this is the slow and too-fixed part
      // TODO Change it to weighting with flexible masking.
      reps (jj, act_ii, last_ii) {
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
  // parameters
  float mature_update_count;

  bool  update_rave;
  
  // playout
  Board play_board;
  MctsBestChildFinder best_child_finder;
  FastRandom& random;
  vector <MctsNode*> trace;               // nodes in the path
  LightPlayout::MoveHistory move_history; // edges in the path
};
