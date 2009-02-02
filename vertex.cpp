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

class Coord {
public:
  explicit Coord() {
  }

  explicit Coord(int idx_) {
    idx = idx_;
  }

  bool is_ok () const {
    return (idx < int (board_size)) & (idx >= -1); 
  }

  void check () const { 
    assertc (coord_ac, is_ok()); 
  }

  bool is_on_board () const {
    return uint (idx) < board_size; 
  }

  // TODO to gtp string
  string row_to_string () const {
    check ();
    ostringstream ss;
    ss << board_size - idx;
    return ss.str ();
  }

  string col_to_string () const {
    check ();
    ostringstream ss;
    ss << col_tab [idx];
    return ss.str ();
  }

  static const string col_tab;

  int idx;
};

const string Coord::col_tab = "ABCDEFGHJKLMNOPQRSTUVWXYZ";

#define coord_for_each(rc) \
  for (Coord rc(0); rc.idx < int(board_size); rc = Coord (rc.idx+1))


//--------------------------------------------------------------------------------

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
  return idx < cnt; 
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


// TODO of_gtp_string
istream& operator>> (istream& in, Vertex& v) {
  char c;
  int n;

  string str;
  if (!(in >> str)) return in;
  if (str == "pass" || str == "PASS" || str == "Pass") { 
    v = Vertex::pass ();
    return in; 
  }
  if (str == "resign" || str == "RESIGN" || str == "Resign") {
    v = Vertex::resign ();
    return in; 
  }

  istringstream in2 (str);
  if (!(in2 >> c >> n)) return in;

  Coord row (board_size - n);
  
  Coord col (0);
  while (col.idx < int (Coord::col_tab.size ())) {
    if (Coord::col_tab[col.idx] == c || 
        Coord::col_tab[col.idx] -'A' + 'a' == c ) 
      break;
    col.idx++;
  }
  
  if (col.idx == int (Coord::col_tab.size ())) {
    in.setstate (ios_base::badbit);
    return in;
  }

  v = Vertex (row, col);
  return in;
}

ostream& operator<< (ostream& out, Vertex& v) { out << v.to_string (); return out; }

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
