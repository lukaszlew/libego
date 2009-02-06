#ifndef _HASH_H_
#define _HASH_H_

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

  FastMap<Move, Hash> hashes;

  Zobrist (FastRandom& fr);

  Hash of_move (Move m) const;

  Hash of_pl_v (Player pl,  Vertex v) const;
};

#endif
