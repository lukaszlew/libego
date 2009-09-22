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

// -----------------------------------------------------------------------------

struct NodeData {
  Stat stat;                    // stat is initalized during construction
  Player player;
  Vertex v;

  NodeData (Player player_, Vertex v_) : player(player_), v(v_) {
  }

  string ToString() {
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
  string operator () (MctsNode* node, float min_visit_) { 
    min_visit = min_visit_;
    out.str("");
    out.clear();
    depth = 0;
    RecPrint (node); 
    return out.str ();
  }

private:

  struct CompareNodeMean { 
    CompareNodeMean(Player player) : player_(player) {}
    bool operator()(MctsNode* a, MctsNode* b) {
      if (player_ == Player::black ()) {
        return a->stat.mean() < b->stat.mean();
      } else {
        return a->stat.mean() > b->stat.mean();
      }
    }
    Player player_;
  };

  void RecPrint (MctsNode* node) {
    rep (d, depth) out << "  ";
    out << node->ToString () << endl;

    vector <MctsNode*> child_tab;
    for(MctsNode::ChildrenIterator child(*node); child; ++child)
      child_tab.push_back(child);

    sort (child_tab.begin(), child_tab.end(), CompareNodeMean(node->player));

    while (child_tab.size() > 0) {
      MctsNode* act_child = child_tab.front();
      child_tab.erase(child_tab.begin());
      if (act_child->stat.update_count() < min_visit) continue;
      depth += 1;
      RecPrint (act_child);
      depth -= 1;
    }
  }

private:
  ostringstream out;
  uint depth;
  float min_visit;
};

// -----------------------------------------------------------------------------

class Mcts {
public:
  
  Mcts (FullBoard& full_board_, MctsParams& params_)
    : full_board (full_board_),
      root (new MctsNode(NodeData(Player::white(), Vertex::any()))),
      params (params_)
  {
  }

  void Reset () {
    Player act_player = full_board.board().act_player();
    // prepare pool and root of the tree
    delete root;
    root = new MctsNode(NodeData(act_player.other(), Vertex::any()));
    act_node.SetToRoot(root);

    // add 1 level of tree with superko detection // TODO remove
    empty_v_for_each_and_pass (&full_board.board(), v, {
      if (full_board.is_legal (act_player, v)) {
        act_node->AddChild (NodeData(act_player, v));
      }
    });
  }

  void DoNPlayouts (uint n) {
    rep (ii, n) {
      DoOnePlayout ();
    }
  }

  string ToString () {
    return tree_to_string (root, params.min_visit);
  }

  Vertex BestMove () {
    // Find best move from the root and print tree.
    MctsNode* best_node = most_explored_root_node ();

    // Return the best move or resign.
    Player act_player = full_board.board().act_player();

    return
      (act_player.subjective_score (best_node->stat.mean()) < params.resign_mean) ? 
      Vertex::resign () :
      best_node->v;
  }

  vector<Move> NewPlayout () {
    LightPlayout::MoveHistory history;

    Board playout_board;
    playout_board.load (&full_board.board());
    LightPlayout playout (&playout_board);
    playout.Run (history);
    
    return history.AsVector ();
  }

private:

  void DoOnePlayout (){
    // Prepare simulation board and tree iterator.
    play_board.load (&full_board.board());
    act_node.ResetToRoot ();
    // descent the MCTS tree
    while(act_node->HaveChildren()) {
      DoTreeMove();
      if (play_board.last_move_status != Board::play_ok) {
        // large suicide
        assertc (tree_ac, !act_node->HaveChildren ());
        Vertex v = act_node->v;
        act_node.Ascend();
        act_node->RemoveChild (v);
        return;
      }
    }
    // exit from MCTS tree

    if (play_board.both_player_pass()) {
      update_history (play_board.tt_winner().to_score());
      return;
    }
    
    // Is leaf is ready to expand ?
    if (act_node->stat.update_count() > params.mature_update_count_threshold) {
      Player pl = play_board.act_player();
      assertc (mcts_ac, pl == act_node->player.other());

      empty_v_for_each_and_pass (&play_board, v, {
        // big suicides and superko nodes have to be removed from the tree later
        if (play_board.is_pseudo_legal (pl, v))
          act_node->AddChild (NodeData(pl, v));
      });

      // Descend one more level.
      DoTreeMove();
      assertc (mcts_ac, play_board.last_move_status == Board::play_ok);
    }

    // Finish with regular playout.
    LightPlayout (&play_board).Run();
    
    // Update score.
    update_history (play_board.playout_winner().to_score());
  }
  
  void DoTreeMove () {
    // Find UCT child.
    Vertex best_v = Vertex::any();
    float best_urgency = -large_float;
    float explore_coeff = log (act_node->stat.update_count()) * params.explore_rate;

    for(MctsNode::ChildrenIterator ni(*act_node); ni; ++ni) {
      float child_urgency = ni->stat.ucb (ni->player, explore_coeff);
      if (child_urgency > best_urgency) {
        best_urgency  = child_urgency;
        best_v = ni->v;
      }
    }

    assertc (tree_ac, best_v != Vertex::any()); // at least pass
    
    // Update tree itreatror and playout board.
    act_node.Descend (best_v);
    Player pl = play_board.act_player ();
    assertc (mcts_ac, play_board.is_pseudo_legal (pl, best_v));
    play_board.play_legal (pl, best_v);
  }

  void update_history (float score) {
    // score: black -> 1, white -> -1
    vector<MctsNode*> path = act_node.Path();
    rep (ii, path.size()) {
      path[ii]->stat.update (score);
    }
  }

  MctsNode* most_explored_root_node () {
    MctsNode* best = NULL;
    float best_update_count = -1;

    for (MctsNode::ChildrenIterator child(*root); child; ++child) {
      if (child->stat.update_count() > best_update_count) {
        best_update_count = child->stat.update_count();
        best = child;
      }
    }

    assertc (tree_ac, best != NULL);
    return best;
  }

private:
  // base board
  FullBoard& full_board;
  
  // playout
  Board play_board;

  // tree
  MctsNode* root;
  MctsNode::Iterator act_node;      // TODO sync tree->root with full_board

  // params
  MctsParams& params;

  // presentation
  TreeToString tree_to_string;
};
