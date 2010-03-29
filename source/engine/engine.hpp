//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef ENGINE_H_
#define ENGINE_H_

#include "to_string.hpp"
#include "gtp_gogui.hpp"
#include "ego.hpp"
#include "time_control.hpp"
#include "mcts_tree.hpp"

class Engine {
public:
  Engine ();

  bool SetBoardSize (uint board_size);
  void SetKomi (float komi);
  void ClearBoard ();
  bool Play (Move move);
  Move Genmove (Player player);
  bool Undo ();

  const Board& GetBoard () const;

  // Playout functions
  Move ChooseBestMove ();
  void DoNPlayouts (uint n);
  void SyncRoot ();
  void PrepareToPlayout ();
  void DoOnePlayout ();
  void PlayMove (Move m);
  double Score (bool accurate);

  double GetStatForVertex (Vertex v);
  std::string GetStringForVertex (Vertex v);
  vector<Move> LastPlayout ();

private:
  TimeControl time_control;
  FastRandom random;
  Mcts mcts;
  Gammas gammas;
  Sampler sampler;

  Board base_board;
  MctsNode* act_root;
  
  Board play_board;

  vector<Move> playout_moves;

  friend class MctsGtp;
};

#endif /* ENGINE_H_ */
