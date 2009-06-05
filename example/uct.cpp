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

#include "stat.h"
#include "full_board.h"

// -----------------------------------------------------------------------------

class NodeData {
public:
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

// -----------------------------------------------------------------------------

class Node : public NodeData {
public:

  // ------------------------------------------------------------------

  class Iterator {
  public:
    Iterator(Node& parent) : parent_(parent), act_v_(0) { Sync (); }

    Node& operator* ()  { return *parent_.children_[act_v_]; }
    Node* operator-> () { return parent_.children_[act_v_]; }
    operator Node* ()   { return parent_.children_[act_v_]; }

    void operator++ () { act_v_.next(); Sync (); }
    operator bool () { return act_v_.in_range(); } 
  private:
    void Sync () {
      while (act_v_.in_range () && parent_.children_[act_v_] == NULL) {
        act_v_.next();
      }
    }
    Node& parent_;
    Vertex act_v_;
  };

  // ------------------------------------------------------------------

  Iterator children() {
    return Iterator(*this);
  }

  void init (Player pl, Vertex v) {
    this->player = pl;
    this->v = v;
    vertex_for_each_all (v)
      children_[v] = NULL;
    have_child = false;
  }

  void add_child (Vertex v, Node* new_child) { // TODO sorting?
    have_child = true;
    // TODO assert
    children_[v] = new_child;
  }

  void remove_child (Vertex v) { // TODO inefficient
    assertc (tree_ac, children_[v] != NULL);
    children_[v] = NULL;
  }

  bool have_children () {
    return have_child;
  }

  Node* child(Vertex v) {
    return children_[v];
  }

private:
  FastMap<Vertex, Node*> children_;
  bool have_child;
};

// -----------------------------------------------------------------------------

class Tree {
public:

  Tree () : node_pool(uct_max_nodes) {
  }

  void init (Player pl) {
    node_pool.reset();
    path.clear();
    path.push_back(node_pool.malloc());
    path.back()->init (pl.other(), Vertex::any());
  }

  void history_reset () {
    path.resize(1);
  }
  
  Node* act_node () {
    return path.back();
  }
  
  void descend (Vertex v) {
    path.push_back(path.back()->child(v));
    assertc (tree_ac, act_node () != NULL);
  }
  
  void alloc_child (Vertex v) {
    Node* new_node;
    new_node = node_pool.malloc ();
    new_node->init (act_node()->player.other(), v);
    act_node ()->add_child (v, new_node);
  }
  
  void delete_act_node (Vertex v) {
    assertc (tree_ac, !act_node ()->have_children ());
    assertc (tree_ac, path.size() >= 2);
    path.pop_back();
    path.back()->remove_child (v);
  }
  
  void free_subtree (Node* parent) {
    for(Node::Iterator child(*parent); child; ++child) {
      free_subtree(child);
      node_pool.free(child);
    };
  }

  // TODO free history (for sync with base board)
  
  void update_history (float sample) {
    rep (hi, path.size())
       path [hi]->stat.update (sample);
  }

private:

  static const uint uct_max_nodes = 1000000;

  FastPool <Node> node_pool;
  vector<Node*> path;

};


struct CmpNodeMean { 
  CmpNodeMean(Player player) : player_(player) {}
  bool operator()(Node* a, Node* b) {
    if (player_ == Player::black ()) {
      return a->stat.mean() < b->stat.mean();
    } else {
      return a->stat.mean() > b->stat.mean();
    }
  }
  Player player_;
};


void Node_rec_print (Node* node, ostream& out, uint depth, float min_visit) {
  rep (d, depth) out << "  ";
  out << node->to_string () << endl;

  vector <Node*> child_tab;
  for(Node::Iterator child(*node); child; ++child)
    child_tab.push_back(child);

  sort (child_tab.begin(), child_tab.end(), CmpNodeMean(node->player));

  while (child_tab.size() > 0) {
    Node* act_child = child_tab.front();
    child_tab.erase(child_tab.begin());
    if (act_child->stat.update_count() < min_visit) continue;
    Node_rec_print (act_child, out, depth + 1, min_visit);
  }
}


string Node_to_string (Node* node, float min_visit) { 
  ostringstream out_str;
  Node_rec_print (node, out_str, 0, min_visit); 
  return out_str.str ();
}

// -----------------------------------------------------------------------------

class Uct {
public:
  
  Uct (FullBoard& base_board_) : base_board (base_board_), policy(global_random) { 
    explore_rate                   = 1.0;
    uct_genmove_playout_cnt        = 100000;
    mature_update_count_threshold  = 100.0;

    min_visit         = 2500.0;
    min_visit_parent  = 0.02;

    resign_mean = 0.95;
  }

  Vertex genmove () {
    tree.init(base_board.board().act_player());
    root_ensure_children_legality ();

    rep (ii, uct_genmove_playout_cnt)
      do_playout ();

    tree.history_reset();

    Vertex best_v   = most_explored_root_move ();
    float best_mean = tree.act_node()->child(best_v)->stat.mean();
    
    if ((base_board.board().act_player() == Player::black () &&
         best_mean < -resign_mean) ||
        (base_board.board().act_player() == Player::white ()
         && best_mean >  resign_mean)) {
      best_v = Vertex::resign ();
    }

    cerr << Node_to_string (tree.act_node(), min_visit) << endl;

    return best_v;
  }

private:
  // take care about strict legality (superko) in root
  void root_ensure_children_legality () {
    //assertc (uct_ac, tree.history_top == 1);
    assertc (uct_ac, !tree.act_node ()->have_children());

    empty_v_for_each_and_pass (&base_board.board(), v, {
      if (base_board.is_legal (base_board.board().act_player(), v))
        tree.alloc_child (v);
    });
  }

  Vertex uct_child_move() {
    Node* parent = tree.act_node ();
    Vertex best_v = Vertex::any();
    float best_urgency = -large_float;
    float explore_coeff = log (parent->stat.update_count()) * explore_rate;

    for(Node::Iterator ni(*parent); ni; ++ni) {
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

    for(Node::Iterator child(*tree.act_node()); child; ++child) {
      if (child->stat.update_count() > best_update_count) {
        best_update_count = child->stat.update_count();
        best = child->v;
      }
    }

    assertc (tree_ac, best != Vertex::any());
    return best;
  }


  bool do_tree_move () {
    Vertex v = uct_child_move();
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
        tree.alloc_child (v); // TODO simple ko should be handled here
        // (suicides and ko recaptures, needs to be dealt with later)
      });
      return true;
    }
    return false;
  }

  void do_playout (){
    play_board.load (&base_board.board());
    tree.history_reset ();
    
    while(tree.act_node ()->have_children()) {
      if (!do_tree_move()) return;

      if (play_board.both_player_pass()) {
        tree.update_history (play_board.tt_winner().to_score());
        return;
      }
    }
    
    if (try_add_children()) {
      bool ok = do_tree_move();
      assertc(uct_ac, ok);
    }

    Playout<SimplePolicy> (&policy, &play_board).run ();

    int score = play_board.playout_winner().to_score();
    tree.update_history (score); // black -> 1, white -> -1
    return;
  }
  
private:

  float explore_rate;
  uint  uct_genmove_playout_cnt;
  float mature_update_count_threshold;

  float min_visit;
  float min_visit_parent;

  float resign_mean;

  FullBoard&    base_board;
  Tree          tree;      // TODO sync tree->root with base_board
  SimplePolicy  policy;

  Board play_board;
};
