//
// Copyright 2006 and onwards, Lukasz Lew
//

#include "playout.h"


LightPlayout::LightPlayout (Board* board_, FastRandom& random_, uint max_moves_)
: board (board_), random (random_), max_moves (max_moves_)
{
}

all_inline
LightPlayout::Status LightPlayout::Run () {
  while (true) {
    if (board->both_player_pass ())  return pass_pass;
    if (board->move_no >= max_moves) return too_long;
    // if (abs(board->approx_score ()) > 25) return mercy;
    PlayOneMove ();
  }
}

all_inline
void LightPlayout::PlayOneMove () {
  uint ii_start = random.rand_int (board->empty_v_cnt); 
  uint ii = ii_start;
  Player act_player = board->act_player ();

  while (true) { // TODO separate iterator
    Vertex v = board->empty_v [ii];
    if (!board->is_eyelike (act_player, v) &&
        board->is_pseudo_legal (act_player, v)) { 
      board->play_legal(act_player, v);
      return;
    }
    ii += 1;
    ii &= ~(-(ii == board->empty_v_cnt)); // if (ii==board->empty_v_cnt) ii=0;
    if (ii == ii_start) {
      board->play_legal(act_player, Vertex::Pass());
      return;
    }
  }
}

