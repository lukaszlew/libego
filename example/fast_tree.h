#ifndef _FAST_TREE_
#define _FAST_TREE_

#include <list>

const bool tree_ac = false;

// -----------------------------------------------------------------------------

// TODO replace default std::allocator with FastPool
template <class Data, template <class N> class Allocator = std::allocator>
class Node : public Data {
public:
  typedef std::list<Node, Allocator<Node> > ChildrenList; // TODO vector?
  typedef typename ChildrenList::iterator ChildrenListIterator;

  explicit Node (const Data& data) : Data (data) { }

  void AddChild (const Data& data) {
    children.push_front (Node(data));
  }

  // TODO better implementation of child removation.
  void RemoveChild (Node* child) {
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

private:
  ChildrenList children;
};

// -----------------------------------------------------------------------------

#endif
