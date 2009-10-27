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

#include "vertex.h"
#include "testing.h"

Coord::Coord() {
}

Coord::Coord(int idx_) {
  idx = idx_;
}

bool Coord::is_ok () const {
  return (idx < int (board_size)) & (idx >= -1); 
}

// TODO enforce invariant
void Coord::check () const { 
  assertc (coord_ac, is_ok()); 
}

bool Coord::is_on_board () const {
  return uint (idx) < board_size; 
}

// TODO to gtp string
string Coord::row_to_string () const {
  check ();
  ostringstream ss;
  ss << board_size - idx;
  return ss.str ();
}

string Coord::col_to_string () const {
  check ();
  ostringstream ss;
  ss << col_tab [idx];
  return ss.str ();
}

const string Coord::col_tab = "ABCDEFGHJKLMNOPQRSTUVWXYZ";

//--------------------------------------------------------------------------------


// TODO
// static_assert (cnt <= (1 << bits_used));
// static_assert (cnt > (1 << (bits_used-1)));


Vertex::Vertex () { 
} // TODO is it needed

Vertex::Vertex (uint _idx) {
  idx = _idx; 
}

// TODO make this constructor a static function
Vertex::Vertex (Coord row, Coord col) {
  if (vertex_ac) {
    if (row.idx != -1 || col.idx != -1) { // pass
      assertc (coord_ac, row.is_on_board ()); 
      assertc (coord_ac, col.is_on_board ()); 
    }
  }
  idx = (row.idx+1) * dNS + (col.idx+1) * dWE;
}

uint Vertex::get_idx () const {
  return idx;
}

bool Vertex::operator== (Vertex other) const {
  return idx == other.idx; 
}

bool Vertex::operator!= (Vertex other) const {
  return idx != other.idx; 
}

bool Vertex::in_range () const {
  return idx < kBound; 
}

void Vertex::next () {
  idx++; 
}

void Vertex::check () const {
  assertc (vertex_ac, in_range ()); 
}

Coord Vertex::get_row () const {
  return Coord (idx / dNS - 1); 
}

Coord Vertex::get_col () const {
  return Coord (idx % dNS - 1); 
}

// this usualy can be achieved quicker by color_at lookup
bool Vertex::is_on_board () const {
  return get_row().is_on_board () & get_col().is_on_board ();
}

void Vertex::check_is_on_board () const {
  assertc (vertex_ac, is_on_board ()); 
}

Vertex Vertex::N () const { return Vertex (idx - dNS); }
Vertex Vertex::W () const { return Vertex (idx - dWE); }
Vertex Vertex::E () const { return Vertex (idx + dWE); }
Vertex Vertex::S () const { return Vertex (idx + dNS); }

Vertex Vertex::NW () const { return N ().W (); } // TODO can it be faster?
Vertex Vertex::NE () const { return N ().E (); } // only Go
Vertex Vertex::SW () const { return S ().W (); } // only Go
Vertex Vertex::SE () const { return S ().E (); }

string Vertex::to_string () const {
  Coord r;
  Coord c;
  
  if (idx == pass_idx) {
    return "pass";
  } else if (idx == any_idx) {
    return "any";
  } else if (idx == resign_idx) {
    return "resign";
  } else {
    r = get_row ();
    c = get_col ();
    ostringstream ss;
    ss << c.col_to_string () << r.row_to_string ();
    return ss.str ();
  }
}

Vertex Vertex::pass   () { return Vertex (Vertex::pass_idx); }
Vertex Vertex::any    () { return Vertex (Vertex::any_idx); }
Vertex Vertex::resign () { return Vertex (Vertex::resign_idx); }

Vertex Vertex::of_sgf_coords (string s) {
  if (s == "") return pass ();
  if (s == "tt" && board_size <= 19) return pass ();
  if (s.size () != 2 ) return any ();
  Coord col (s[0] - 'a');
  Coord row (s[1] - 'a');
  
  if (row.is_on_board () && col.is_on_board ()) {
    return Vertex (row, col);
  } else {
    return any ();
  }
}

Vertex Vertex::of_gtp_string (string s) {
  if (s == "pass" || s == "PASS" || s == "Pass") return Vertex::pass ();
  if (s == "resign" || s == "RESIGN" || s == "Resign") return Vertex::resign ();

  istringstream in (s);
  char c;
  uint n;
  if (!(in >> c >> n)) return Vertex::any ();

  Coord row (board_size - n);
  
  Coord col (0);
  while (col.idx < int (Coord::col_tab.size ())) {
    if (Coord::col_tab[col.idx] == c || 
        Coord::col_tab[col.idx] -'A' + 'a' == c )
    {
      break;
    }
    col.idx++;
  }
  
  if (col.idx == int (Coord::col_tab.size ())) return Vertex::any ();

  return Vertex (row, col);
}


// TODO of_gtp_string
istream& operator>> (istream& in, Vertex& v) {
  string str;
  if (!(in >> str)) return in;
  v = Vertex::of_gtp_string (str);
  if (v == Vertex::any()) in.setstate (ios_base::badbit);
  return in;
}

ostream& operator<< (ostream& out, Vertex& v) { out << v.to_string (); return out; }
