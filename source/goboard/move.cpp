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

Move Move::OtherPlayer () const {
  return Move::OfRaw (GetRaw() ^ 0x1);
};

Player Move::GetPlayer () const {
  return Player::OfRaw (GetRaw() & 0x1);
}

Vertex Move::GetVertex () const { 
  return Vertex::OfRaw (GetRaw() >> 1) ; 
}

string Move::ToGtpString () const {
  return
    GetPlayer().ToGtpString() + " " +
    GetVertex().ToGtpString();
}

Move Move::OfGtpString (const std::string& s) {
  stringstream ss (s);
  return OfGtpStream (ss);
}

Move Move::OfGtpStream (istream& in) {
  Player pl = Player::OfGtpStream (in);
  Vertex v  = Vertex::OfGtpStream (in);
  if (!in || pl == Player::Invalid() || v == Vertex::Invalid()) {
    in.setstate (ios_base::badbit); // TODO undo read?
    return Invalid();
  }
  return Move (pl, v);
}
