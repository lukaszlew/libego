#ifndef MCTS_TREE_
#define MCTS_TREE_

#include <list>

const bool mcts_tree_ac = true;

// -----------------------------------------------------------------------------

class MctsNode {
public:
  typedef std::list<MctsNode> ChildrenList; // TODO vector, allocator?

  // Initialization.

  explicit MctsNode (Player player_, Vertex v_);

  void Reset ();

  // Printing.

  string ToString() const;

  string RecToString (float min_visit, uint max_children) const; 

  // Children operations.
  
  ChildrenList& Children ();

  void AddChild (const MctsNode& node);

  void RemoveChild (MctsNode* child_ptr);

  void EnsureAllPseudoLegalChildren (Player pl, const Board& board);

  void RemoveIllegalChildren (Player pl, const FullBoard& full_board);

  // Child finding.

  MctsNode* AddFindChild (Move m, Board& board);

  const MctsNode& MostExploredChild (Player pl);

  // Other.
  
  float SubjectiveMean() const;

public:

  Move GetMove () const;

  Player player;
  Vertex v;
  NatMap <Player, bool> has_all_legal_children;

  Stat stat;
  Stat rave_stat;

private:
  void RecPrint (ostream& out, uint depth, float min_visit, uint max_children) const;

  ChildrenList children;
};

// -----------------------------------------------------------------------------

MctsNode::MctsNode (Player player_, Vertex v_) : player(player_), v(v_) {
  Reset ();
}

MctsNode::ChildrenList& MctsNode::Children () {
  return children;
}

Move MctsNode::GetMove () const {
  return Move(player, v);
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

MctsNode* MctsNode::AddFindChild (Move m, Board& board) {
  // TODO make invariant about haveChildren and has_all_legal_children
  Player pl = m.get_player();
  Vertex v  = m.get_vertex();
  EnsureAllPseudoLegalChildren (pl, board);
  BOOST_FOREACH (MctsNode& child, children) {
    if (child.player == pl && child.v == v) {
      board.play_legal (pl, v);
      assertc (mcts_tree_ac, board.last_move_status == Board::play_ok);
      return &child;
    }
  }
  assert (false);
}

string MctsNode::ToString() const {
  stringstream s;
  s << player.to_string () << " " 
    << v.to_string () << " " 
    << stat.to_string() << " "
    << rave_stat.to_string()
    ;
  return s.str();
}

namespace {
  bool SubjectiveCmp (const MctsNode* a, const MctsNode* b) {
    return a->SubjectiveMean () > b->SubjectiveMean ();
  }
}

void MctsNode::RecPrint (ostream& out, uint depth, float min_visit, uint max_children) const {
  rep (d, depth) out << "  ";
  out << ToString () << endl;

  vector <const MctsNode*> child_tab;
  BOOST_FOREACH (const MctsNode& child, children) {
    child_tab.push_back(&child);
  }

  sort (child_tab.begin(), child_tab.end(), SubjectiveCmp);
  if (child_tab.size () > max_children) child_tab.resize(max_children);

  BOOST_FOREACH (const MctsNode* child, child_tab) {
    if (child->stat.update_count() >= min_visit) {
      child->RecPrint (out, depth + 1, min_visit, max(1u, max_children - 1));
    }
  }
}

string MctsNode::RecToString (float min_visit, uint max_children) const { 
  ostringstream out;
  RecPrint (out, 0, min_visit, max_children); 
  return out.str ();
}

const MctsNode& MctsNode::MostExploredChild (Player pl) {
  const MctsNode* best = NULL;
  float best_update_count = -1;

  assertc (mcts_tree_ac, has_all_legal_children [pl]);

  BOOST_FOREACH (const MctsNode& child, children) {
    if (child.player == pl && child.stat.update_count() > best_update_count) {
      best_update_count = child.stat.update_count();
      best = &child;
    }
  }

  assertc (mcts_tree_ac, best != NULL);
  return *best;
}

void MctsNode::EnsureAllPseudoLegalChildren (Player pl, const Board& board) {
  if (has_all_legal_children [pl]) return;
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

void MctsNode::Reset () {
  has_all_legal_children.SetAll (false);
  children.clear ();
  stat.reset ();
  rave_stat.reset ();
}

float MctsNode::SubjectiveMean () const {
  return player.subjective_score (stat.mean ());
}

#endif
