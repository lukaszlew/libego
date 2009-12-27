//
// Copyright 2006 and onwards, Lukasz Lew
//

#include "playout.hpp"

LightPlayout::LightPlayout (Board* board_, FastRandom& random_, uint max_moves_)
: board (board_), random (random_), max_moves (max_moves_)
{
}

all_inline
bool LightPlayout::Run() {
  while (true) {
    if (board->BothPlayerPass ())  return true;
    if (board->MoveCount () >= max_moves) return false;
    // if (abs(board->StoneScore ()) > 25) return mercy;
    Player pl = board->ActPlayer ();
    Vertex v  = board->RandomLightMove (pl, random);
    board->PlayPseudoLegal (pl, v);
  }
}

