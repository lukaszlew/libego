//
// Copyright 2006 and onwards, Lukasz Lew
//

#include "move.hpp"

void Move::check () {
  Player::OfRaw (idx & 0x1);
  // TODO
  //Vertex::OfRaw (idx & ((1 << Vertex::bits_used) - 1)).check();
}

Move::Move (Player player, Vertex v) { 
  idx = player.GetRaw () | (v.GetRaw () << 1);
}

Move::Move () : idx(-1) {
}

Move::Move (int idx_) {
  idx = idx_;
}

Move Move::other_player () {
  return Move (idx ^ 0x1);
};

Player Move::get_player () { 
  return Player::OfRaw (idx & 0x1);
}

Vertex Move::get_vertex () { 
  return Vertex::OfRaw (idx >> 1) ; 
}

string Move::to_string () {
  return
    get_player().ToGtpString() + " " +
    get_vertex().ToGtpString();
}

uint Move::GetRaw () { return idx; }

bool Move::operator== (Move other) const { return idx == other.idx; }
bool Move::operator!= (Move other) const { return idx != other.idx; }

bool Move::in_range () const {
  return idx < kBound;
}

void Move::next () {
  idx++; 
}

