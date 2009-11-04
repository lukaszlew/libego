//
// Copyright 2006 and onwards, Lukasz Lew
//

#include "move.hpp"

Move::Move (Player player, Vertex vertex)
  : Nat<Move> (player.GetRaw () | (vertex.GetRaw () << 1))
{ 
  ASSERT (player.IsValid());
  ASSERT (vertex.IsValid());
}

Move::Move (int raw) : Nat<Move> (raw) {
}

Move Move::other_player () {
  return Move::OfRaw (GetRaw() ^ 0x1);
};

Player Move::get_player () { 
  return Player::OfRaw (GetRaw() & 0x1);
}

Vertex Move::get_vertex () { 
  return Vertex::OfRaw (GetRaw() >> 1) ; 
}

string Move::to_string () {
  return
    get_player().ToGtpString() + " " +
    get_vertex().ToGtpString();
}
