//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef VERTEX_H_
#define VERTEX_H_

// TODO remove headers
#include <string>
#include <iomanip>

#include "utils.h"


// TODO move this to config.h
// TODO this have to be renamed to max_board_size
#ifdef BOARDSIZE
const uint board_size = BOARDSIZE;
#undef BOARDSIZE
#else
const uint board_size = 9;
#endif

// -----------------------------------------------------------------------------
const static uint kBoardAreaWithGuards = (board_size + 2) * (board_size + 2);

class Vertex : public Nat <kBoardAreaWithGuards> {
public:
  // Constructors.
  explicit Vertex();

  static Vertex Pass();
  static Vertex Any(); // TODO remove it
  static Vertex Resign();

  static Vertex OfRaw (uint raw);
  static Vertex OfSgfString (const string& s);
  static Vertex OfGtpString (const string& s);
  static Vertex OfCoords (int row, int column); // TODO uint

  // Utilities.

  int GetRow() const;
  int GetColumn() const;

  // this can be achieved quicker by color_at lookup
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

  const static uint bits_used = 9;     // on 19x19 kBound == 441 < 512 == 1 << 9;

private:
  explicit Vertex (uint _idx);
};

// -----------------------------------------------------------------------------

#define coord_for_each(rc) for (int rc = 0; rc < int(board_size); rc += 1)


// TODO of_gtp_string
istream& operator>> (istream& in, Vertex& v);

template <typename T>
string to_string_2d (const NatMap<Vertex, T>& map, int precision = 3) {
  ostringstream out;
  out << setiosflags (ios_base::fixed) ;
  
  coord_for_each (row) {
    coord_for_each (col) {
      Vertex v = Vertex (row, col);
      out.precision(precision);
      out.width(precision + 3);
      out << map [v] << " ";
    }
    out << endl;
  }
  return out.str ();
}

#define vertex_for_each_all(vv) for (Vertex vv; vv.Next();) 


// misses some offboard vertices (for speed) 
#define vertex_for_each_faster(vv)                                  \
  for (Vertex vv = Vertex(Vertex::dNS+Vertex::dWE);                 \
       vv.GetRaw () <= board_size * (Vertex::dNS + Vertex::dWE);   \
       vv.next ())


// TODO    center_v.check_is_on_board ();
#define vertex_for_each_4_nbr(center_v, nbr_v, block) { \
    Vertex nbr_v;                                       \
    nbr_v = center_v.N (); block;                       \
    nbr_v = center_v.W (); block;                       \
    nbr_v = center_v.E (); block;                       \
    nbr_v = center_v.S (); block;                       \
  }

#define vertex_for_each_5_nbr(center_v, nbr_v, block) { \
    center_v.check_is_on_board ();                      \
    Vertex nbr_v;                                       \
    nbr_v = center_v.N (); block;                       \
    nbr_v = center_v.W (); block;                       \
    nbr_v = center_v;      block;                       \
    nbr_v = center_v.E (); block;                       \
    nbr_v = center_v.S (); block;                       \
  }

// TODO center_v.check_is_on_board ();
#define vertex_for_each_diag_nbr(center_v, nbr_v, block) {      \
    Vertex nbr_v;                                               \
    nbr_v = center_v.NW (); block;                              \
    nbr_v = center_v.NE (); block;                              \
    nbr_v = center_v.SW (); block;                              \
    nbr_v = center_v.SE (); block;                              \
  }

#define vertex_for_each_8_nbr(center_v, nbr_v, block) {         \
    center_v.check_is_on_board ();                              \
    Vertex nbr_v;                                               \
    nbr_v = center_v.NW (); block;                              \
    nbr_v = center_v.N ();  block;                              \
    nbr_v = center_v.NE (); block;                              \
    nbr_v = center_v.W ();  block;                              \
    nbr_v = center_v.E ();  block;                              \
    nbr_v = center_v.SW (); block;                              \
    nbr_v = center_v.S ();  block;                              \
    nbr_v = center_v.SE (); block;                              \
  }


#define vertex_for_each_9_nbr(center_v, nbr_v, block) {         \
    center_v.check_is_on_board ();                              \
    Vertex nbr_v;                                               \
    nbr_v = center_v.NW (); block;                              \
    nbr_v = center_v.N ();  block;                              \
    nbr_v = center_v.NE (); block;                              \
    nbr_v = center_v.W ();  block;                              \
    nbr_v = center_v;       block;                              \
    nbr_v = center_v.E ();  block;                              \
    nbr_v = center_v.SW (); block;                              \
    nbr_v = center_v.S ();  block;                              \
    nbr_v = center_v.SE (); block;                              \
  }

#endif
