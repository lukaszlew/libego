//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef PLAYOUT_H_
#define PLAYOUT_H_

#include "utils.hpp"
#include "board.hpp"
#include "fast_stack.hpp"

#include <cmath>

template <uint stack_size> all_inline
bool DoLightPlayout (Board& board,
                     FastRandom& random, 
                     FastStack<Move, stack_size>& history)
{
  while (true) {
    if (board.BothPlayerPass ())  return true;
    if (history.IsFull ()) return false;
    Player pl = board.ActPlayer ();
    Vertex v  = board.RandomLightMove (pl, random);
    board.PlayLegal (pl, v);
    history.Push (board.LastMove());
  }
}

#endif
