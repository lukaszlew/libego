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

struct NodeData {

  Player player;
  Vertex v;
  FastMap <Player, bool> has_all_legal_children;

  Stat stat;                    // stat is initalized during construction

  NodeData (Player player_, Vertex v_) : player(player_), v(v_) {
    has_all_legal_children.SetAll (false);
  }

  string ToString() const {
    stringstream s;
    s << player.to_string () << " " 
      << v.to_string () << " " 
      << stat.to_string()
      ;
    return s.str();
  }
};

typedef Node<NodeData> MctsNode;

// -----------------------------------------------------------------------------

class TreeToString {
public:
  string operator () (const MctsNode& node, float min_visit_) { 
    min_visit = min_visit_;
    out.str("");
    out.clear();
    depth = 0;
    RecPrint (&node); 
    return out.str ();
  }

private:

  struct CompareNodeMean { 
    CompareNodeMean(Player player) : player_(player) {}
    bool operator()(const MctsNode* a, const MctsNode* b) {
      if (player_ == Player::black ()) {
        return a->stat.mean() < b->stat.mean();
      } else {
        return a->stat.mean() > b->stat.mean();
      }
    }
    Player player_;
  };

  void RecPrint (const MctsNode* node) {
    rep (d, depth) out << "  ";
    out << node->ToString () << endl;

    vector <const MctsNode*> child_tab;
    FOREACH (const MctsNode& child, node->Children())  child_tab.push_back(&child);
    sort (child_tab.begin(), child_tab.end(), CompareNodeMean (node->player));

    depth += 1;
    FOREACH (const MctsNode* child, child_tab) {
      if (child->stat.update_count() >= min_visit) RecPrint (child);
    }
    depth -= 1;
  }

private:
  ostringstream out;
  uint depth;
  float min_visit;
};

// -----------------------------------------------------------------------------

class Mcts {
public:
  
  Mcts (FullBoard& full_board_)
    : full_board (full_board_),
      act_root (new MctsNode(NodeData(Player::white(), Vertex::any())))
  {
    uct_explore_coeff    = 1.0;
    mature_update_count  = 100.0;
    print_update_count   = 500.0;
    resign_mean          = -0.95;
  }

  ~Mcts () {
    delete act_root; // TODO scoped_ptr
  }

  void DoNPlayouts (uint n) { // TODO first_player
    Reset ();
    rep (ii, n) {
      DoOnePlayout ();
    }
  }

  string ToString () {
    return tree_to_string (*act_root, print_update_count);
  }

  Vertex BestMove (Player pl) {
    // Find best move from the act_root and print tree.
    MctsNode* best_node = most_explored_child (act_root, pl);

    return
      (pl.subjective_score (best_node->stat.mean()) < resign_mean) ? 
      Vertex::resign () :
      best_node->v;
  }

  vector<Move> NewPlayout () {
    // TODO replace it with MCTS playout.
    LightPlayout::MoveHistory history;

    Board playout_board;
    playout_board.load (&full_board.board());
    LightPlayout playout (&playout_board);
    playout.Run (history);
    
    return history.AsVector ();
  }

private:

  void Reset () {
    Player act_player = full_board.board().act_player();
    // prepare act_root of the tree
    delete act_root;
    act_root = new MctsNode(NodeData(act_player.other(), Vertex::any()));

    // add 1 level of tree with superko detection // TODO remove
    empty_v_for_each_and_pass (&full_board.board(), v, {
      if (full_board.is_legal (act_player, v)) {
        act_root->AddChild (NodeData(act_player, v));
      }
    });

    act_root->has_all_legal_children [act_player] = true;
  }

  void DoOnePlayout (){
    // Prepare simulation board and tree iterator.
    play_board.load (&full_board.board());
    trace.clear();
    trace.push_back(act_root);

    // descent the MCTS tree
    while(ActNode()->has_all_legal_children [play_board.act_player()]) {
      if (!DoTreeMove (play_board.act_player ())) return;
    }

    if (play_board.both_player_pass()) {
      update_history (play_board.tt_winner().to_score());
      return;
    }
    
    // Is leaf is ready to expand ?
    if (ActNode()->stat.update_count() > mature_update_count) {
      Player pl = play_board.act_player();
      assertc (mcts_ac, pl == ActNode()->player.other());

      AddAllLegalChildren (pl);

      if (!DoTreeMove (pl)) return; // Descend one more level.
    }

    // Finish with regular playout.
    LightPlayout (&play_board).Run();
    
    // Update score.
    update_history (play_board.playout_winner().to_score());
  }
  
  bool DoTreeMove (Player act_player) {
    // Find UCT child.
    MctsNode* best_child = NULL;
    float best_urgency = -large_float;
    const float explore_coeff
      = log (ActNode()->stat.update_count()) * uct_explore_coeff;

    assertc (mcts_ac, ActNode()->has_all_legal_children [act_player]);

    FOREACH (MctsNode& child, ActNode()->Children()) {
      if (child.player != act_player) continue;
      float child_urgency = child.stat.ucb (act_player, explore_coeff);
      if (child_urgency > best_urgency) {
        best_urgency = child_urgency;
        best_child   = &child;
      }
    }

    assertc (mcts_ac, best_child != NULL); // at least pass
    assertc (mcts_ac, play_board.is_pseudo_legal (act_player, best_child->v));

    // Try to play it on the board
    play_board.play_legal (act_player, best_child->v);
    if (play_board.last_move_status != Board::play_ok) { // large suicide
      assertc (mcts_ac, !best_child->HaveChildren ());
      assertc (mcts_ac,
               best_child->stat.update_count() == Stat::prior_update_count);
      // Remove in case of large suicide.
      ActNode()->RemoveChild (best_child);
      return false;
    }

    // Update tree itreatror.
    trace.push_back (best_child);
    return true;
  }

  void AddAllLegalChildren (Player pl) {
    empty_v_for_each_and_pass (&play_board, v, {
      // big suicides and superko nodes have to be removed from the tree later
      if (play_board.is_pseudo_legal (pl, v))
        ActNode()->AddChild (NodeData(pl, v));
    });
    ActNode()->has_all_legal_children [pl] = true;
  }

  void update_history (float score) {
    // score: black -> 1, white -> -1
    rep (ii, trace.size()) {
      trace[ii]->stat.update (score);
    }
  }

  MctsNode* most_explored_child (MctsNode* node, Player pl) {
    MctsNode* best = NULL;
    float best_update_count = -1;

    assertc (mcts_ac, node->has_all_legal_children [pl]);

    FOREACH (MctsNode& child, node->Children()) {
      if (child.player == pl && child.stat.update_count() > best_update_count) {
        best_update_count = child.stat.update_count();
        best = &child;
      }
    }

    assertc (mcts_ac, best != NULL);
    return best;
  }

  MctsNode* ActNode() {
    assertc (mcts_ac, trace.size() > 0);
    return trace.back ();
  }

private:
  friend class MctsGtp;

  // parameters
  float uct_explore_coeff;
  float mature_update_count;
  float print_update_count;
  float resign_mean;

  // base board
  FullBoard& full_board;
  
  // playout
  Board play_board;

  // tree
  MctsNode* act_root;
  vector <MctsNode*> trace;

  // tree printing
  TreeToString tree_to_string;
};
