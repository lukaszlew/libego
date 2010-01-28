//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef MOVE_H_
#define MOVE_H_

#include <string>
#include <istream>

#include "player.hpp"
#include "vertex.hpp"


class Move : public Nat <Move> {
public:

  // Constructors.

  Move () {}                           // TODO remove it
  Move (Player player, Vertex vertex); // Asserts valid player and vertex.
  static Move Null ();
  static Move OfGtpString (const std::string& s);
  static Move OfGtpStream (std::istream& s);

  // Utilities.

  Player GetPlayer ();
  Vertex GetVertex ();
  Move OtherPlayer ();
  std::string ToGtpString ();

  // Other.

  const static uint kBound = (Vertex::kBound << 1) + 1;

private:
  friend class Nat <Move>;
  explicit Move (int raw);

  const static bool kCheckAsserts = false;
};

#endif
