/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                           *
 *  This file is part of Library of Effective GO routines - EGO library      *
 *                                                                           *
 *  Copyright 2006 and onwards, Lukasz Lew                                   *
 *                                                                           *
 *  EGO library is free software; you can redistribute it and/or modify      *
 *  it under the terms of the GNU General Public License as published by     *
 *  the Free Software Foundation; either version 2 of the License, or        *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  EGO library is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public License        *
 *  along with EGO library; if not, write to the Free Software               *
 *  Foundation, Inc., 51 Franklin St, Fifth Floor,                           *
 *  Boston, MA  02110-1301  USA                                              *
 *                                                                           *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "player.h"
#include "testing.h"

Player::Player () { 
  idx = uint(-1);  
}

Player::Player (uint _idx) { 
  idx = _idx;
  check ();
}

bool Player::operator== (Player other) const { 
  return idx == other.idx; 
}

bool Player::operator!= (Player other) const { 
  return idx != other.idx; 
}

void Player::check () const { 
  assertc (player_ac, (idx & (~1)) == 0);
}

Player Player::other () const { 
  return Player(idx ^ 1);
}
  
int Player::to_score () const {
  return 1 - int(idx + idx) ;
}

string Player::to_string () const {
  if (idx == black_idx)
    return "B";
  else
    return "W";
}

bool Player::in_range () const {
  return idx < cnt; 
}

void Player::next () {
  idx++; 
}

uint Player::get_idx () const {
  return idx; 
}
  
Player Player::black () { 
  return Player (black_idx); 
}

Player Player::white () { 
  return Player (white_idx); 
}


// TODO factorize of_string out
istream& operator>> (istream& in, Player& pl) {
  string s;
  in >> s;
  if (s == "b" || s == "B" ||
      s == "Black" || s == "BLACK "|| s == "black" || s == "#") { 
    pl = Player::black (); 
    return in; 
  }
  if (s == "w" || s == "W" || 
      s == "White" || s == "WHITE "|| s == "white" || s == "O") {
    pl = Player::white ();
    return in; 
  }
  in.setstate (ios_base::badbit);
  return in;
}

ostream& operator<< (ostream& out, Player& pl) { 
  out << pl.to_string ();
  return out; 
}
