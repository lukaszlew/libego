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

Move Move::Null () {
  return OfRaw (kBound - 1);
}

Move Move::OtherPlayer () {
  return Move::OfRaw (GetRaw() ^ 0x1);
};

Player Move::GetPlayer () { 
  ASSERT (*this != Null());
  return Player::OfRaw (GetRaw() & 0x1);
}

Vertex Move::GetVertex () { 
  ASSERT (*this != Null());
  return Vertex::OfRaw (GetRaw() >> 1) ; 
}

string Move::ToGtpString () {
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
