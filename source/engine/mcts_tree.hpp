#ifndef MCTS_TREE_
#define MCTS_TREE_

#include <list>
#include "stat.hpp"
#include "gtp.hpp"


class MctsNode {
public:
  typedef std::list<MctsNode> ChildrenList; // TODO vector, allocator?

  // Initialization.

  explicit MctsNode (Player player, Vertex v, double bias);

  void Reset ();

  // Printing.

  string ToString() const;

  string RecToString (float min_visit, uint max_children) const; 

  // Children operations.
  
  void AddChild (const MctsNode& node);

  void RemoveChild (MctsNode* child_ptr);

  bool ReadyToExpand () const;

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
  const double bias;

  void RecPrint (ostream& out, uint depth, float min_visit, uint max_children) const;

  ChildrenList children;
};

// -----------------------------------------------------------------------------

struct MctsTrace {
public:

  void Reset (MctsNode& node);
  void NewMove (Move m);
  void NewNode (MctsNode& node);
  void UpdateTraceRegular (float score);
  void UpdateTraceRave (float score);

private:
  vector <MctsNode*> nodes;
  vector <Move> moves;
};

// -----------------------------------------------------------------------------

struct Mcts {
};


#endif
