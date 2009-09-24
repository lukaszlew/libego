#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "utils.h"

class Player { // TODO check is check always checked in constructors
public:

  static Player black ();
  static Player white ();

  const static uint black_idx = 0;
  const static uint white_idx = 1;
  const static uint cnt = 2;
  uint get_idx () const;

  Player other () const;
  
  // Black -> 1, White -> -1
  int to_score () const;

  string to_string () const;

  template <typename T> T subjective_score (const T& score);

  explicit Player ();
  explicit Player (uint _idx);

  bool operator== (Player other) const;
  bool operator!= (Player other) const;
  
  // TODO iterators?
  bool in_range () const; // TODO do it like check
  void next ();

private:

  void check () const;

  uint idx;
};


istream& operator>> (istream& in, Player& pl);
ostream& operator<< (ostream& out, Player& pl);

// faster than non-loop

#define player_for_each(pl) \
  for (Player pl = Player::black (); pl.in_range (); pl.next ())

// -----------------------------------------------------------------------------
// internal

template <typename T> T Player::subjective_score (const T& score) {
  T tab[2];
  tab[0] = score;
  tab[1] = - score;
  return tab[idx];
}

#endif
