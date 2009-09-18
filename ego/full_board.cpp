/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
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
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "full_board.h"
#include "testing.h"


void FullBoard::clear(float komi) {
  // move_no = 0;
  Board::clear(komi);
  move_history.Clear();
}


void FullBoard::set_komi (float fkomi) {
  Board::set_komi(fkomi);
}


void FullBoard::set_act_player (Player pl) {
  Board::set_act_player(pl);
}


void FullBoard::load (const FullBoard* save_board) {
  memcpy(this, save_board, sizeof(FullBoard));
}


void FullBoard::play_legal (Player player, Vertex v) {
  Board::play_legal(player, v);
  Move m = Board::last_move(); // TODO why can't I inline it?
  move_history.Push(m);
}


bool FullBoard::undo () {
  Move replay [max_game_length];

  uint   game_length  = move_no;
  float  old_komi     = komi ();

  if (game_length == 0)
    return false;

  rep (mn, game_length-1)
    replay [mn] = move_history [mn];

  clear (old_komi);  // TODO maybe last_player should be preserverd as well

  rep (mn, game_length-1)
    play_legal (replay [mn].get_player (), replay [mn].get_vertex ());

  return true;
}


bool FullBoard::is_legal (Player pl, Vertex v) const {
  FullBoard tmp;
  tmp.load(this);
  return tmp.try_play (pl, v);
}


bool FullBoard::is_hash_repeated () {
  Board tmp_board;
  rep (mn, move_no-1) {
    tmp_board.play_legal (move_history[mn].get_player (),
                          move_history[mn].get_vertex ());
    if (hash() == tmp_board.hash())
      return true;
  }
  return false;
}


bool FullBoard::try_play (Player player, Vertex v) {
  if (v == Vertex::pass ()) {
    play_legal (player, v);
    return true;
  }

  v.check_is_on_board ();

  if (color_at [v] != Color::empty ())
    return false;

  if (is_pseudo_legal (player,v) == false)
    return false;

  play_legal (player, v);

  if (last_move_status != play_ok) {
    bool ok = undo ();
    assert(ok);
    return false;
  }

  if (is_hash_repeated ()) {
    bool ok = undo ();
    assert(ok);
    return false;
  }

  return true;
}


const Board& FullBoard::board() const {
  return *this;
}
