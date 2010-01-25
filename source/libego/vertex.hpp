//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef VERTEX_H_
#define VERTEX_H_

#include <istream>
#include <string>
#include "config.hpp"

namespace Coord {
  bool IsOk (int coord);

  std::string RowToGtpString (int row);
  std::string ColumnToGtpString (int column);
  int RowOfGtpInt (int r);
  int ColumnOfGtpChar (char c);
}

class Vertex : public Nat <Vertex> {
public:

  // Constructors.

  explicit Vertex() {}; // TODO remove it

  static Vertex Pass();
  static Vertex Any(); // NatMap doesn't work with Invalid

  static Vertex OfCoords (int row, int column);
  static Vertex OfGtpString (const std::string& s);
  static Vertex OfGtpStream (std::istream& in);
  static Vertex OfSgfString (const std::string& s);

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

  std::string ToGtpString() const;

  // Other.

  static const uint kBound = (board_size + 2) * (board_size + 2) + 2;
  // board with guards + pass + any

private:
  friend class Nat <Vertex>;
  explicit Vertex (uint raw);
};

#define for_each_8_nbr(center_v, nbr_v, block) {                \
    Vertex nbr_v;                                               \
    nbr_v = center_v.N (); block;                               \
    nbr_v = center_v.W (); block;                               \
    nbr_v = center_v.E (); block;                               \
    nbr_v = center_v.S (); block;                               \
    nbr_v = center_v.NW (); block;                              \
    nbr_v = center_v.NE (); block;                              \
    nbr_v = center_v.SW (); block;                              \
    nbr_v = center_v.SE (); block;                              \
  }

// -----------------------------------------------------------------------------
#endif
