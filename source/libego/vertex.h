//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef VERTEX_H_
#define VERTEX_H_

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


// TODO of_gtp_string
istream& operator>> (istream& in, Vertex& v);

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

// TODO center_v.check_is_on_board ();
#define vertex_for_each_diag_nbr(center_v, nbr_v, block) {      \
    Vertex nbr_v;                                               \
    nbr_v = center_v.NW (); block;                              \
    nbr_v = center_v.NE (); block;                              \
    nbr_v = center_v.SW (); block;                              \
    nbr_v = center_v.SE (); block;                              \
  }

#endif
