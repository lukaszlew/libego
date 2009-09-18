#ifndef _FAST_TREE_
#define _FAST_TREE_

// -----------------------------------------------------------------------------

// TODO extract iterator with path
// pool podawac jako parametr

template <class Data>
class TreeT {
public:

  class Node;
  class Iterator;
};

// -----------------------------------------------------------------------------

template <class Data>
class TreeT<Data> :: Iterator {
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
    path.push_back(path.back()->child(v));
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
class TreeT<Data> :: Node : public Data {
public:

  class Iterator;

  Iterator children() {
    return Iterator(*this);
  }

  void init () {
    children_.memset(NULL);
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

template <class Data>
class TreeT<Data> :: Node :: Iterator {
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
private:
  Node& parent_;
  Vertex act_v_;
};

#endif
