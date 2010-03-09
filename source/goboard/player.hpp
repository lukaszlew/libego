//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef PLAYER_H_
#define PLAYER_H_

#include <istream>
#include <string>
#include "nat.hpp"

class Player : public Nat <Player> {
public:

  // Constructors.
  Player ();
  static Player Black ();
  static Player White ();
  static Player None ();
  static Player WinnerOfBoardScore (int score);
  static Player OfGtpStream (std::istream& s);

  // Utilities.

  Player Other() const;
  int ToScore () const;   // ToScore (Black()) == 1, ToScore (White()) == -1
  std::string ToGtpString () const;

  float SubjectiveScore (const float& score) const;

  // Other.

  static const uint kBound = 2;

 private:
  friend class Nat <Player>;
  explicit Player (uint raw);
};

#endif
