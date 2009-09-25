#ifndef _MCTS_TREE_
#define _MCTS_TREE_

#include <list>

const bool mcts_tree_ac = true;

// -----------------------------------------------------------------------------

class MctsNode {
public:
  typedef std::list<MctsNode> ChildrenList; // TODO vector, allocator?
  typedef ChildrenList::iterator ChildrenListIterator;

  explicit MctsNode (Player player_, Vertex v_) : player(player_), v(v_) {
    has_all_legal_children.SetAll (false);
  }

  void AddChild (const MctsNode& node) {
    children.push_front (node);
  }

  // TODO better implementation of child removation.
  void RemoveChild (MctsNode* child) {
    ChildrenListIterator it = children.begin();
    while (true) {
      assertc (mcts_tree_ac, it != children.end());
      if (&*it == child) {
        children.erase(it);
        return;
      }
      it++;
    }
  }

  bool HaveChildren () {
    return !children.empty();
  }

  ChildrenList& Children () {
    return children;
  }

  const ChildrenList& Children () const {
    return children;
  }

public:

  string ToString() const {
    stringstream s;
    s << player.to_string () << " " 
      << v.to_string () << " " 
      << stat.to_string()
      ;
    return s.str();
  }

  Player player;
  Vertex v;
  FastMap <Player, bool> has_all_legal_children;

  Stat stat;                    // stat is initalized during construction

  const MctsNode& MostExploredChild (Player pl) {
    const MctsNode* best = NULL;
    float best_update_count = -1;

    assertc (mcts_tree_ac, has_all_legal_children [pl]);

    FOREACH (const MctsNode& child, Children()) {
      if (child.player == pl && child.stat.update_count() > best_update_count) {
        best_update_count = child.stat.update_count();
        best = &child;
      }
    }

    assertc (mcts_tree_ac, best != NULL);
    return *best;
  }

private:
  ChildrenList children;
};

// -----------------------------------------------------------------------------

class TreeToString {
public:
  string operator () (const MctsNode& node, float min_visit_) { 
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
    bool operator()(const MctsNode* a, const MctsNode* b) {
      if (player_ == Player::black ()) {
        return a->stat.mean() < b->stat.mean();
      } else {
        return a->stat.mean() > b->stat.mean();
      }
    }
    Player player_;
  };

  void RecPrint (const MctsNode& node) {
    rep (d, depth) out << "  ";
    out << node.ToString () << endl;

    vector <const MctsNode*> child_tab;
    FOREACH (const MctsNode& child, node.Children())  child_tab.push_back(&child);
    sort (child_tab.begin(), child_tab.end(), CompareNodeMean (node.player));

    depth += 1;
    FOREACH (const MctsNode* child, child_tab) {
      if (child->stat.update_count() >= min_visit) RecPrint (*child);
    }
    depth -= 1;
  }

private:
  ostringstream out;
  uint depth;
  float min_visit;
};

// -----------------------------------------------------------------------------
#endif
