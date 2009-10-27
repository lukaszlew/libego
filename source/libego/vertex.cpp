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

namespace {
  const string col_tab = "ABCDEFGHJKLMNOPQRSTUVWXYZ";
  const static uint pass_idx = 0; // TODO make it big
  const static uint any_idx  = 1; // TODO any
  const static uint resign_idx = 2;
  const static uint dNS = (board_size + 2);
  const static uint dWE = 1;
}

bool CoordIsOk (int idx) {
  return (idx < int (board_size)) & (idx >= -1); 
}

bool CoordIsOnBoard (int idx) {
  return uint (idx) < board_size; 
}

// TODO to gtp string
string CoordRowToString (int idx) {
  ostringstream ss;
  ss << board_size - idx;
  return ss.str ();
}

string CoordColToString (int idx) {
  ostringstream ss;
  ss << col_tab [idx];
  return ss.str ();
}


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
Vertex::Vertex (int row, int col) {
  if (vertex_ac) {
    if (row != -1 || col != -1) { // pass
      assertc (coord_ac, CoordIsOnBoard (row)); 
      assertc (coord_ac, CoordIsOnBoard (col)); 
    }
  }
  idx = (row+1) * dNS + (col+1) * dWE;
}

uint Vertex::GetRaw () const {
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

int Vertex::get_row () const {
  return int (idx / dNS - 1); 
}

int Vertex::get_col () const {
  return int (idx % dNS - 1); 
}

// this usualy can be achieved quicker by color_at lookup
bool Vertex::is_on_board () const {
  return CoordIsOnBoard (get_row()) & CoordIsOnBoard (get_col());
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
  int r;
  int c;
  
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
    ss << CoordColToString (c) << CoordRowToString (r);
    return ss.str ();
  }
}

Vertex Vertex::pass   () { return Vertex (pass_idx); }
Vertex Vertex::any    () { return Vertex (any_idx); }
Vertex Vertex::resign () { return Vertex (resign_idx); }

Vertex Vertex::of_sgf_coords (string s) {
  if (s == "") return pass ();
  if (s == "tt" && board_size <= 19) return pass ();
  if (s.size () != 2 ) return any ();
  int col (s[0] - 'a');
  int row (s[1] - 'a');
  
  if (CoordIsOnBoard (row) && CoordIsOnBoard (col)) {
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

  int row (board_size - n);
  
  int col (0);
  while (col < int (col_tab.size ())) {
    if (col_tab[col] == c || 
        col_tab[col] -'A' + 'a' == c )
    {
      break;
    }
    col++;
  }
  
  if (col == int (col_tab.size ())) return Vertex::any ();

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
