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

#include <algorithm>
#include <sstream>
#include <vector>

#include "fast_tree.h"
#include "stat.h"
#include "full_board.h"
#include "gtp.h"
#include "gtp_gogui.h"

// -----------------------------------------------------------------------------

struct NodeData {
  Stat stat;                    // stat is initalized during construction
  Player player;
  Vertex v;

  string ToString() {
    stringstream s;
    s << player.to_string () << " " 
      << v.to_string () << " " 
      << stat.to_string();
    return s.str();
  }
};

typedef Node<NodeData> MctsNode;

// -----------------------------------------------------------------------------

struct CmpNodeMean { 
  CmpNodeMean(Player player) : player_(player) {}
  bool operator()(MctsNode* a, MctsNode* b) {
    if (player_ == Player::black ()) {
      return a->stat.mean() < b->stat.mean();
    } else {
      return a->stat.mean() > b->stat.mean();
    }
  }
  Player player_;
};


void Node_rec_print (MctsNode* node, ostream& out, uint depth, float min_visit) {
  rep (d, depth) out << "  ";
  out << node->ToString () << endl;

  vector <MctsNode*> child_tab;
  for(MctsNode::ChildrenIterator child(*node); child; ++child)
    child_tab.push_back(child);

  sort (child_tab.begin(), child_tab.end(), CmpNodeMean(node->player));

  while (child_tab.size() > 0) {
    MctsNode* act_child = child_tab.front();
    child_tab.erase(child_tab.begin());
    if (act_child->stat.update_count() < min_visit) continue;
    Node_rec_print (act_child, out, depth + 1, min_visit);
  }
}


string Node_to_string (MctsNode* node, float min_visit) { 
  ostringstream out_str;
  Node_rec_print (node, out_str, 0, min_visit); 
  return out_str.str ();
}

// -----------------------------------------------------------------------------

class Mcts {
public:
  
  Mcts (Gtp::Gogui::Analyze& gogui_analyze, FullBoard& base_board_)
    : base_board (base_board_), policy(global_random)
  {
    explore_rate                   = 1.0;
    genmove_playout_count          = 100000;
    mature_update_count_threshold  = 100.0;

    min_visit   = 2500.0;
    resign_mean = -0.95;
    show_move_count = 6;

    gogui_analyze.RegisterParam ("MCTS.params", "explore_rate",  &explore_rate);
    gogui_analyze.RegisterParam ("MCTS.params", "playout_count",
                                 &genmove_playout_count);
    gogui_analyze.RegisterParam ("MCTS.params", "#_updates_to_promote",
                                 &mature_update_count_threshold);
    gogui_analyze.RegisterParam ("MCTS.params", "print_min_visit", &min_visit);

    gogui_analyze.RegisterGfxCommand ("MCTS.show", "playout", this, &Mcts::CShow);
    gogui_analyze.RegisterGfxCommand ("MCTS.show", "more",    this, &Mcts::CShow);
    gogui_analyze.RegisterGfxCommand ("MCTS.show", "less",    this, &Mcts::CShow);

    gogui_analyze.GetRepl().RegisterCommand ("genmove", this, &Mcts::CGenmove);
  }

  Vertex genmove (Player player) {
    // init
    base_board.set_act_player(player);
    node_pool.Reset();

    // prepare root
    act_node.SetToRoot(node_pool.Alloc());
    act_node->player = base_board.board().act_player().other();
    act_node->v = Vertex::any();

    root_ensure_children_legality ();

    // find best move
    rep (ii, genmove_playout_count)
      do_playout ();

    MctsNode* best_node = most_explored_root_node ();

    // log
    cerr << Node_to_string (act_node, min_visit) << endl;

    // play and return
    float best_mean = best_node->stat.mean();

    if (base_board.board().act_player().subjective_score(best_mean) < resign_mean) {
      return Vertex::resign ();
    }

    bool ok = base_board.try_play (player, best_node->v);
    assert(ok);
    return best_node->v;
  }

private:
  // take care about strict legality (superko) in root
  void root_ensure_children_legality () {
    //assertc (mcts_ac, tree.history_top == 1);
    assertc (mcts_ac, !act_node->HaveChildren());

    empty_v_for_each_and_pass (&base_board.board(), v, {
      if (base_board.is_legal (base_board.board().act_player(), v)) {
        MctsNode* child = alloc_child (v);
        child->player = act_node->player.other();
        child->v = v;
      }
    });
  }

  Vertex mcts_child_move() {
    Vertex best_v = Vertex::any();
    float best_urgency = -large_float;
    float explore_coeff = log (act_node->stat.update_count()) * explore_rate;

    for(MctsNode::ChildrenIterator ni(*act_node); ni; ++ni) {
      float child_urgency = ni->stat.ucb (ni->player, explore_coeff);
      if (child_urgency > best_urgency) {
        best_urgency  = child_urgency;
        best_v = ni->v;
      }
    }

    assertc (tree_ac, best_v != Vertex::any()); // at least pass
    return best_v;
  }

  MctsNode* most_explored_root_node () {
    act_node.ResetToRoot();
    MctsNode* best = NULL;
    float best_update_count = -1;

    for(MctsNode::ChildrenIterator child(*act_node); child; ++child) {
      if (child->stat.update_count() > best_update_count) {
        best_update_count = child->stat.update_count();
        best = child;
      }
    }

    assertc (tree_ac, best != NULL);
    return best;
  }

  void delete_act_node (Vertex v) {
    assertc (tree_ac, !act_node->HaveChildren ());
    act_node.Ascend();
    act_node->DeattachChild (v);
  }

  MctsNode* alloc_child (Vertex v) {
    MctsNode* new_node = node_pool.Alloc ();
    act_node->AttachChild (v, new_node);
    return new_node;
  }

  bool do_tree_move () {
    Vertex v = mcts_child_move();
    act_node.Descend (v);
      
    if (play_board.is_pseudo_legal (play_board.act_player(), v) == false) {
      delete_act_node (v);
      return false;
    }
      
    play_board.play_legal (play_board.act_player(), v);

    if (play_board.last_move_status != Board::play_ok) {
      delete_act_node (v);
      return false;
    }

    return true;
  }

  bool try_add_children () {
    // If the leaf is ready expand the tree -- add children - 
    // all potential legal v (i.e.empty)
    if (act_node->stat.update_count() >
        mature_update_count_threshold) {
      empty_v_for_each_and_pass (&play_board, v, {
        MctsNode* child = alloc_child (v);
        child->player = act_node->player.other();
        child->v = v;
        // TODO simple ko should be handled here
        // (suicides and ko recaptures, needs to be dealt with later)
      });
      return true;
    }
    return false;
  }
  
  void update_history (float score) {
    rep (hi, act_node.Path().size()) {
      // black -> 1, white -> -1
       act_node.Path()[hi]->stat.update (score);
    }
  }


  void do_playout (){
    play_board.load (&base_board.board());
    act_node.ResetToRoot ();
    
    while(act_node->HaveChildren()) {
      if (!do_tree_move()) return;

      if (play_board.both_player_pass()) {
        update_history (play_board.tt_winner().to_score());
        return;
      }
    }
    
    if (try_add_children()) {
      bool ok = do_tree_move();
      assertc(mcts_ac, ok);
    }

    Playout<SimplePolicy> (&policy, &play_board).run ();

    update_history (play_board.playout_winner().to_score());
  }
  
  void CGenmove (Gtp::Io& io) {
    Player player = io.Read<Player> ();
    io.CheckEmpty ();
    io.Out () << genmove (player).to_string();
  }

  void CShow (Gtp::Io& io) {
    string sub = io.Read<string> ();
    io.CheckEmpty ();

    if (sub == "playout") {
      show_move_count = 6;

      Board playout_board;
      playout_board.load (&base_board.board());
      SimplePolicy policy(global_random);
      Playout<SimplePolicy> playout (&policy, &playout_board);
      playout.run();

      showed_playout.clear();
      rep (ii, playout.move_history.Size())
        showed_playout.push_back(playout.move_history[ii]);

    } else if (sub == "more") {
      show_move_count += 1;
    } else if (sub == "less") {
      show_move_count -= 1;
    } else {
      throw Gtp::syntax_error;
    }

    show_move_count = max(show_move_count, 0);
    show_move_count = min(show_move_count, int(showed_playout.size()));

    Gfx gfx;

    rep(ii, show_move_count) {
      gfx.add_var_move(showed_playout[ii]);
    }

    gfx.add_symbol(showed_playout[show_move_count-1].get_vertex(),
                   Gfx::circle);

    io.Out () << gfx.to_string ();
  }

private:

  vector<Move> showed_playout;
  int show_move_count;

  float explore_rate;
  uint  genmove_playout_count;
  float mature_update_count_threshold;

  float min_visit;

  float resign_mean;

  FullBoard&    base_board;
  SimplePolicy  policy;

  static const uint max_nodes = 1000000;
  FastPool<MctsNode, max_nodes> node_pool;
  MctsNode::Iterator act_node;      // TODO sync tree->root with base_board

  Board play_board;
};
