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

class Player { // TODO check is check always checked in constructors

  uint idx;

public:

  const static uint black_idx = 0;
  const static uint white_idx = 1;

  const static uint cnt = 2;


  explicit Player () { idx = -1; }
  explicit Player (uint _idx) { idx = _idx; check ();}

  bool operator== (Player other) const { return idx == other.idx; }

  void check () const { 
    assertc (player_ac, (idx & (~1)) == 0);
  }

  Player other () const { 
    return Player(idx ^ 1);
  }
  
  string to_string () const {
    if (idx == black_idx)
      return "B";
    else
      return "W";
  }

  bool in_range () const { return idx < cnt; } // TODO do it like check
  void next () { idx++; }

  uint get_idx () const { return idx; }
  
  static Player black () { return Player (black_idx); }
  static Player white () { return Player (white_idx); }
};

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

// faster than non-loop
#define player_for_each(pl) \
  for (Player pl = Player::black (); pl.in_range (); pl.next ())

