#ifndef _FAST_TREE_
#define _FAST_TREE_

#include <list>

const bool tree_ac = false;

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
      assertc (tree_ac, it != children.end());
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

private:
  ChildrenList children;
};

// -----------------------------------------------------------------------------

#endif
