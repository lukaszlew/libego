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

#include "hash.h"

Hash::Hash () { }

uint Hash::index () const { return hash; }
uint Hash::lock  () const { return hash >> 32; }

void Hash::randomize (FastRandom& fr) { 
  hash =
    (uint64 (fr.rand_int ()) << (0*16)) ^
    (uint64 (fr.rand_int ()) << (1*16)) ^ 
    (uint64 (fr.rand_int ()) << (2*16)) ^ 
    (uint64 (fr.rand_int ()) << (3*16));
}

void Hash::set_zero () { hash = 0; }

bool Hash::operator== (const Hash& other) const { return hash == other.hash; }
void Hash::operator^= (const Hash& other) { hash ^= other.hash; }

// -----------------------------------------------------------------------------

Zobrist::Zobrist () {
  FastRandom fr (123);
  player_for_each (pl) vertex_for_each_all (v) {
    Move m = Move (pl, v);
    hashes [m].randomize (fr);
  }
}

Hash Zobrist::of_move (Move m) const {
  return hashes [m];
}

Hash Zobrist::of_pl_v (Player pl,  Vertex v) const {
  return hashes [Move (pl, v)];
}
