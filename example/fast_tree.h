#ifndef _FAST_TREE_
#define _FAST_TREE_

// -----------------------------------------------------------------------------

// TODO replace default std::allocator with FastPool
template <class Data, template <class N> class Allocator = std::allocator>
class Node : public Data, private Allocator<Node<Data, Allocator> > {
public:

  // TODO replace this by placement new in pool or Boost::pool
  explicit Node (const Data& data) : Data (data) {
    children.memset(NULL);
    child_count = 0;
  }

  ~Node () {
    for (ChildrenIterator child(*this); child; ++child) {
      if ((Node*)child != NULL) {
        this->destroy    (child);
        this->deallocate (child, 1);
        child_count -= 1;
      }
    }
    assertc (tree_ac, child_count == 0);
  }

  Node* AddChild (const Data& data) {
    assertc (tree_ac, children[data.v] == NULL);
    Node* child = this->allocate (1);
    this->construct (child, Node(data));
    children[data.v] = child;
    child_count += 1;
    return child;
  }

  void RemoveChild (Node* child) {
    assertc (tree_ac, children[child->v] != NULL);
    this->destroy    (child);
    this->deallocate (child, 1);
    children[child->v] = NULL;
    child_count -= 1;
  }

  bool HaveChildren () {
    return child_count > 0;
  }

  // ------------------------------------------------------------------

  class ChildrenIterator {
  public:

    explicit ChildrenIterator(Node& parent) : parent_(parent), act_v_(0) { 
      Sync ();
    }

    Node* operator-> () { return parent_.children[act_v_]; }
    operator Node* ()   { return parent_.children[act_v_]; }

    void operator++ () {
      act_v_.next();
      Sync ();
    }

    operator bool () { return act_v_.in_range(); }

  private:
    void Sync () {
      while (act_v_.in_range () && parent_.children[act_v_] == NULL) {
        act_v_.next();
      }
    }

  private:
    Node& parent_;
    Vertex act_v_;
  };

  // ------------------------------------------------------------------

private:
  FastMap<Vertex, Node*> children;
  int child_count;
};

// -----------------------------------------------------------------------------

#endif
