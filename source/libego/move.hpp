//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef MOVE_H_
#define MOVE_H_

#include <string>

#include "player.hpp"
#include "vertex.hpp"

class Move : public Nat <Move> {
public:

  // Constructors.

  Move () {}                           // TODO remove it
  Move (Player player, Vertex vertex); // Asserts valid player and vertex.

  // Utilities.

  Player get_player ();
  Vertex get_vertex ();

  Move other_player ();

  string to_string ();

  // Other.

  const static uint kBound = Vertex::kBound << 1;

private:
  friend class Nat <Move>;
  explicit Move (int raw);

  const static bool kCheckAsserts = false;
};

#endif
