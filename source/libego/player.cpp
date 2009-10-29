//
// Copyright 2006 and onwards, Lukasz Lew
//

#include "player.h"

Player::Player (uint raw) : Nat<Player> (raw) { 
}

Player Player::Black () { 
  return Player (0); 
}

Player Player::White () { 
  return Player (1); 
}

Player Player::OfGtpString (const string& s) {
  if (s == "b" || s == "B" || s == "Black" || s == "BLACK "|| s == "black") { 
    return Player::Black();
  }
  if (s == "w" || s == "W" || s == "White" || s == "WHITE "|| s == "white") {
    return Player::White();
  }
  return Player::Invalid();
}


Player Player::Other() const { 
  return Player(GetRaw() ^ 1);
}
  
int Player::ToScore () const {
  return 1 - int(GetRaw() + GetRaw()) ;
}

string Player::ToGtpString () const {
  NatMap <Player, string> gtp_string;
  gtp_string [Black()] = "B";
  gtp_string [White()] = "W";
  return gtp_string [*this];
}

// TODO move this to GTP implementation
istream& operator>> (istream& in, Player& pl) {
  string s;
  in >> s;
  if (!in) return in;
  pl = Player::OfGtpString (s);
  if (!pl) {
    in.setstate (ios_base::badbit);
  }
  return in;
}
