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


#define coord_for_each(rc) \
  for (Coord rc(0); rc.idx < int(board_size); rc = Coord (rc.idx+1))


class Vertex {

public:
  static Vertex pass   ();
  static Vertex any    ();
  static Vertex resign ();
  static Vertex of_sgf_coords (string s);

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


// TODO of_gtp_string
istream& operator>> (istream& in, Vertex& v);
ostream& operator<< (ostream& out, Vertex& v);

template <typename T>
string to_string_2d (FastMap<Vertex, T>& map, int precision = 3) {
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


#define vertex_for_each_all(vv)                                         \
  for (Vertex vv = Vertex(0); vv.in_range (); vv.next ()) 
// TODO 0 works??? // TODO player the same way!

// misses some offboard vertices (for speed) 
#define vertex_for_each_faster(vv)                                  \
  for (Vertex vv = Vertex(Vertex::dNS+Vertex::dWE);                 \
       vv.get_idx () <= board_size * (Vertex::dNS + Vertex::dWE);   \
       vv.next ())


#define vertex_for_each_nbr(center_v, nbr_v, block) {   \
    center_v.check_is_on_board ();                      \
    Vertex nbr_v;                                       \
    nbr_v = center_v.N (); block;                       \
    nbr_v = center_v.W (); block;                       \
    nbr_v = center_v.E (); block;                       \
    nbr_v = center_v.S (); block;                       \
  }

#define vertex_for_each_diag_nbr(center_v, nbr_v, block) {      \
    center_v.check_is_on_board ();                              \
    Vertex nbr_v;                                               \
    nbr_v = center_v.NW (); block;                              \
    nbr_v = center_v.NE (); block;                              \
    nbr_v = center_v.SW (); block;                              \
    nbr_v = center_v.SE (); block;                              \
  }

#define player_vertex_for_each_9_nbr(center_v, pl, nbr_v, i) {  \
    v::check_is_on_board (center_v);                            \
    Move    nbr_v;                                              \
    player_for_each (pl) {                                      \
      nbr_v = center_v;                                         \
      i;                                                        \
      vertex_for_each_nbr      (center_v, nbr_v, i);            \
      vertex_for_each_diag_nbr (center_v, nbr_v, i);            \
    }                                                           \
  }

#endif
