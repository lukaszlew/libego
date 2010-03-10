#ifndef DIR_H_
#define DIR_H_

#include "nat.hpp"

class Dir : public Nat <Dir> {
public:
  explicit Dir () : Nat<Dir> () {};

  static Dir  N () { return OfRaw (0); }
  static Dir  E () { return OfRaw (1); }
  static Dir  S () { return OfRaw (2); }
  static Dir  W () { return OfRaw (3); }
  static Dir NW () { return OfRaw (4); }
  static Dir NE () { return OfRaw (5); }
  static Dir SE () { return OfRaw (6); }
  static Dir SW () { return OfRaw (7); }

  bool IsSimple4 () const { return GetRaw() < 4; }

  Dir Opposite () {
    return OfRaw (((raw + 2) & 3) | (raw & 4));
  }

  static const uint kBound = 8;

private:
  friend class Nat <Dir>;
  explicit Dir (uint raw) : Nat<Dir> (raw) {};
};

#endif
