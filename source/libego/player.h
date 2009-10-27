//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef PLAYER_H_
#define PLAYER_H_

class Player : public Nat<2> { // TODO check is check always checked in constructors
public:

  // Constructors.

  explicit Player ();
  static Player Black ();
  static Player White ();
  static Player OfRaw (uint raw);
  static Player OfGtpString (const std::string& s);

  // Utilities.

  Player Other() const;
  int ToScore () const;   // ToScore (Black()) == 1, ToScore (White()) == -1
  string ToGtpString () const;

  template <typename T>
  T SubjectiveScore (const T& score) const;

 private:
  explicit Player (uint raw);
};

// TODO move this to GTP implementation.
istream& operator>> (istream& in, Player& pl);

// TODO move this to player-impl.h
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
