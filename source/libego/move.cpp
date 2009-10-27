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
  Player (idx >> Vertex::bits_used);
  Vertex (idx & ((1 << Vertex::bits_used) - 1)).check();
}

Move::Move (Player player, Vertex v) { 
  idx = (player.get_idx () << Vertex::bits_used) | v.get_idx ();
}

Move::Move () {
  Move (Player::black (), Vertex::any ());
}

Move::Move (int idx_) {
  idx = idx_;
}

Move Move::other_player () {
  return Move (idx ^ (1 << Vertex::bits_used));
};

Player Move::get_player () { 
  return Player (idx >> Vertex::bits_used);
}

Vertex Move::get_vertex () { 
  return Vertex (idx & ((1 << ::Vertex::bits_used) - 1)) ; 
}

string Move::to_string () {
  return get_player ().to_string () + " " + get_vertex ().to_string ();
}

uint Move::get_idx () { return idx; }

bool Move::operator== (Move other) const { return idx == other.idx; }
bool Move::operator!= (Move other) const { return idx != other.idx; }

bool Move::in_range () const {
  return idx < kBound;
}

void Move::next () {
  idx++; 
}


istream& operator>> (istream& in, Move& m) {
  Player pl;
  Vertex v;
  if (!(in >> pl >> v)) return in;
  m = Move (pl, v);
  return in;
}
