#ifndef PLAYER_H_
#define PLAYER_H_

#include "utils.h"

class Player : public Nat<2> { // TODO check is check always checked in constructors
public:
  explicit Player ();

  static Player Black ();
  static Player White ();

  const static uint black_idx = 0;
  const static uint white_idx = 1;

  Player other () const;
  
  // Black -> 1, White -> -1
  int to_score () const;

  string to_string () const;

  template <typename T> T subjective_score (const T& score) const;

  explicit Player (uint _idx); // TODO private
};


istream& operator>> (istream& in, Player& pl);
ostream& operator<< (ostream& out, Player& pl);

// faster than non-loop

#define player_for_each(pl) for (Player pl; pl.TryInc ();)

// -----------------------------------------------------------------------------
// internal

template <typename T> T Player::subjective_score (const T& score) const {
  T tab[2];
  tab[0] = score;
  tab[1] = - score;
  return tab [GetRaw()];
}

#endif
