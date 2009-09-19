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

template <class Source>
class PlayoutGfx {
public:
  PlayoutGfx (Gtp::ReplWithGogui& gtp, Source& source_, const string& prefix)
    : source (source_)
  {
    const string cmd_name = prefix + "show";
    gtp.RegisterGfx (cmd_name, "new",  this, &PlayoutGfx::CShow);
    gtp.RegisterGfx (cmd_name, "more", this, &PlayoutGfx::CShow);
    gtp.RegisterGfx (cmd_name, "less", this, &PlayoutGfx::CShow);
    show_move_count = 6;
  }

  void CShow (Gtp::Io& io) {
    int n = io.Read<int> (-1);
    if (n > 0) {
      io.CheckEmpty ();
      show_move_count = n;
    } else {
      string sub = io.Read<string> ();
      io.CheckEmpty ();
      if (sub == "new") {
        show_move_count = 6;
        playout = source.NewPlayout ();
      } else if (sub == "more") {
        show_move_count += 1;
      } else if (sub == "less") {
        show_move_count -= 1;
      } else {
        throw Gtp::syntax_error;
      }
    }

    show_move_count = max(show_move_count, 0);
    show_move_count = min(show_move_count, int(playout.size()));

    Gfx gfx;

    rep(ii, show_move_count) {
      gfx.add_var_move(playout[ii]);
    }

    if (show_move_count > 0) {
      gfx.add_symbol(playout[show_move_count-1].get_vertex(), Gfx::circle);
    }

    io.Out () << gfx.to_string ();
  }

private:
  vector<Move> playout;
  int show_move_count;
  Source& source;
};

// -----------------------------------------------------------------------------

class MctsParams {
public:
  
  MctsParams (Gtp::ReplWithGogui& gtp) {
    explore_rate                   = 1.0;
    mature_update_count_threshold  = 100.0;
    min_visit                      = 2500;
    resign_mean                    = -0.95;

    gtp.RegisterParam ("MCTS.params", "explore_rate",
                                 &explore_rate);
    gtp.RegisterParam ("MCTS.params", "#_updates_to_promote",
                                 &mature_update_count_threshold);
    gtp.RegisterParam ("MCTS.params", "print_min_visit",
                                 &min_visit);
  }

private:

  friend class Mcts;

  float explore_rate;
  float mature_update_count_threshold;
  float resign_mean;
  float min_visit;
};

// -----------------------------------------------------------------------------

class Mcts {
public:
  
  Mcts (Gtp::ReplWithGogui& gtp, FullBoard& full_board_)
    : full_board (full_board_),
      policy (global_random),
      params (gtp)
  {
  }

  void Reset () {
    Player act_player = full_board.board().act_player();
    // prepare pool and root of the tree
    node_pool.Reset();
    act_node.SetToRoot(node_pool.Alloc());
    act_node->player = act_player.other();
    act_node->v = Vertex::any();

    // add 1 level of tree with superko detection // TODO remove
    empty_v_for_each_and_pass (&full_board.board(), v, {
      if (full_board.is_legal (act_player, v)) {
        alloc_child (act_player, v);
      }
    });
  }

  void DoNPlayouts (uint n) {
    rep (ii, n) {
      DoOnePlayout ();
    }
  }

  string ToString () {
    act_node.ResetToRoot();
    return tree_to_string (act_node, params.min_visit);
  }

  Vertex BestMove () {
    // Find best move from the root and print tree.
    act_node.ResetToRoot();
    MctsNode* best_node = most_explored_root_node ();

    // Return the best move or resign.
    Player act_player = full_board.board().act_player();

    return
      (act_player.subjective_score (best_node->stat.mean()) < params.resign_mean) ? 
      Vertex::resign () :
      best_node->v;
  }

  vector<Move> NewPlayout () {
    vector<Move> ret;

    Board playout_board;
    playout_board.load (&full_board.board());
    SimplePolicy policy(global_random);
    Playout<SimplePolicy> playout (&policy, &playout_board);
    playout.run();
    
    rep (ii, playout.move_history.Size()) {
      ret.push_back (playout.move_history[ii]);
    }
    return ret;
  }

private:

  void DoOnePlayout (){
    // Prepare simulation board and tree iterator.
    play_board.load (&full_board.board());
    act_node.ResetToRoot ();
    // descent the MCTS tree
    while(act_node->HaveChildren()) {
      do_tree_move();
      if (play_board.last_move_status != Board::play_ok) {
        // large suicide
        delete_act_node ();
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
        if (play_board.is_pseudo_legal (pl, v)) alloc_child (pl, v);
      });

      // Descend one more level.
      do_tree_move();
      assertc (mcts_ac, play_board.last_move_status == Board::play_ok);
    }

    // Finish with regular playout.
    Playout<SimplePolicy> (&policy, &play_board).run ();
    
    // Update score.
    update_history (play_board.playout_winner().to_score());
  }
  
  void do_tree_move () {
    Vertex v = mcts_child_move();
    act_node.Descend (v);
    Player pl = play_board.act_player ();
    assertc (mcts_ac, play_board.is_pseudo_legal (pl, v));
    play_board.play_legal (pl, v);
  }

  Vertex mcts_child_move() {
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
    return best_v;
  }

  void update_history (float score) {
    rep (hi, act_node.Path().size()) {
      // black -> 1, white -> -1
      act_node.Path()[hi]->stat.update (score);
    }
  }

  MctsNode* most_explored_root_node () {
    MctsNode* best = NULL;
    float best_update_count = -1;

    for (MctsNode::ChildrenIterator child(*act_node); child; ++child) {
      if (child->stat.update_count() > best_update_count) {
        best_update_count = child->stat.update_count();
        best = child;
      }
    }

    assertc (tree_ac, best != NULL);
    return best;
  }

  void delete_act_node () {
    assertc (tree_ac, !act_node->HaveChildren ());
    Vertex v = act_node->v;
    act_node.Ascend();
    act_node->DeattachChild (v);
    // TODO free in the pool
  }

  MctsNode* alloc_child (Player pl, Vertex v) {
    MctsNode* new_node = node_pool.Alloc ();
    new_node->player = pl;
    new_node->v = v;
    act_node->AttachChild (v, new_node);
    return new_node;
  }

private:
  // base board
  FullBoard& full_board;
  
  // playout
  Board play_board;
  SimplePolicy policy;

  // tree
  FastPool<MctsNode, 500000> node_pool;
  MctsNode::Iterator act_node;      // TODO sync tree->root with full_board

  // params
  MctsParams params;

  // presentation
  TreeToString tree_to_string;
};

// -----------------------------------------------------------------------------

class Genmove {
public:
  Genmove (Gtp::ReplWithGogui& gtp, FullBoard& full_board_, Mcts& mcts_)
    : mcts(mcts_), full_board(full_board_)
  {
    playout_count = 10000;
    gtp.Register ("genmove", this, &Genmove::CGenmove);
    gtp.RegisterParam ("genmove.params", "playout_count", &playout_count);
  }

private:
  void CGenmove (Gtp::Io& io) {
    Player player = io.Read<Player> ();
    io.CheckEmpty ();

    full_board.set_act_player(player);

    mcts.Reset ();
    mcts.DoNPlayouts (playout_count);
    cerr << mcts.ToString () << endl;

    Vertex v = mcts.BestMove ();

    if (v != Vertex::resign ()) {
      bool ok = full_board.try_play (player, v);
      assert(ok);
      io.Out () << v.to_string();
    } else {
      io.Out () << "resign";
    }
  }
private:
  Mcts& mcts;
  FullBoard& full_board;
  float playout_count;
};
