#ifndef PLAYER_H_
#define PLAYER_H_

#include "utils.h"

class Player : public Nat<2> { // TODO check is check always checked in constructors
public:
  // constructors

  explicit Player ();

  static Player Black ();
  static Player White ();
  static Player OfGtpString (std::string s);

  // utilities

  Player Other() const;
  
  int ToScore () const;   // ToScore (Black()) == 1, ToScore (White()) == -1

  string ToGtpString () const;

  template <typename T>
  T SubjectiveScore (const T& score) const;

  // TODO

  explicit Player (uint _idx); // TODO private
};


istream& operator>> (istream& in, Player& pl);
ostream& operator<< (ostream& out, Player& pl);

// faster than non-loop


// -----------------------------------------------------------------------------
// internal

template <typename T>
T Player::SubjectiveScore (const T& score) const {
  T tab[2];
  tab[0] = score;
  tab[1] = - score;
  return tab [GetRaw()];
}

#endif
