#ifndef _FAST_TREE_
#define _FAST_TREE_

// -----------------------------------------------------------------------------

template <class Data>
class Node : public Data {
public:

  class Iterator;
  class ChildrenIterator;

  // TODO replace this by placement new in pool or Boost::pool
  void PoolConstruct () {
    children.memset(NULL);
    child_count = 0;
  }

  void AttachChild (Vertex v, Node* new_child) {
    assertc (tree_ac, children[v] == NULL);
    children[v] = new_child;
    child_count += 1;
  }

  void DeattachChild (Vertex v) {
    assertc (tree_ac, children[v] != NULL);
    children[v] = NULL;
    child_count -= 1;
  }

  bool HaveChildren () {
    return child_count > 0;
  }

private:
  FastMap<Vertex, Node*> children;
  int child_count;
};

// -----------------------------------------------------------------------------

template <class Data>
class Node<Data> :: Iterator {
public:
  void SetRoot (Node* root) {
    path.clear();
    path.push_back(root);
  }
  
  void ResetToRoot () {
    assertc (tree_ac, path.size() > 0);
    path.resize(1);
  }

  void Descend (Vertex v) {
    assertc (tree_ac, path.size() > 0);
    path.push_back(path.back()->children[v]);
    assertc (tree_ac, ActNode () != NULL);
  }
  
  void Ascend () {
    assertc (tree_ac, path.size() >= 2);
    path.pop_back ();
  }

  vector<Node*>& Path () {
    assertc (tree_ac, path.size() > 0);
    return path;
  }

  Node* operator-> () { return ActNode (); }
  operator Node* ()   { return ActNode (); }

private:
  Node* ActNode() {
    assertc (tree_ac, path.size() > 0);
    return path.back();
  }

private:
  vector<Node*> path;
};

// -----------------------------------------------------------------------------

template <class Data>
class Node<Data> :: ChildrenIterator {
public:

  ChildrenIterator(Node& parent) : parent_(parent), act_v_(0) { 
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

#endif
