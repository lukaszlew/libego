#ifndef _VERTEX_H_
#define _VERTEX_H_

class Coord {
public:
  explicit Coord();
  explicit Coord(int idx_);
  bool is_ok () const;
  void check () const;
  bool is_on_board () const;
  string row_to_string () const;
  string col_to_string () const;

  static const string col_tab;
  int idx;
};


class Vertex {

public:
  static Vertex pass   ();
  inline static Vertex any    ();
  static Vertex resign ();
  inline static Vertex of_sgf_coords (string s);

  explicit Vertex (); // TODO is it needed
  explicit Vertex (uint _idx);

  // TODO make this constructor a static function
  Vertex (Coord r, Coord c);

  Coord get_row () const;
  Coord get_col () const;

  // this usualy can be achieved quicker by color_at lookup
  bool is_on_board () const;

  Vertex N () const;
  Vertex W () const;
  Vertex E () const;
  Vertex S () const;

  Vertex NW () const;
  Vertex NE () const;
  Vertex SW () const;
  Vertex SE () const;

  string to_string () const;

  bool operator== (Vertex other) const;
  bool operator!= (Vertex other) const;

  bool in_range () const;
  void next ();

  void check () const;

  void check_is_on_board () const;

  uint get_idx () const;

  const static uint dNS = (board_size + 2);
  const static uint dWE = 1;

  const static uint bits_used = 9;     // on 19x19 cnt == 441 < 512 == 1 << 9;
  const static uint pass_idx = 0;
  const static uint any_idx  = 1; // TODO any
  const static uint resign_idx = 2;

  const static uint cnt = (board_size + 2) * (board_size + 2);

private:

  uint idx;

};

#endif
