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

#include "move.h"

void Move::check () {
  Player::OfRaw (idx & 0x1);
  // TODO
  //Vertex::OfRaw (idx & ((1 << Vertex::bits_used) - 1)).check();
}

Move::Move (Player player, Vertex v) { 
  idx = player.GetRaw () | (v.GetRaw () << 1);
}

Move::Move () : idx(-1) {
}

Move::Move (int idx_) {
  idx = idx_;
}

Move Move::other_player () {
  return Move (idx ^ 0x1);
};

Player Move::get_player () { 
  return Player::OfRaw (idx & 0x1);
}

Vertex Move::get_vertex () { 
  return Vertex::OfRaw (idx >> 1) ; 
}

string Move::to_string () {
  return
    get_player().ToGtpString() + " " +
    get_vertex().ToGtpString();
}

uint Move::GetRaw () { return idx; }

bool Move::operator== (Move other) const { return idx == other.idx; }
bool Move::operator!= (Move other) const { return idx != other.idx; }

bool Move::in_range () const {
  return idx < kBound;
}

void Move::next () {
  idx++; 
}

