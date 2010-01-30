#ifndef MCTS_TREE_
#define MCTS_TREE_

#include <list>

const bool mcts_tree_ac = true;

// -----------------------------------------------------------------------------
// TODO move to separate class and static object

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

  bool ReadyToExpand () const;

  void EnsureAllLegalChildren (Player pl, const Board& board);

  void RemoveIllegalChildren (Player pl, const Board& full_board);

  // Child finding.

  MctsNode* FindChild (Move m);

  const MctsNode& MostExploredChild (Player pl) const;

  MctsNode& BestRaveChild (Player pl);

  // Other.
  
  float SubjectiveMean() const;

  float SubjectiveRaveValue (Player pl, float log_val) const;

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

  static const bool kCheckAsserts = false;
};

// -----------------------------------------------------------------------------

MctsNode::MctsNode (Player player_, Vertex v_)
: player(player_), v(v_), has_all_legal_children (false) {
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
    ASSERT (child != children.end());
    if (&*child == child_ptr) {
      children.erase(child);
      return;
    }
    child++;
  }
}

bool MctsNode::ReadyToExpand () const {
  return stat.update_count() > 
    Param::prior_update_count + Param::mature_update_count;
}

MctsNode* MctsNode::FindChild (Move m) {
  // TODO make invariant about haveChildren and has_all_legal_children
  Player pl = m.GetPlayer();
  Vertex v  = m.GetVertex();
  ASSERT (has_all_legal_children [pl]);
  BOOST_FOREACH (MctsNode& child, children) {
    if (child.player == pl && child.v == v) {
      return &child;
    }
  }
  FAIL ("should not happen");
}

string MctsNode::ToString() const {
  stringstream s;
  s << player.ToGtpString() << " " 
    << v.ToGtpString() << " " 
    << stat.to_string() << " "
    << rave_stat.to_string() << " -> "
    << Stat::Mix (stat,      Param::tree_stat_bias,
                  rave_stat, Param::tree_rave_bias)
    // << " - ("  << stat.precision (Param::mcts_bias) << " : "
    // << stat.precision (Param::rave_bias) << ")"
    // << Stat::SlowMix (stat,
    //                   Param::mcts_bias,
    //                   rave_stat,
    //                   Param::rave_bias)
    ;

  return s.str();
}

namespace {
  bool SubjectiveCmp (const MctsNode* a, const MctsNode* b) {
    return a->stat.update_count() > b->stat.update_count();
    // return SubjectiveMean () > b->SubjectiveMean ();
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

const MctsNode& MctsNode::MostExploredChild (Player pl) const {
  const MctsNode* best = NULL;
  float best_update_count = -1;

  ASSERT (has_all_legal_children [pl]);

  BOOST_FOREACH (const MctsNode& child, children) {
    if (child.player == pl && child.stat.update_count() > best_update_count) {
      best_update_count = child.stat.update_count();
      best = &child;
    }
  }

  ASSERT (best != NULL);
  return *best;
}


MctsNode& MctsNode::BestRaveChild (Player pl) {
  MctsNode* best_child = NULL;
  float best_urgency = -100000000000000.0; // TODO infinity
  const float log_val = log (stat.update_count());

  ASSERT (has_all_legal_children [pl]);

  BOOST_FOREACH (MctsNode& child, Children()) {
    if (child.player != pl) continue;
    float child_urgency = child.SubjectiveRaveValue (pl, log_val);
    if (child_urgency > best_urgency) {
      best_urgency = child_urgency;
      best_child   = &child;
    }
  }

  ASSERT (best_child != NULL); // at least pass
  return *best_child;
}


void MctsNode::EnsureAllLegalChildren (Player pl, const Board& board) {
  if (has_all_legal_children [pl]) return;
  empty_v_for_each_and_pass (&board, v, {
      // superko nodes have to be removed from the tree later
      if (board.IsLegal (pl, v))
        AddChild (MctsNode(pl, v));
    });
  has_all_legal_children [pl] = true;
}

void MctsNode::RemoveIllegalChildren (Player pl, const Board& full_board) {
  ASSERT (has_all_legal_children [pl]);

  ChildrenList::iterator child = children.begin();
  while (child != children.end()) {
    if (child->player == pl && !full_board.IsReallyLegal (Move (pl, child->v))) {
      children.erase (child++);
    } else {
      ++child;
    }
  }
}

void MctsNode::Reset () {
  has_all_legal_children.SetAll (false);
  children.clear ();
  stat.reset      (Param::prior_update_count,
                   player.SubjectiveScore (Param::prior_mean));
  rave_stat.reset (Param::prior_update_count,
                   player.SubjectiveScore (Param::prior_mean));
}

float MctsNode::SubjectiveMean () const {
  return player.SubjectiveScore (stat.mean ());
}

float MctsNode::SubjectiveRaveValue (Player pl, float log_val) const {
  float value;

  if (Param::tree_rave_use) {
    value = Stat::Mix (stat,      Param::tree_stat_bias,
                       rave_stat, Param::tree_rave_bias);
  } else {
    value = stat.mean ();
  }

  return
    pl.SubjectiveScore (value) +
    Param::tree_explore_coeff * sqrt (log_val / stat.update_count());
}

// -----------------------------------------------------------------------------

struct Tree {
  Tree () :
    root (Player::White(), Vertex::Any ())
  {
  }

  void Reset () {
    root.Reset ();
  }

  MctsNode& FindRoot (const Board& board) {
    Board sync_board;
    MctsNode* act_root = &root;
    BOOST_FOREACH (Move m, board.Moves ()) {
      act_root->EnsureAllLegalChildren (m.GetPlayer(), sync_board);
      act_root = act_root->FindChild (m);
      CHECK (sync_board.IsLegal (m));
      sync_board.PlayLegal (m);
    }
    
    Player pl = board.ActPlayer();
    act_root->EnsureAllLegalChildren (pl, board);
    act_root->RemoveIllegalChildren (pl, board);

    return *act_root;
  }


  MctsNode root;
};



// -----------------------------------------------------------------------------
// Tree iterator 

// TODO merge with tree

struct TT {
  void Reset (MctsNode& playout_root) {
    trace.clear();
    trace.push_back (&playout_root);
    move_history.clear ();
    move_history.push_back (playout_root.GetMove());
    tree_phase = Param::tree_use;
    tree_move_count = 0;
  }

  void NewMove (Move m) {
    move_history.push_back (m);
  }

  Vertex ChooseTreeMove (Board& play_board, Player pl) {
    if (!tree_phase || tree_move_count >= Param::tree_max_moves) {
      return Vertex::Any();
    }

    if (!ActNode().has_all_legal_children [pl]) {
      if (!ActNode().ReadyToExpand ()) {
        tree_phase = false;
        return Vertex::Any ();
      }
      ASSERT (pl == ActNode().player.Other());
      ActNode().EnsureAllLegalChildren (pl, play_board);
    }

    MctsNode& uct_child = ActNode().BestRaveChild (pl);
    trace.push_back (&uct_child);
    ASSERT (uct_child.v != Vertex::Any());
    tree_move_count += 1;
    return uct_child.v;
  }
  
  void UpdateTraceRegular (float score) {
    BOOST_FOREACH (MctsNode* node, trace) {
      node->stat.update (score);
    }

    if (Param::tree_rave_update) {
      UpdateTraceRave (score);
    }
  }

  void UpdateTraceRave (float score) {
    // TODO configure rave blocking through options


    uint last_ii  = move_history.size () * 7 / 8; // TODO 

    rep (act_ii, trace.size()) {
      // Mark moves that should be updated in RAVE children of: trace [act_ii]
      NatMap <Move, bool> do_update (false);
      NatMap <Move, bool> do_update_set_to (true);

      // TODO this is the slow and too-fixed part
      // TODO Change it to weighting with flexible masking.
      reps (jj, act_ii+1, last_ii) {
        Move m = move_history [jj];
        do_update [m] = do_update_set_to [m];
        do_update_set_to [m] = false;
        do_update_set_to [m.OtherPlayer()] = false;
      }

      // Do the update.
      BOOST_FOREACH (MctsNode& child, trace[act_ii]->Children()) {
        if (do_update [child.GetMove()]) {
          child.rave_stat.update (score);
        }
      }
    }
  }

  MctsNode& ActNode() {
    ASSERT (trace.size() > 0);
    return *trace.back ();
  }


private:
  vector <MctsNode*> trace;               // nodes in the path
  vector <Move> move_history;
  uint tree_move_count;
public:
  bool tree_phase;
};


#endif
