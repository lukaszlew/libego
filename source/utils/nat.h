#ifndef NAT_H_
#define NAT_H_

#include "uint.h"

// -----------------------------------------------------------------------------
// Nat<bound> class implements integers smaller than bound.

template <uint bound>
class Nat {
 public:
  static const uint kBound = bound;

  Nat () : raw (-1) {
  }

  bool TryInc () {
    if (raw+1 < kBound) {
      raw += 1;
      return true;
    }
    return false;
  }

  uint GetRaw() const {
    return raw;
  }

  void SetRaw (uint raw) {
    this->raw = raw;
  }

  static Nat OfRaw (uint raw) {
    return Nat (raw);
  }

  bool operator == (const Nat& other) const {
    return this->raw == other.raw;
  }

  bool operator != (const Nat& other) const {
    return this->raw != other.raw;
  }

 protected:
  explicit Nat (uint raw) : raw (raw) {
  }

 private:
  uint raw;
};

// -----------------------------------------------------------------------------
// NatMap<Nat, Elt> is an array that can be indexed by Nat

template <typename Nat, typename Elt>
class NatMap {
 public:
  NatMap() {
  }

  Elt& operator[] (Nat nat) { 
    return tab [nat.GetRaw()];
  }

  const Elt& operator[] (Nat nat) const {
    return tab [nat.GetRaw()];
  }

  void SetAll (const Elt& elt) {
    for (Nat e; e.TryNext();) {
      (*this)[e] = elt;
    }
  }

  void SetToZero () { 
    memset (tab, '\0', sizeof (tab)); 
  }

 private:
  Elt tab [Nat::kBound];
};

// -----------------------------------------------------------------------------

#endif // NAT_H_
