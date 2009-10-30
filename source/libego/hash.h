//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef HASH_H_
#define HASH_H_

#include "utils.h"
#include "fast_random.h"
#include "move.h"

class Hash {
public:
  
  uint64 hash;

  Hash ();
  uint index () const;
  uint lock  () const;

  void randomize (FastRandom& fr);
  void set_zero ();

  bool operator== (const Hash& other) const;
  void operator^= (const Hash& other);
};


class Zobrist {
public:

  NatMap<Move, Hash> hashes;

  Zobrist ();

  Hash of_move (Move m) const;

  Hash of_pl_v (Player pl,  Vertex v) const;
};

#endif
