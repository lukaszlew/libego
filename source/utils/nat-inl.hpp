//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef NAT_INL_H_
#define NAT_INL_H_

#include "test.hpp"
#include <cmath>
#include <iostream>


template <class T>
T Nat<T>::OfRaw (uint raw) {
  ASSERT (raw < T::kBound);
  return T (raw);
}

template <class T>
T Nat<T>::Invalid() {
  return T (-1);
}

template <class T>
Nat<T>::Nat () : raw (-1) {
}

// Utils.

template <class T>
bool Nat<T>::MoveNext() {
  if (raw+1 == T::kBound) {
    return false;
  }
  raw += 1;
  return true;
}

template <class T>
uint Nat<T>::GetRaw() const {
  return raw;
}

template <class T>
bool Nat<T>::IsValid() const {
  return *this != Invalid();
}

template <class T>
bool Nat<T>::operator == (const Nat& other) const {
  return this->raw == other.raw;
}

template <class T>
bool Nat<T>::operator != (const Nat& other) const {
  return this->raw != other.raw;
}

template <class T>
Nat<T>::Nat (uint raw) : raw (raw) {
  ASSERT (raw < T::kBound || raw == static_cast <uint> (-1));
}

// -----------------------------------------------------------------------------

template <typename Nat, typename Elt>
NatMap <Nat, Elt>::NatMap () {
}

template <typename Nat, typename Elt>
NatMap <Nat, Elt>::NatMap (const Elt& init) {
  SetAll (init);
}

template <typename Nat, typename Elt>
Elt& NatMap <Nat, Elt>::operator [] (Nat nat) { 
  ASSERT (nat != Nat::Invalid());
  return tab [nat.GetRaw()];
}

template <typename Nat, typename Elt>
const Elt& NatMap <Nat, Elt>::operator [] (Nat nat) const {
  ASSERT (nat != Nat::Invalid());
  return tab [nat.GetRaw()];
}

template <typename Nat, typename Elt>
void NatMap <Nat, Elt>::SetAll (const Elt& elt) {
  ForEachNat (Nat, nat) {
    (*this)[nat] = elt;
  }
}

template <typename Nat, typename Elt>
void NatMap <Nat, Elt>::SetAllToZero () { 
  memset (tab, '\0', sizeof (tab)); 
}

template <typename Nat, typename Elt>
Elt NatMap <Nat, Elt>::Min () const {
  Elt min_all = 1E20;
  ForEachNat (Nat, nat) {
    Elt val = (*this) [nat];
    if (isnan (val)) continue;
    min_all = min (min_all, val);
  }
  return min_all;
}

template <typename Nat, typename Elt>
Elt NatMap <Nat, Elt>::Max () const {
  Elt max_all = -1E20;
  ForEachNat (Nat, nat) {
    Elt val = (*this) [nat];
    if (isnan (val)) continue;
    max_all = max (max_all, val);
  }
  return max_all;
}

template <typename Nat, typename Elt>
Elt NatMap <Nat, Elt>::Mean () const {
  double n = 0.0;
  Elt sum = 0.0;
  ForEachNat (Nat, nat) {
    Elt val = (*this) [nat];
    if (std::isnan (val)) continue;
    sum += val;
    n += 1;
  }
  if (n == 0.0) return nan("");
  return sum / n;
}


template <typename Nat, typename Elt>
Elt NatMap <Nat, Elt>::StdDev () const {
  double n = 0.0;
  Elt sum2 = 0.0;
  ForEachNat (Nat, nat) {
    Elt val = (*this) [nat];
    if (std::isnan (val)) continue;
    sum2 += val * val;
    n += 1;
  }
  if (n == 0.0) return nan("");
  double mean = Mean ();
  return sqrt (sum2 / n - mean * mean);
}

template <typename Nat, typename Elt>
void NatMap <Nat, Elt>::
operator += (Elt val) { ForEachNat (Nat, nat) (*this) [nat] += val; }

template <typename Nat, typename Elt>
void NatMap <Nat, Elt>::
operator -= (Elt val) { ForEachNat (Nat, nat) (*this) [nat] -= val; }

template <typename Nat, typename Elt>
void NatMap <Nat, Elt>::
operator *= (Elt val) { ForEachNat (Nat, nat) (*this) [nat] *= val; }

template <typename Nat, typename Elt>
void NatMap <Nat, Elt>::
operator /= (Elt val) { ForEachNat (Nat, nat) (*this) [nat] /= val; }

template <typename Nat, typename Elt>
void NatMap <Nat, Elt>::LogAll (double base) { 
  double base_change = log (base);
  ForEachNat (Nat, nat) {
    (*this) [nat] = log ((*this) [nat]) / base_change;
  }
}


template <typename Nat, typename Elt>
void NatMap <Nat, Elt>::Scale (Elt min_val, Elt max_val) {
  Elt min_all = Min ();
  Elt max_all = Max ();

  std::cerr << "Scale: " << min_all << " .. " << max_all << endl;

  ForEachNat (Nat, nat) {
    Elt& elt = (*this) [nat];
    elt = (elt - min_all) / (max_all - min_all);
    elt = elt * (max_val - min_val) + min_val;
    (*this) [nat] = elt;    
  }
}

template <typename Nat, typename Elt>
void NatMap <Nat, Elt>::Dump () { 
  ForEachNat (Nat, v) {
    if (v.IsOnBoard() || v == Nat::Pass ()) {
      cerr << v.ToGtpString() << " " << (*this) [v] << endl;
    }
  }
}


template <typename Nat, typename Elt>
void NatMap <Nat, Elt>::Load (const NatMap& other) {
  memcpy(this, &other, sizeof(NatMap));
}

#endif
