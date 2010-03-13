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
  
  ChildrenList& Children ();

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


struct Mcts {
  Mcts ();

  void Reset ();

  void SyncRoot (const Board& board, const Gammas& gammas);
  Move BestMove (Player player);
  void NewPlayout ();
  void EnsureAllLegalChildren (MctsNode* node, Player pl, const Board& board, const Sampler& sampler);
  void RemoveIllegalChildren (MctsNode* node, Player pl, const Board& full_board);
  void NewMove (Move m);

  Move ChooseMove (Board& play_board, const Sampler& sampler);
  void UpdateTraceRegular (float score);
  void UpdateTraceRave (float score);

  MctsNode& ActNode();

  void GtpShowTree (Gtp::Io& io);

private:

  MctsNode root;
  MctsNode* act_root;

  vector <MctsNode*> trace;               // nodes in the path
  vector <Move> move_history;
  uint tree_move_count;

public:
  bool tree_phase;
};


#endif
