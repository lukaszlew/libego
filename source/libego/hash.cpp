//
// Copyright 2006 and onwards, Lukasz Lew
//

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
  ForEachNat (Player, pl) {
    ForEachNat (Vertex, v) {
      Move m = Move (pl, v);
      hashes [m].randomize (fr);
    }
  }
}

Hash Zobrist::of_move (Move m) const {
  return hashes [m];
}

Hash Zobrist::of_pl_v (Player pl,  Vertex v) const {
  return hashes [Move (pl, v)];
}
