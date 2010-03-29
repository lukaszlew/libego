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
  Move Genmove ();
  void DoNPlayouts (uint n);
  void PrepareToPlayout ();
  void DoOnePlayout ();
  Move ChooseMove ();
  void PlayMove (Move m);
  void Sync ();
  double Score (bool accurate);
  Move ChooseLocalMove (); // TODO policy randomization

  double GetStatForVertex (Vertex v);
  std::string GetStringForVertex (Vertex v);
  vector<Move> LastPlayout ();

private:
  Board base_board;
  TimeControl time_control;

  Board play_board;
  vector<Move> playout_moves;
  Mcts mcts;
  FastRandom random;
  Gammas gammas;
  Sampler sampler;

  friend class MctsGtp;
};

#endif /* ENGINE_H_ */
