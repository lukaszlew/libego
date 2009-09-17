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

class NodeData {
public:
  void init_data (Player pl, Vertex v) {
    this->player = pl;
    this->v = v;
    this->stat.reset();
  }

  string to_string() {
    stringstream s;
    s << player.to_string () << " " 
      << v.to_string () << " " 
      << stat.to_string();
    return s.str();
  }

public:
  Stat   stat;
  Player player;
  Vertex v;
};

typedef TreeT<NodeData> Tree;

struct CmpNodeMean { 
  CmpNodeMean(Player player) : player_(player) {}
  bool operator()(Tree::Node* a, Tree::Node* b) {
    if (player_ == Player::black ()) {
      return a->stat.mean() < b->stat.mean();
    } else {
      return a->stat.mean() > b->stat.mean();
    }
  }
  Player player_;
};


void Node_rec_print (Tree::Node* node, ostream& out, uint depth, float min_visit) {
  rep (d, depth) out << "  ";
  out << node->to_string () << endl;

  vector <Tree::Node*> child_tab;
  for(Tree::Node::Iterator child(*node); child; ++child)
    child_tab.push_back(child);

  sort (child_tab.begin(), child_tab.end(), CmpNodeMean(node->player));

  while (child_tab.size() > 0) {
    Tree::Node* act_child = child_tab.front();
    child_tab.erase(child_tab.begin());
    if (act_child->stat.update_count() < min_visit) continue;
    Node_rec_print (act_child, out, depth + 1, min_visit);
  }
}


string Node_to_string (Tree::Node* node, float min_visit) { 
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
    tree.init();
    tree.act_node()->init_data (base_board.board().act_player().other(),
                                Vertex::any());
    root_ensure_children_legality ();

    // find best move
    rep (ii, genmove_playout_count)
      do_playout ();

    tree.history_reset();
    Vertex best_v = most_explored_root_move ();

    // log
    cerr << Node_to_string (tree.act_node(), min_visit) << endl;

    // play and return
    float best_mean = tree.act_node()->child(best_v)->stat.mean();

    if (base_board.board().act_player().subjective_score(best_mean) < resign_mean) {
      return Vertex::resign ();
    }

    bool ok = base_board.try_play (player, best_v);
    assert(ok);
    return best_v;
  }

private:
  // take care about strict legality (superko) in root
  void root_ensure_children_legality () {
    //assertc (mcts_ac, tree.history_top == 1);
    assertc (mcts_ac, !tree.act_node ()->have_children());

    empty_v_for_each_and_pass (&base_board.board(), v, {
      if (base_board.is_legal (base_board.board().act_player(), v)) {
        tree.alloc_child (v)->init_data (tree.act_node()->player.other(), v);
      }
    });
  }

  Vertex mcts_child_move() {
    Tree::Node* parent = tree.act_node ();
    Vertex best_v = Vertex::any();
    float best_urgency = -large_float;
    float explore_coeff = log (parent->stat.update_count()) * explore_rate;

    for(Tree::Node::Iterator ni(*parent); ni; ++ni) {
      float child_urgency = ni->stat.ucb (ni->player, explore_coeff);
      if (child_urgency > best_urgency) {
        best_urgency  = child_urgency;
        best_v = ni->v;
      }
    }

    assertc (tree_ac, best_v != Vertex::any()); // at least pass
    return best_v;
  }

  Vertex most_explored_root_move () {
    tree.history_reset();
    Vertex best = Vertex::any();
    float best_update_count = -1;

    for(Tree::Node::Iterator child(*tree.act_node()); child; ++child) {
      if (child->stat.update_count() > best_update_count) {
        best_update_count = child->stat.update_count();
        best = child->v;
      }
    }

    assertc (tree_ac, best != Vertex::any());
    return best;
  }


  bool do_tree_move () {
    Vertex v = mcts_child_move();
    tree.descend (v);
      
    if (play_board.is_pseudo_legal (play_board.act_player(), v) == false) {
      tree.delete_act_node (v);
      return false;
    }
      
    play_board.play_legal (play_board.act_player(), v);

    if (play_board.last_move_status != Board::play_ok) {
      tree.delete_act_node (v);
      return false;
    }

    return true;
  }

  bool try_add_children () {
    // If the leaf is ready expand the tree -- add children - 
    // all potential legal v (i.e.empty)
    if (tree.act_node()->stat.update_count() >
        mature_update_count_threshold) {
      empty_v_for_each_and_pass (&play_board, v, {
        tree.alloc_child (v)->init_data (tree.act_node()->player.other(), v);
        // TODO simple ko should be handled here
        // (suicides and ko recaptures, needs to be dealt with later)
      });
      return true;
    }
    return false;
  }
  
  void update_history (float score) {
    rep (hi, tree.history().size()) {
      // black -> 1, white -> -1
       tree.history()[hi]->stat.update (score);
    }
  }


  void do_playout (){
    play_board.load (&base_board.board());
    tree.history_reset ();
    
    while(tree.act_node ()->have_children()) {
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
      rep (ii, playout.move_history.size)
        showed_playout.push_back(playout.move_history.tab[ii]);

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
  Tree          tree;      // TODO sync tree->root with base_board
  SimplePolicy  policy;

  Board play_board;
};
