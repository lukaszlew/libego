//
// Copyright 2006 and onwards, Lukasz Lew
//

#include "player.hpp"

Player::Player (uint raw) : Nat<Player> (raw) { 
}

Player Player::Black () { 
  return Player (0); 
}

Player Player::White () { 
  return Player (1); 
}

Player Player::OfGtpStream (istream& in) {
  string s;
  in >> s;
  if (!in) return Invalid ();
  if (s == "b" || s == "B" || s == "Black" || s == "BLACK "|| s == "black") { 
    return Black();
  }
  if (s == "w" || s == "W" || s == "White" || s == "WHITE "|| s == "white") {
    return White();
  }
  in.setstate (ios_base::badbit); // TODO undo read?
  return Invalid();
}

Player Player::WinnerOfBoardScore (int score) {
  return Player::OfRaw (score <= 0); 
}

Player Player::Other() const { 
  return Player(GetRaw() ^ 1);
}
  
int Player::ToScore () const {
  return 1 - int(GetRaw() + GetRaw()) ;
}

string Player::ToGtpString () const {
  NatMap <Player, string> gtp_string ("");
  gtp_string [Black()] = "B";
  gtp_string [White()] = "W";
  return gtp_string [*this];
}
