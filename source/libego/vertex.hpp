//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef VERTEX_H_
#define VERTEX_H_

#include "config.hpp"

namespace Coord {
  bool IsOk (int coord);

  string RowToGtpString (int row);
  string ColumnToGtpString (int column);
  int RowOfGtpInt (int r);
  int ColumnOfGtpChar (char c);
}

class Vertex : public Nat <Vertex> {
public:

  // Constructors.

  explicit Vertex() {}; // TODO remove it

  static Vertex Pass();

  static Vertex OfCoords (int row, int column);
  static Vertex OfGtpString (const string& s);
  static Vertex OfGtpStream (istream& in);
  static Vertex OfSgfString (const string& s);

  // Utilities.

  int GetRow() const;
  int GetColumn() const;

  // This can be achieved quicker by color_at lookup.
  bool IsOnBoard() const;

  Vertex N() const;
  Vertex W() const;
  Vertex E() const;
  Vertex S() const;

  Vertex NW() const;
  Vertex NE() const;
  Vertex SW() const;
  Vertex SE() const;

  string ToGtpString() const;

  // Other.

  static const uint kBound = (board_size + 2) * (board_size + 2);

private:
  friend class Nat <Vertex>;
  explicit Vertex (uint raw);
};

#endif
