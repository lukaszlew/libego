/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                           *
 *  This file is part of Library of Effective GO routines - EGO library      *
 *                                                                           *
 *  Copyright 2006 and onwards, Lukasz Lew                                   *
 *                                                                           *
 *  EGO library is free software; you can redistribute it and/or modify      *
 *  it under the terms of the GNU General Public License as published by     *
 *  the Free Software Foundation; either version 2 of the License, or        *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  EGO library is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public License        *
 *  along with EGO library; if not, write to the Free Software               *
 *  Foundation, Inc., 51 Franklin St, Fifth Floor,                           *
 *  Boston, MA  02110-1301  USA                                              *
 *                                                                           *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

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

  Vertex v;
  while (true) { // TODO separate iterator
    v = board->empty_v [ii];
    if (!board->is_eyelike (act_player, v) &&
        board->is_pseudo_legal (act_player, v)) { 
      board->play_legal(act_player, v);
      return;
    }
    ii += 1;
    ii &= ~(-(ii == board->empty_v_cnt)); // if (ii==board->empty_v_cnt) ii=0;
    if (ii == ii_start) {
      board->play_legal(act_player, Vertex::pass());
      return;
    }
  }
}

