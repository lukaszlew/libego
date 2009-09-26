#ifndef _MCTS_TREE_
#define _MCTS_TREE_

#include <list>

const bool mcts_tree_ac = true;

// -----------------------------------------------------------------------------

class MctsNode {
public:
  typedef std::list<MctsNode> ChildrenList; // TODO vector, allocator?

  explicit MctsNode (Player player_, Vertex v_);

  // Tree topology methods.
  // Printing.

  string ToString() const;

  string RecToString (float min_visit) const; 

  // Children operations.

  void AddChild (const MctsNode& node);

  void RemoveChild (MctsNode* child_ptr);

  void AddAllPseudoLegalChildren (Player pl, const Board& board);

  void RemoveIllegalChildren (Player pl, const FullBoard& full_board);

  // Child finding.

  MctsNode* FindChild (Player pl, Vertex v);

  const MctsNode& MostExploredChild (Player pl);

  MctsNode& FindUctChild (Player pl, float uct_explore_coeff);

public:
  Player player;
  Vertex v;
  FastMap <Player, bool> has_all_legal_children;

  Stat stat;

private:
  void RecPrint (ostream& out, uint depth, float min_visit) const;

  ChildrenList children;
};

// -----------------------------------------------------------------------------

MctsNode::MctsNode (Player player_, Vertex v_) : player(player_), v(v_) {
  has_all_legal_children.SetAll (false);
}

void MctsNode::AddChild (const MctsNode& node) {
  children.push_front (node);
}

// TODO better implementation of child removation.
void MctsNode::RemoveChild (MctsNode* child_ptr) {
  ChildrenList::iterator child = children.begin();
  while (true) {
    assertc (mcts_tree_ac, child != children.end());
    if (&*child == child_ptr) {
      children.erase(child);
      return;
    }
    child++;
  }
}

MctsNode* MctsNode::FindChild (Player pl, Vertex v) {
  FOREACH (MctsNode& child, children) {
    if (child.player == pl && child.v == v) {
      return &child;
    }
  }
  return NULL;
}

string MctsNode::ToString() const {
  stringstream s;
  s << player.to_string () << " " 
    << v.to_string () << " " 
    << stat.to_string()
    ;
  return s.str();
}

namespace {
// TODO replace this by subjective stat.mean
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
}

void MctsNode::RecPrint (ostream& out, uint depth, float min_visit) const {
  rep (d, depth) out << "  ";
  out << ToString () << endl;

  vector <const MctsNode*> child_tab;
  FOREACH (const MctsNode& child, children) {
    child_tab.push_back(&child);
  }

  sort (child_tab.begin(), child_tab.end(), CompareNodeMean (player));

  FOREACH (const MctsNode* child, child_tab) {
    if (child->stat.update_count() >= min_visit) {
      child->RecPrint (out, depth + 1, min_visit);
    }
  }
}

string MctsNode::RecToString (float min_visit) const { 
  ostringstream out;
  RecPrint (out, 0, min_visit); 
  return out.str ();
}

const MctsNode& MctsNode::MostExploredChild (Player pl) {
  const MctsNode* best = NULL;
  float best_update_count = -1;

  assertc (mcts_tree_ac, has_all_legal_children [pl]);

  FOREACH (const MctsNode& child, children) {
    if (child.player == pl && child.stat.update_count() > best_update_count) {
      best_update_count = child.stat.update_count();
      best = &child;
    }
  }

  assertc (mcts_tree_ac, best != NULL);
  return *best;
}

void MctsNode::AddAllPseudoLegalChildren (Player pl, const Board& board) {
  assertc (mcts_tree_ac, has_all_legal_children [pl] == false);
  empty_v_for_each_and_pass (&board, v, {
      // big suicides and superko nodes have to be removed from the tree later
      if (board.is_pseudo_legal (pl, v))
        AddChild (MctsNode(pl, v));
    });
  has_all_legal_children [pl] = true;
}

void MctsNode::RemoveIllegalChildren (Player pl, const FullBoard& full_board) {
  assertc (mcts_tree_ac, has_all_legal_children [pl]);

  ChildrenList::iterator child = children.begin();
  while (child != children.end()) {
    if (child->player == pl && !full_board.is_legal (pl, child->v)) {
      children.erase (child++);
    } else {
      ++child;
    }
  }
}

MctsNode& MctsNode::FindUctChild (Player pl, float uct_explore_coeff) {
  MctsNode* best_child = NULL;
  float best_urgency = -large_float;
  const float explore_coeff = log (stat.update_count()) * uct_explore_coeff;

  assertc (mcts_tree_ac, has_all_legal_children [pl]);

  FOREACH (MctsNode& child, children) {
    if (child.player != pl) continue;
    float child_urgency = child.stat.ucb (pl, explore_coeff);
    if (child_urgency > best_urgency) {
      best_urgency = child_urgency;
      best_child   = &child;
    }
  }

  assertc (mcts_tree_ac, best_child != NULL); // at least pass
  return *best_child;
}

#endif
