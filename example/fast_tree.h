#ifndef _FAST_TREE_
#define _FAST_TREE_

// -----------------------------------------------------------------------------

template <class NodeData>
class TreeT {
public:

  class Node;

  TreeT () : node_pool(mcts_max_nodes) {
  }

  void init () {
    node_pool.reset();
    path.clear();
    Node* new_node = node_pool.malloc();
    path.push_back(new_node);
    new_node->init (); // TODO move to malloc // TODO use Pool Boost
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

  Node* alloc_child (Vertex v) {
    Node* new_node;
    new_node = node_pool.malloc ();
    new_node->init ();
    act_node ()->add_child (v, new_node);
    return new_node;
  }

  void delete_act_node (Vertex v) {
    assertc (tree_ac, !act_node ()->have_children ());
    assertc (tree_ac, path.size() >= 2);
    path.pop_back();
    path.back()->remove_child (v);
  }

  void free_subtree (Node* parent) {
    for(typename Node::Iterator child(*parent); child; ++child) {
      free_subtree(child);
      node_pool.free(child);
    };
  }

  // TODO free history (for sync with base board)

  vector<Node*>& history () {
    return path;
  }

private:
  static const uint mcts_max_nodes = 1000000;
  FastPool <Node> node_pool;
  vector<Node*> path;
};

// -----------------------------------------------------------------------------

template <class NodeData>
class TreeT<NodeData> :: Node : public NodeData {
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

template <class NodeData>
class TreeT<NodeData> :: Node :: Iterator {
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
