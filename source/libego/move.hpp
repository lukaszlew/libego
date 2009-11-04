//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef MOVE_H_
#define MOVE_H_

#include <string>

#include "player.hpp"
#include "vertex.hpp"

class Move {
public:
  explicit Move (Player player, Vertex v);
  explicit Move ();

  static Move Invalid() { return Move (-1); } // TODO replace by Nat

  Player get_player ();
  Vertex get_vertex ();

  Move other_player ();

  string to_string ();

  bool operator!= (Move other) const;
  bool operator== (Move other) const;

  const static uint kBound = Vertex::kBound << 1;

  uint GetRaw ();

  bool MoveNext() { // TODO Nat
    if (idx+1 < kBound) {
      idx += 1;
      return true;
    }
    return false;
  }

private:
  explicit Move (int idx_);
  void check ();

  uint idx;
};

#endif
