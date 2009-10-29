#ifndef NAT_H_
#define NAT_H_

#include "uint.h"

// -----------------------------------------------------------------------------
// Nat<bound> class implements integers smaller than bound.

// TODO separate implementation
// TODO assert system

template <class T>
class Nat {
 public:
  // Constructors.

  static T OfRaw (uint raw) {
    return T (raw);
  }

  static T Invalid() {
    return T (-1);
  }

  Nat () : raw (-1) { // TODO remove it
  }

  // Utils.

  bool MoveNext() {
    if (raw+1 < T::kBound) {
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

  operator bool () {
    return raw != static_cast<uint> (-1);
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

#define ForEachNat(T, var) for (T var = T::Invalid(); var.MoveNext(); )

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
    ForEachNat (Nat, nat) {
      (*this)[nat] = elt;
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
