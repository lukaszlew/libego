//
// Copyright 2006 and onwards, Lukasz Lew
//

#include "vertex.h"

#include "testing.h"

namespace {
  const string col_tab = "ABCDEFGHJKLMNOPQRSTUVWXYZ";
  const static uint dNS = (board_size + 2);
  const static uint dWE = 1;

  bool CoordIsOnBoard (int coord) {
    return static_cast <uint> (coord) < board_size; 
  }

}

string CoordRowToString (int idx) {
  return ToString (board_size - idx);
}

string CoordColToString (int idx) {
  return ToString (col_tab [idx]);
}



//--------------------------------------------------------------------------------

Vertex::Vertex (uint raw) : Nat <Vertex> (raw) {
}

Vertex Vertex::Pass() {
  return Vertex (0); // TODO change it
}

Vertex Vertex::OfCoords (int row, int column) {
  if (!CoordIsOnBoard (row) || !CoordIsOnBoard (column)) {
    return Vertex::Invalid();
  }
  return Vertex::OfRaw ((row+1) * dNS + (column+1) * dWE);
}

Vertex Vertex::OfSgfString (const string& s) {
  if (s == "") return Pass(); // TODO pass ?
  if (s == "tt" && board_size <= 19) return Pass(); // TODO comment
  if (s.size() != 2) return Invalid();
  int col = s[0] - 'a';
  int row = s[1] - 'a';
  
  if (CoordIsOnBoard (row) && CoordIsOnBoard (col)) { // TODO move this ...
    return Vertex::OfCoords (row, col); // ... to this
  } else {
    return Invalid();
  }
}

Vertex Vertex::OfGtpStream (istream& in) {
  string s;
  in >> s;
  if (!in) return Invalid ();

  if (s == "pass" || s == "PASS" || s == "Pass") return Pass();

  istringstream vin (s);
  char c;
  uint n;
  if (!(vin >> c >> n)) {
    in.setstate (ios_base::badbit); // TODO undo read + error handling based on invalid
    return Invalid();
  }

  int row = board_size - n;
  
  int col = 0;
  while (col < int (col_tab.size ())) {
    if (col_tab [col] == c || col_tab [col] -'A' + 'a' == c) break;
    col++;
  }

  if (col == int (col_tab.size ())) {
    in.setstate (ios_base::badbit);
    return Invalid();
  }
  return Vertex::OfCoords (row, col);
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
  
  if (*this == Invalid()) return "invalid";
  if (*this == Pass())    return "pass";

  r = GetRow ();
  c = GetColumn ();
  ostringstream ss;
  ss << CoordColToString (c) << CoordRowToString (r);
  return ss.str ();
}
