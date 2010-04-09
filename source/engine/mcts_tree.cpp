#include <list>
#include <boost/foreach.hpp>
#include "mcts_tree.hpp"
#include "gtp_gogui.hpp"

extern Gtp::ReplWithGogui gtp;

MctsNode::MctsNode (Player player, Vertex v, double bias)
: player(player), v(v), has_all_legal_children (false), bias(bias)
{
  ASSERT2 (!isnan (bias), WW(bias));
  ASSERT2 (bias >= 0.0, WW(bias));
  ASSERT2 (bias <= 1.0, WW(bias));
  Reset ();
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

  return NULL; // no child
}

string MctsNode::ToString() const {
  stringstream s;
  s << player.ToGtpString() << " " 
    << v.ToGtpString() << " " 
    << stat.to_string() << " "
    << rave_stat.to_string() << " + "
    << bias << " -> "
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

string MctsNode::GuiString() const {
  stringstream s;
  s << player.ToGtpString() << " " 
    << v.ToGtpString() << endl
    << "MCTS: " << stat.to_string() << endl
    << "RAVE: " << rave_stat.to_string() << endl
    << "Bias: " << bias << endl
    << "Mix:  "
    << Stat::Mix (stat,      Param::tree_stat_bias,
                  rave_stat, Param::tree_rave_bias) << endl
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

  BOOST_FOREACH (MctsNode& child, children) {
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
    pl.SubjectiveScore (value)
    + Param::tree_explore_coeff * sqrt (log_val / stat.update_count())
    + Param::tree_progressive_bias * bias
    / max (1.0f, (stat.update_count () + Param::tree_progressive_bias_prior));
  // TODO other equation for PB
}


// -----------------------------------------------------------------------------

void MctsTrace::Reset (MctsNode& node) {
  nodes.clear();
  nodes.push_back (&node);
  moves.clear ();
  moves.push_back (node.GetMove());
}


void MctsTrace::NewNode (MctsNode& node) {
  nodes.push_back (&node);  
}


void MctsTrace::NewMove (Move m) {
  moves.push_back (m);
}


void MctsTrace::UpdateTraceRegular (float score) {
  BOOST_FOREACH (MctsNode* node, nodes) {
    node->stat.update (score);
  }

  if (Param::tree_rave_update) {
    UpdateTraceRave (score);
  }
}


void MctsTrace::UpdateTraceRave (float score) {
  // TODO configure rave blocking through options

  uint last_ii  = moves.size () * Param::tree_rave_update_fraction;
  // TODO tune that

  rep (act_ii, nodes.size()) {
    // Mark moves that should be updated in RAVE children of: trace [act_ii]
    NatMap <Move, bool> do_update (false);
    NatMap <Move, bool> do_update_set_to (true);
    ForEachNat (Player, pl) do_update_set_to [Move (pl, Vertex::Pass())] = false;

    // TODO this is the slow and too-fixed part
    // TODO Change it to weighting with flexible masking.
    reps (jj, act_ii+1, last_ii) {
      Move m = moves [jj];
      do_update [m] = do_update_set_to [m];
      do_update_set_to [m] = false;
      do_update_set_to [m.OtherPlayer()] = false;
    }

    // Do the update.
    BOOST_FOREACH (MctsNode& child, nodes[act_ii]->children) {
      if (do_update [child.GetMove()]) {
        child.rave_stat.update (score);
      }
    }
  }
}

// -----------------------------------------------------------------------------
