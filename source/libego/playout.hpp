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

  static const uint kDefaultMaxMoves = Board::kArea * 2;

  typedef FastStack <Move, kDefaultMaxMoves> MoveHistory;

public:

  LightPlayout (Board* board_, FastRandom& random_);

  // The same as Run() but stores the moves on given stack as long as possible.
  template <uint stack_size>
  bool Run (FastStack<Move, stack_size>& history);

private:
  Board* board;
  FastRandom& random;
};

// -----------------------------------------------------------------------------
// internatl implementation

template <uint stack_size> all_inline
bool LightPlayout::Run (FastStack<Move, stack_size>& history) {
  while (true) {
    if (board->BothPlayerPass ())  return true;
    if (history.IsFull ()) return false;
    Player pl = board->ActPlayer ();
    Vertex v  = board->RandomLightMove (pl, random);
    board->PlayPseudoLegal (pl, v);
    history.Push (board->LastMove());
  }
}

#endif
