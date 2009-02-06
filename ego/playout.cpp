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

SimplePolicy::SimplePolicy () : board (NULL) { }

void SimplePolicy::begin_playout (Board* board_) { 
  board = board_;
}

void SimplePolicy::prepare_vertex () {
  act_player     = board->act_player ();
  to_check_cnt   = board->empty_v_cnt;
  act_evi        = fr.rand_int (board->empty_v_cnt); 
}

Vertex SimplePolicy::next_vertex () {
  Vertex v;
  while (true) {
    if (to_check_cnt == 0) return Vertex::pass ();
    to_check_cnt--;
    v = board->empty_v [act_evi];
    act_evi++;
    if (act_evi == board->empty_v_cnt) act_evi = 0;
    if (!board->is_eyelike (act_player, v)) return v;
  }
}

void SimplePolicy::bad_vertex (Vertex) {
}

void SimplePolicy::played_vertex (Vertex) { 
}

void SimplePolicy::end_playout (playout_status_t) { 
}

FastRandom SimplePolicy::fr(123);
