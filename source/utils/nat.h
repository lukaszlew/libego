//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef NAT_H_
#define NAT_H_

#include "uint.h"

// -----------------------------------------------------------------------------
// For a use case look in player.h

template <class T>
class Nat {
 public:
  // Constructors.

  Nat ();  // TODO replace default constructor with Invalid.
  static T OfRaw (uint raw);
  static T Invalid();

  // Utils.

  bool MoveNext();
  uint GetRaw() const;
  bool IsValid() const;
  bool operator == (const Nat& other) const;
  bool operator != (const Nat& other) const;

 protected:
  explicit Nat (uint raw);

 private:
  uint raw;
};

#define ForEachNat(T, var) for (T var = T::Invalid(); var.MoveNext(); )

// -----------------------------------------------------------------------------
// NatMap<Nat, Elt> is an array that can be indexed by Nat

template <typename Nat, typename Elt>
class NatMap {
 public:
  NatMap (const Elt& init);
  Elt& operator[] (Nat nat);
  const Elt& operator[] (Nat nat) const;
  void SetAll (const Elt& elt);
  void SetToZero ();

 private:
  Elt tab [Nat::kBound];
};

// -----------------------------------------------------------------------------

#include "nat-inl.h"

#endif // NAT_H_
