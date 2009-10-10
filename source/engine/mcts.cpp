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
public:
  MctsBestChildFinder () {
    uct_explore_coeff = 1.0;
    bias_stat = 0.0;
    bias_rave = 0.0001;
    use_rave = true;
  }

  MctsNode& Find (Player pl, MctsNode& node) {
    MctsNode* best_child = NULL;
    float best_urgency = -large_float;
    const float explore_coeff = log (node.stat.update_count()) * uct_explore_coeff;

    assertc (mcts_tree_ac, node.has_all_legal_children [pl]);

    FOREACH (MctsNode& child, node.Children()) {
      if (child.player != pl) continue;
      float child_urgency = NodeValue (child, pl, explore_coeff);
      if (child_urgency > best_urgency) {
        best_urgency = child_urgency;
        best_child   = &child;
      }
    }

    assertc (mcts_tree_ac, best_child != NULL); // at least pass
    return *best_child;
  }

private:

  float NodeValue (MctsNode& node, Player pl, float explore_coeff) {
    return
      (pl == Player::black () ? node.stat.mean() : -node.stat.mean()) +
      sqrt (explore_coeff / node.stat.update_count());
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
public:
  MctsPlayout () {
    mature_update_count  = 100.0;
    update_rave = true;
  }

  void DoOnePlayout (MctsNode& playout_root, const Board& board) {
    // Prepare simulation board and tree iterator.
    play_board.load (&board);
    trace.clear();
    trace.push_back (&playout_root);
    move_history.Clear ();

    // descent the MCTS tree
    while(ActNode().has_all_legal_children [play_board.act_player()]) {
      if (!DoTreeMove ()) return;
    }

    if (play_board.both_player_pass()) {
      UpdateTrace (play_board.tt_winner().to_score());
      return;
    }
    
    // Is leaf is ready to expand ?
    if (ActNode().stat.update_count() > mature_update_count) {
      Player pl = play_board.act_player();
      assertc (mcts_ac, pl == ActNode().player.other());

      ActNode().AddAllPseudoLegalChildren (pl, play_board);

      if (!DoTreeMove ()) return; // Descend one more level.
    }
    // TODO check for pass x 2 here as well

    // Finish with regular playout.
    LightPlayout (&play_board).Run (move_history);
    
    // Update score.
    UpdateTrace (play_board.playout_winner().to_score());
  }

  vector<Move> LastPlayout () {
    return move_history.AsVector ();
  }

private:

  bool DoTreeMove () {
    Player pl = play_board.act_player ();
    MctsNode& uct_child = best_child_finder.Find (pl, ActNode());

    assertc (mcts_ac, play_board.is_pseudo_legal (pl, uct_child.v));

    // Try to play it on the board
    play_board.play_legal (pl, uct_child.v);
    if (play_board.last_move_status != Board::play_ok) { // large suicide
      assertc (mcts_ac, play_board.last_move_status == Board::play_suicide);
      assertc (mcts_ac, !uct_child.has_all_legal_children [pl.other()]);
      assertc (mcts_ac, uct_child.stat.update_count() == Stat::prior_update_count);
      // Remove in case of large suicide.
      ActNode().RemoveChild (&uct_child);
      return false;
    }

    // Update tree itreatror.
    trace.push_back (&uct_child);
    move_history.Push (play_board.last_move ());
    return true;
  }

  void UpdateTrace (float score) {  // score: black -> 1, white -> -1
    UpdateTraceRegular (score);
    if (update_rave) UpdateTraceRave (score);
  }

  void UpdateTraceRegular (float score) {
    FOREACH (MctsNode* node, trace) {
      node->stat.update (score);
    }
  }

  void UpdateTraceRave (float score) {
    // TODO configure rave blocking through options

    FastMap <Move, bool> do_update;
    FastMap <Move, bool> do_update_set_to;

    uint last_ii  = move_history.Size () * 7 / 8;

    rep (act_ii, trace.size()) {
      // Mark moves that should be updated.
      do_update.SetAll (false);
      do_update_set_to.SetAll (true);

      // TODO this is the slow and too-fixed part
      // TODO Change it to weighting with flexible masking.
      reps (jj, act_ii, last_ii) {
        Move m = move_history [jj];
        do_update [m] = do_update_set_to [m];
        do_update_set_to [m] = false;
        do_update_set_to [m.other_player()] = false;
      }

      // Do the update.
      FOREACH (MctsNode& child, trace[act_ii]->Children()) {
        if (do_update [child.GetMove()]) {
          child.rave_stat.update (score);
        }
      }
    }
  }

  MctsNode& ActNode() {
    assertc (mcts_ac, trace.size() > 0);
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
  vector <MctsNode*> trace;               // nodes in the path
  LightPlayout::MoveHistory move_history; // edges in the path
};

// -----------------------------------------------------------------------------

class Mcts {
public:
  
  Mcts (FullBoard& full_board_)
  : full_board (full_board_), root (Player::white(), Vertex::any())
  {
    resign_mean = -0.95;
  }

  void Reset () {
    root.Reset ();
  }

  void DoNPlayouts (uint n) { // TODO first_player
    MctsNode& act_root = FindRoot ();
    rep (ii, n) {
      playout.DoOnePlayout (act_root, full_board.board());
    }
  }

  string ToString (uint min_updates, uint max_children) {
    MctsNode& act_root = FindRoot ();
    return act_root.RecToString (min_updates, max_children);
  }

  Vertex BestMove (Player pl) {
    MctsNode& act_root = FindRoot ();
    const MctsNode& best_node = act_root.MostExploredChild (pl);

    return
      best_node.SubjectiveMean() < resign_mean ?
      Vertex::resign() :
      best_node.v;
  }

  vector<Move> LastPlayout () {
    return playout.LastPlayout ();
  }

private:

  MctsNode& FindRoot () {
    Board sync_board;
    MctsNode* act_root = &root;
    FOREACH (Move m, full_board.MoveHistory ()) {
      Player pl = m.get_player();
      Vertex v  = m.get_vertex();
      if (!act_root->has_all_legal_children[pl]) {
        // TODO make invariant about haveChildren and has_all_legal_children
        act_root->AddAllPseudoLegalChildren (pl, sync_board);
      }
      act_root = act_root->FindChild (pl, v);
      assertc (mcts_ac, act_root != NULL);

      sync_board.play_legal (pl, v);
      assertc (mcts_ac, sync_board.last_move_status == Board::play_ok);
    }
    
    Player pl = full_board.act_player();
    if (!act_root->has_all_legal_children[pl]) {
      act_root->AddAllPseudoLegalChildren (pl, full_board);
    }
    act_root->RemoveIllegalChildren (pl, full_board);

    return *act_root;
  }

private:
  friend class MctsGtp;

  // parameters
  float print_update_count;
  float resign_mean;

  // base board
  FullBoard& full_board;
  
  // tree
  MctsNode root;
  
  // playout
  MctsPlayout playout;
};
