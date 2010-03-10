//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef HASH_H_
#define HASH_H_

#include "utils.hpp"
#include "fast_random.hpp"
#include "move.hpp"
#include "color.hpp"

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

  static Hash3x3 Any () {
    return OfRaw (0xfffff);
  }

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
    raw |= mask;
  }


  void UnsetAtariBits (bool bN, bool bE, bool bS, bool bW) {
    uint mask = (bN << 16) | (bE << 17) | (bS << 18) | (bW << 19);
    raw &= ~mask;
  }


  void ResetAtariBits () {
    raw &= (1 << 16) - 1;
  }


  bool IsInAtari (Dir dir) const {
    ASSERT (dir.IsSimple4());
    //ASSERT (ColorAt(dir).IsPlayer());
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


  bool IsEyelike (Player pl) const {
    NatMap <Color, uint> color_cnt(0);
    NatMap <Color, uint> diag_color_cnt(0);
    ForEachNat (Dir, dir) {
      if (dir.IsSimple4()) {
        color_cnt [ColorAt (dir)] += 1;
      } else {
        diag_color_cnt [ColorAt (dir)] += 1;
      }
    }
    
    if (color_cnt [Color::OfPlayer(pl)] + color_cnt [Color::OffBoard()] < 4) {
      return false;
    }

    return
      diag_color_cnt [Color::OfPlayer (pl.Other())] +
      (diag_color_cnt [Color::OffBoard ()] > 0) < 2;
  }


  // bits from oldest:
  //   aW aS aE aN SW SE NE NW  W  S  E  N
  //    1  1  1  1  2  2  2  2  2  2  2  2
  // 0x      f        f     f     f     f
  // Clockwise
  Hash3x3 Rotate90 () const {
    uint raw90_colors =
      ((raw << 2) & 0x0fcfc) |
      ((raw >> 6) & 0x00303); // new N and NW

    uint raw90_ataris =
      ((raw << 1) & 0xe0000) |
      ((raw >> 3) & 0x10000); // new N and NW

    ASSERT ((raw90_colors & 0xffff)  == raw90_colors);
    ASSERT ((raw90_ataris & 0xf0000) == raw90_ataris);
    
    return OfRaw (raw90_colors | raw90_ataris);
  }


  Hash3x3 Mirror () const {
    Hash3x3 ret = *this;
    ret.SetColorAt (Dir::E(), ColorAt(Dir::W()));
    ret.SetColorAt (Dir::W(), ColorAt(Dir::E()));
    ret.SetColorAt (Dir::NE(), ColorAt(Dir::NW()));
    ret.SetColorAt (Dir::NW(), ColorAt(Dir::NE()));
    ret.SetColorAt (Dir::SE(), ColorAt(Dir::SW()));
    ret.SetColorAt (Dir::SW(), ColorAt(Dir::SE()));
    ret.ResetAtariBits();
    ret.SetAtariBits (IsInAtari(Dir::N()),
                      IsInAtari(Dir::W()),
                      IsInAtari(Dir::S()),
                      IsInAtari(Dir::E()));
    return ret;
  }


  Hash3x3 InvertColors () const {
    Hash3x3 ret = *this;
    ForEachNat (Dir, dir) {
      if (ColorAt(dir).IsPlayer()) {
        Color new_color = Color::OfPlayer (ColorAt(dir).ToPlayer().Other());
        ret.SetColorAt (dir, new_color);
      }
    }
    return ret;
  }

  void GetAll8Symmetries (Hash3x3 all[8]) const {
    Hash3x3 act = *this;
    rep (kk, 2) {
      Hash3x3 tmp = act;
      rep (ii, 4) {
        all[ii + kk*4] = act;
        act = act.Rotate90();
      }

      ASSERT (act == tmp);
      act = act.Mirror();
    }

    ASSERT (act == *this);

    uint min_ii = 0;
    rep (ii, 8) {
      if (all[ii].raw < all[min_ii].raw) min_ii = ii;
    }
    swap (all[0], all[min_ii]);
  }


  string ToString () const {
    ostringstream out;
    ForEachNat (Dir, dir) {
      out << ColorAt(dir).ToShowboardChar ();
      if (dir.IsSimple4()) {
        if (IsInAtari (dir)) out << '!';
      }
    }
    out << " " << hex << raw;
    return out.str();
  }

  string ToAsciiArt () const {
    ostringstream out;
    out
      << ColorAt(Dir::NW()).ToShowboardChar () << " "
      << ColorAt(Dir::N ()).ToShowboardChar () << (IsInAtari(Dir::N()) ? "!" : " ")
      << ColorAt(Dir::NE()).ToShowboardChar () << " "
      << endl
      << ColorAt(Dir::W ()).ToShowboardChar () << (IsInAtari(Dir::W()) ? "!" : " ")
      << "." << " "
      << ColorAt(Dir::E ()).ToShowboardChar () << (IsInAtari(Dir::E()) ? "!" : " ")
      << endl
      << ColorAt(Dir::SW()).ToShowboardChar () << " "
      << ColorAt(Dir::S ()).ToShowboardChar () << (IsInAtari(Dir::S()) ? "!" : " ")
      << ColorAt(Dir::SE()).ToShowboardChar () << " "
      << endl;

    return out.str();
  }


  static const uint kBound = 1 << 20;

private:

  friend class  Nat <Hash3x3>;
  explicit Hash3x3 (uint raw) : Nat <Hash3x3> (raw) {}
};

#endif
