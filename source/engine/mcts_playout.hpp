#ifndef MCTS_PLAYOUT_H_
#define MCTS_PLAYOUT_H_

//
// Copyright 2006 and onwards, Lukasz Lew
//

#include "ego.hpp"
#include "time_control.hpp"
#include "mcts_tree.hpp"

class MctsPlayout {
public:

  MctsPlayout (const Board& base_board);

  void Reset ();
  Move Genmove ();
  void DoNPlayouts (uint n);
  void PrepareToPlayout ();

  void DoOnePlayout ();
  Move ChooseMove ();

  void PlayMove (Move m);
  void Sync ();

  vector<Move> LastPlayout ();

  double Score (bool accurate);

  // TODO policy randomization
  Move ChooseLocalMove ();

private:
  
  const Board& base_board;
  TimeControl time_control;

  // playout
  Board play_board;
  vector<Move> playout_moves;

  Mcts mcts;

public:
  friend class MctsGtp;
  FastRandom random;
  Gammas gammas;
  Sampler sampler;
};

#endif /* MCTS_PLAYOUT_H_ */
