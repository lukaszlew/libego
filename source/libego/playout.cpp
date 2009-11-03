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
    if (board->move_no >= max_moves) return false;
    // if (abs(board->StoneScore ()) > 25) return mercy;
    PlayOneMove ();
  }
}

all_inline
void LightPlayout::PlayOneMove () {
  uint ii_start = random.GetNextUint (board->empty_v_cnt); 
  uint ii = ii_start;
  Player act_player = board->ActPlayer ();

  while (true) { // TODO separate iterator
    Vertex v = board->empty_v [ii];
    if (!board->IsEyelike (act_player, v) &&
        board->IsPseudoLegal (act_player, v)) { 
      board->PlayLegal(act_player, v);
      return;
    }
    ii += 1;
    ii &= ~(-(ii == board->empty_v_cnt)); // if (ii==board->empty_v_cnt) ii=0;
    if (ii == ii_start) {
      board->PlayLegal(act_player, Vertex::Pass());
      return;
    }
  }
}

