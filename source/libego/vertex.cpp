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
  const static uint dNS = (board_size + 2);
  const static uint dWE = 1;
}

bool CoordIsOnBoard (int idx) {
  return uint (idx) < board_size; 
}

// TODO to gtp string
string CoordRowToString (int idx) {
  return ToString (board_size - idx);
}

string CoordColToString (int idx) {
  return ToString (col_tab [idx]);
}



// TODO
// static_assert (cnt <= (1 << bits_used));
// static_assert (cnt > (1 << (bits_used-1)));
//--------------------------------------------------------------------------------

Vertex::Vertex () : Nat <kBoardAreaWithGuards> () { 
} // TODO is it needed

Vertex::Vertex (uint raw) : Nat <kBoardAreaWithGuards> (raw) {
}

Vertex Vertex::Pass() {
  return Vertex (0); // TODO change it
}

Vertex Vertex::Any() {
  return Vertex (1); // TODO change it 
}

Vertex Vertex::Resign() {
  return Vertex (2); // TODO change it
}

Vertex Vertex::OfRaw (uint raw) {
  return Vertex (raw);
}

Vertex Vertex::OfSgfString (const string& s) {
  if (s == "") return Pass(); // TODO pass ?
  if (s == "tt" && board_size <= 19) return Pass(); // TODO comment
  if (s.size() != 2) return Any();
  int col = s[0] - 'a';
  int row = s[1] - 'a';
  
  if (CoordIsOnBoard (row) && CoordIsOnBoard (col)) { // TODO move this ...
    return Vertex::OfCoords (row, col); // ... to this
  } else {
    return Any ();
  }
}

Vertex Vertex::OfGtpString (const string& s) {
  if (s == "pass" || s == "PASS" || s == "Pass") return Pass();
  if (s == "resign" || s == "RESIGN" || s == "Resign") return Resign();

  istringstream in (s);
  char c;
  uint n;
  if (!(in >> c >> n)) return Any();

  int row = board_size - n;
  
  int col = 0;
  while (col < int (col_tab.size ())) {
    if (col_tab [col] == c || col_tab [col] -'A' + 'a' == c) break;
    col++;
  }

  if (col == int (col_tab.size ())) return Any();
  return Vertex::OfCoords (row, col);
}

Vertex Vertex::OfCoords (int row, int column) {
  // TODO
  //   if (vertex_ac) {
  //     if (row != -1 || col != -1) { // pass
  //       assertc (coord_ac, CoordIsOnBoard (row)); 
  //       assertc (coord_ac, CoordIsOnBoard (col)); 
  //     }
  //   }
  return Vertex::OfRaw ((row+1) * dNS + (column+1) * dWE);
}

int Vertex::GetRow() const {
  return int (GetRaw() / dNS - 1); 
}

int Vertex::GetColumn() const {
  return int (GetRaw() % dNS - 1); 
}

bool Vertex::IsOnBoard() const {
  return CoordIsOnBoard (GetRow()) & CoordIsOnBoard (GetColumn());
}

Vertex Vertex::N() const { return Vertex::OfRaw (GetRaw() - dNS); }
Vertex Vertex::W() const { return Vertex::OfRaw (GetRaw() - dWE); }
Vertex Vertex::E() const { return Vertex::OfRaw (GetRaw() + dWE); }
Vertex Vertex::S() const { return Vertex::OfRaw (GetRaw() + dNS); }

Vertex Vertex::NW() const { return N().W(); }
Vertex Vertex::NE() const { return N().E(); }
Vertex Vertex::SW() const { return S().W(); }
Vertex Vertex::SE() const { return S().E(); }

string Vertex::ToGtpString() const {
  int r;
  int c;
  
  if (*this == Pass())   return "pass";
  if (*this == Any())    return "any";
  if (*this == Resign()) return "resign";

  r = GetRow ();
  c = GetColumn ();
  ostringstream ss;
  ss << CoordColToString (c) << CoordRowToString (r);
  return ss.str ();
}


istream& operator>> (istream& in, Vertex& v) {
  string str;
  if (!(in >> str)) return in;
  v = Vertex::OfGtpString (str);
  if (v == Vertex::Any()) in.setstate (ios_base::badbit);
  return in;
}
