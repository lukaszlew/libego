//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef PLAYOUT_H_
#define PLAYOUT_H_

#include "utils.hpp"
#include "board.hpp"
#include "fast_stack.hpp"

#include <cmath>

// mercy rule


class LightPlayout {
public:

  static const uint kDefaultMaxMoves = Board::area * 2;

  typedef FastStack <Move, kDefaultMaxMoves> MoveHistory;

public:

  LightPlayout (Board* board_,
                FastRandom& random_,
                uint max_moves = kDefaultMaxMoves);

  // Look at Run() implementation in playout.cpp
  bool Run();
  
  // The same as Run() but stores the moves on given stack as long as possible.
  template <uint stack_size>
  bool Run (FastStack<Move, stack_size>& history);

  // Plays one move according to Light policy
  void PlayOneMove ();

private:
  Board* board;
  FastRandom& random;
  uint max_moves;
};

// -----------------------------------------------------------------------------
// internatl implementation

template <uint stack_size> all_inline
bool LightPlayout::Run (FastStack<Move, stack_size>& history) {
  uint last_move = board->move_no + max_moves;
  while (true) {
    if (board->both_player_pass ())  return true;
    if (board->move_no >= last_move) return false;
    PlayOneMove ();
    if (!history.IsFull ()) history.Push (board->last_move());
  }
}

#endif
