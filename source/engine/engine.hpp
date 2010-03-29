#ifndef ENGINE_H_
#define ENGINE_H_
//
// Copyright 2006 and onwards, Lukasz Lew
//

#include "logger.hpp"
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

  string BoardAsciiArt ();
  Gtp::GoguiGfx LastPlayoutGfx (uint move_count);
  void ShowGammas (Gtp::GoguiGfx& gfx);

  double GetStatForVertex (Vertex /*v*/);
  std::string GetStringForVertex (Vertex v);

private:
  // Playout functions
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
  // base board
  Board base_board;

  // logging
  Logger logger;
  

  TimeControl time_control;

  // playout
  Board play_board;
  vector<Move> playout_moves;

  Mcts mcts;

  FastRandom random;
  Gammas gammas;
  Sampler sampler;
private:
  friend class MctsGtp;
};

#endif /* ENGINE_H_ */
