//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef HASH_H_
#define HASH_H_

#include "utils.hpp"
#include "fast_random.hpp"
#include "move.hpp"

class Hash {
public:
  Hash ();
  uint Index () const;
  uint Lock  () const;

  void Randomize (FastRandom& fr);
  void SetZero();

  bool operator== (const Hash& other) const;
  void operator^= (const Hash& other);
  void operator+= (const Hash& other);
  void operator-= (const Hash& other);

private:  
  uint64 hash;
};

// -----------------------------------------------------------------------------

class Zobrist {
public:
  Zobrist();
  Hash OfMove (Move m) const;
  Hash OfPlayerVertex (Player pl,  Vertex v) const;

private:
  NatMap<Move, Hash> hashes;
};

#endif
