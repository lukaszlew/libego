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


// -----------------------------------------------------------------------------


// perfect 20 bit hash (bitmask)
// bit mask from least significant
// N, E, S, W, NW, NE, SE, SW, aN, aE, aS, sW
// 2  2  2  2   2   2   2   2   1   1   1   1

class Hash3x3 : public Nat <Hash3x3> {
public:
  explicit Hash3x3 () : Nat <Hash3x3>() {};

  
  // ataris have to be marked manually
  static Hash3x3 OfBoard (const NatMap <Vertex, Color>& color_at, Vertex v) {
    if (!v.IsOnBoard()) return OfRaw (0);
    uint raw = 0;
    ForEachNat (Dir, dir) {
      raw |= color_at [v.Nbr(dir)].GetRaw() << (2*dir.GetRaw());
    }

    return OfRaw (raw);
  }

  Color ColorAt (Dir dir) const {
    return Color::OfRaw ((GetRaw() >> (2*dir.GetRaw())) & 3);
  }

  void SetColorAt (Dir dir, Color color) {
    raw &= ~(3 << (2*dir.GetRaw()));
    raw |= color.GetRaw() << (2*dir.GetRaw());
  }

  void SetAtariBits (bool bN, bool bE, bool bS, bool bW) {
    uint mask = (bN << 16) | (bE << 17) | (bS << 18) | (bW << 19);
    ASSERT ((raw & mask) == 0);
    raw |= mask;
  }

  void UnsetAtariBits (bool bN, bool bE, bool bS, bool bW) {
    uint mask = (bN << 16) | (bE << 17) | (bS << 18) | (bW << 19);
    ASSERT ((raw & mask) == mask);
    raw &= ~mask;
  }

  void ResetAtariBits () {
    raw &= (1 << 16) - 1;
  }

  bool IsInAtari (Dir dir) const {
    ASSERT (dir.IsSimple4());
    ASSERT (ColorAt(dir).IsPlayer());
    return (raw >> (16 + dir.GetRaw())) & 1;
  }

  bool IsLegal (Player pl) {
    NatMap <Color, uint> color_cnt(0);
    NatMap <Player, uint> atari_cnt(0);

    ForEachNat (Dir, dir) {
      if (!dir.IsSimple4()) continue;
      Color c = ColorAt (dir);
      color_cnt [c] += 1;
      if (c.IsPlayer() && IsInAtari(dir)) atari_cnt [c.ToPlayer()] += 1;
    }

    if (color_cnt [Color::Empty()] > 0) return true;
    if (atari_cnt [pl.Other()] > 0) return true;
    if (atari_cnt [pl] < color_cnt [Color::OfPlayer(pl)]) return true;
    return false;
  }

  string ToString () const {
    ostringstream out;
    ForEachNat (Dir, dir) {
      out << ColorAt(dir).ToShowboardChar ();
      if (dir.IsSimple4()) {
        if (IsInAtari (dir)) out << '!';
      }
    }
    out << hex << raw;
    return out.str();
  }

  // TODO atari stuff

  static const uint kBound = 1 << 20;
private:

  friend class  Nat <Hash3x3>;
  explicit Hash3x3 (uint raw) : Nat <Hash3x3> (raw) {}
  const static bool kCheckAsserts = false;
};

#endif
