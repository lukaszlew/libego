#ifndef PLAYER_H_
#define PLAYER_H_

#include "utils.h"

class Player : public Nat<2> { // TODO check is check always checked in constructors
public:
  explicit Player ();

  static Player Black ();
  static Player White ();

  Player Other() const;
  
  // Black -> 1, White -> -1
  int ToScore () const;

  string ToGtpString () const;

  template <typename T>
  T SubjectiveScore (const T& score) const;

  const static uint black_idx = 0;
  const static uint white_idx = 1;

  explicit Player (uint _idx); // TODO private
};


istream& operator>> (istream& in, Player& pl);
ostream& operator<< (ostream& out, Player& pl);

// faster than non-loop

#define player_for_each(pl) for (Player pl; pl.TryInc ();)

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
