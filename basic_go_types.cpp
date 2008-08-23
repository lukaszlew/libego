/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                           *
 *  This file is part of Library of Effective GO routines - EGO library      *
 *                                                                           *
 *  Copyright 2006, 2007 Lukasz Lew                                          *
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



class player_t { // TODO check is check always checked in constructors

  uint idx;

public:

  const static uint black_idx = 0;
  const static uint white_idx = 1;

  const static uint cnt = 2;


  player_t () { idx = -1; }
  player_t (uint _idx) { idx = _idx; check ();}

  bool operator== (player_t other) const { return idx == other.idx; }

  void check () const { 
    assertc (player_ac, (idx & (~1)) == 0);
  }

  player_t other () const { 
    return player_t(idx ^ 1);
  }
  
  string to_string () const {
    if (idx == black_idx)
      return "#";
    else
      return "O";
  }

  bool in_range () const { return idx < cnt; } // TODO do it like check
  void next () { idx++; }

  uint get_idx () const { return idx; }
  
  static player_t black () { return player_t (black_idx); }
  static player_t white () { return player_t (white_idx); }

  template <typename elt_t> class map_t {
  public:
    elt_t tab [cnt];
    elt_t& operator[] (player_t pl)             { return tab [pl.get_idx ()]; }
    const elt_t& operator[] (player_t pl) const { return tab [pl.get_idx ()]; }
  };

};


istream& operator>> (istream& in, player_t& pl) {
  string s;
  in >> s;
  if (s == "b" || s == "B" || s == "Black" || s == "BLACK "|| s == "black" || s == "#") { pl = player_t::black (); return in; }
  if (s == "w" || s == "W" || s == "White" || s == "WHITE "|| s == "white" || s == "O") { pl = player_t::white (); return in; }
  in.setstate (ios_base::badbit);
  return in;
}

ostream& operator<< (ostream& out, player_t& pl) { out << pl.to_string (); return out; }

// faster than non-loop
#define player_for_each(pl) \
  for (player_t pl = player_t::black (); pl.in_range (); pl.next ())

//------------------------------------------------------------------------------
// class color

class color_t {
  uint idx;
public:

  const static uint black_idx = 0;
  const static uint white_idx = 1;
  const static uint empty_idx = 2;
  const static uint off_board_idx  = 3;
  const static uint wrong_char_idx = 40;

  const static uint cnt = 4;

  color_t () { idx = -1; } // TODO test - remove it

  color_t (uint idx_) { idx = idx_; } // TODO test - remove it

  color_t (player_t pl) { idx = pl.get_idx (); }

  color_t (char c, uint dummy) {  // may return color_wrong_char // TODO dummmy is just to be able to choose the constructor
     switch (c) {
     case '#': idx = black_idx; break;
     case 'O': idx = white_idx; break;
     case '.': idx = empty_idx; break;
     case '*': idx = off_board_idx; break;
     default : idx = wrong_char_idx; break;
     }
  }

  void check () const { 
    assertc (color_ac, (idx & (~3)) == 0); 
  }

  bool is_player     () const { return idx <= white_idx; } // & (~1)) == 0; }
  bool is_not_player () const { return idx  > white_idx; } // & (~1)) == 0; }

  player_t to_player () const { return player_t (idx); }

  char to_char () const { 
    switch (idx) {
    case black_idx:      return '#';
    case white_idx:      return 'O';
    case empty_idx:      return '.';
    case off_board_idx:  return ' ';
    default : assertc (color_ac, false);
    }
    return '?';                 // should not happen
  }

  bool in_range () const { return idx < color_t::cnt; }
  void next () { idx++; }

  uint get_idx () const { return idx; }
  bool operator== (color_t other) const { return idx == other.idx; }
  bool operator!= (color_t other) const { return idx != other.idx; }

  static color_t black ()      { return color_t (black_idx); }
  static color_t white ()      { return color_t (white_idx); }
  static color_t empty ()      { return color_t (empty_idx); }
  static color_t off_board  () { return color_t (off_board_idx); }
  static color_t wrong_char () { return color_t (wrong_char_idx); }


  // class color_map_t
  
  template <typename elt_t> class map_t {
  public:
    elt_t tab [color_t::cnt];
    elt_t& operator[] (color_t col)             { return tab [col.get_idx ()]; }
    const elt_t& operator[] (color_t col) const { return tab [col.get_idx ()]; }
  };

};

// TODO test it for performance
#define color_for_each(col) \
  for (color_t col = color_t::black (); col.in_range (); col.next ())

//--------------------------------------------------------------------------------


namespace coord { // TODO class

  typedef int t;

  bool is_ok (t coord) { return (coord < int (board_size)) & (coord >= -1); }
  bool is_on_board (t coord) { return uint (coord) < board_size; }

  void check (t coo) { 
    unused (coo);
    assertc (coord_ac, is_ok (coo)); 
  }

  void check2 (t row, t col) { 
    if (!coord_ac) return;
    if (row == -1 && col == -1) return;
    assertc (coord_ac, is_on_board (row)); 
    assertc (coord_ac, is_on_board (col)); 
  }

  string col_tab = "ABCDEFGHJKLMNOPQRSTUVWXYZ";

  // TODO to gtp string
  string row_to_string (t row) {
    check (row);
    ostringstream ss;
    ss << board_size - row;
    return ss.str ();
  }

  string col_to_string (t col) {
    check (col);
    ostringstream ss;
    ss << col_tab [col];
    return ss.str ();
  }

}

#define coord_for_each(rc) \
  for (coord::t rc = 0; rc < int(board_size); rc = coord::t (rc+1))


//--------------------------------------------------------------------------------



class vertex_t {


  //static_assert (cnt <= (1 << bits_used));
  //static_assert (cnt > (1 << (bits_used-1)));


  uint idx;

public:

  const static uint dNS = (board_size + 2);
  const static uint dWE = 1;

  const static uint bits_used = 9;     // on 19x19 cnt == 441 < 512 == 1 << 9;
  const static uint pass_idx = 0;
  const static uint any_idx  = 1; // TODO any
  const static uint resign_idx = 2;

  const static uint cnt = (board_size + 2) * (board_size + 2);

  vertex_t () { } // TODO is it needed
  vertex_t (uint _idx) { idx = _idx; }

 // TODO make this constructor a static function
  vertex_t (coord::t r, coord::t c) {
    coord::check2 (r, c);
    idx = (r+1) * dNS + (c+1) * dWE;
  }

  uint get_idx () const { return idx; }

  bool operator== (vertex_t other) const { return idx == other.idx; }
  bool operator!= (vertex_t other) const { return idx != other.idx; }

  bool in_range ()          const { return idx < cnt; }
  void next ()                    { idx++; }

  void check ()             const { assertc (vertex_ac, in_range ()); }

  coord::t row () const { return idx / dNS - 1; }

  coord::t col () const { return idx % dNS - 1; }

  // this usualy can be achieved quicker by color_at lookup
  bool is_on_board () const {
    return coord::is_on_board (row ()) & coord::is_on_board (col ());
  }

  void check_is_on_board () const { 
    assertc (vertex_ac, is_on_board ()); 
  }

  vertex_t N () const { return vertex_t (idx - dNS); }
  vertex_t W () const { return vertex_t (idx - dWE); }
  vertex_t E () const { return vertex_t (idx + dWE); }
  vertex_t S () const { return vertex_t (idx + dNS); }

  vertex_t NW () const { return N ().W (); } // TODO can it be faster?
  vertex_t NE () const { return N ().E (); } // only Go
  vertex_t SW () const { return S ().W (); } // only Go
  vertex_t SE () const { return S ().E (); }

  string to_string () const {
    coord::t r;
    coord::t c;
    
    if (idx == pass_idx) {
      return "pass";
    } else if (idx == any_idx) {
      return "any";
    } else if (idx == resign_idx) {
      return "resign";
    } else {
      r = row ();
      c = col ();
      ostringstream ss;
      ss << coord::col_to_string (c) << coord::row_to_string (r);
      return ss.str ();
    }
  }


  template <typename elt_t> class map_t {
  public:
    elt_t tab [cnt];
    elt_t& operator[] (vertex_t v)             { return tab [v.get_idx ()]; }
    const elt_t& operator[] (vertex_t v) const { return tab [v.get_idx ()]; }
    
    string to_string_2d (int precision = 3) {
      ostringstream out;
      out << setiosflags (ios_base::fixed) ;
      
      coord_for_each (row) {
        coord_for_each (col) {
          vertex_t v = vertex_t (row, col);
          out.precision(precision);
          out.width(precision + 3);
          out << tab [v.get_idx ()] << " ";
        }
        out << endl;
      }
      return out.str ();
    }
    
  };

  static vertex_t pass   () { return vertex_t (vertex_t::pass_idx); }
  static vertex_t any    () { return vertex_t (vertex_t::any_idx); }
  static vertex_t resign () { return vertex_t (vertex_t::resign_idx); }

  static vertex_t of_sgf_coords (string s) {
    if (s == "") return pass ();
    if (s == "tt" && board_size <= 19) return pass ();
    if (s.size () != 2 ) return any ();
    coord::t col = s[0] - 'a';
    coord::t row = s[1] - 'a';
    
    if (coord::is_on_board (row) &&
        coord::is_on_board (col)) {
      return vertex_t (row, col);
    } else {
      return any ();
    }
  }
};
  

// TODO of_gtp_string
istream& operator>> (istream& in, vertex_t& v) {
  char c;
  int n;
  coord::t row, col;

  string str;
  if (!(in >> str)) return in;
  if (str == "pass" || str == "PASS" || str == "Pass") { v = vertex_t::pass (); return in; }
  if (str == "resign" || str == "RESIGN" || str == "Resign") { v = vertex_t::resign (); return in; }

  istringstream in2 (str);
  if (!(in2 >> c >> n)) return in;

  row = board_size - n;
  
  col = 0;
  while (col < int (coord::col_tab.size ())) {
    if (coord::col_tab[col] == c || coord::col_tab[col] -'A' + 'a' == c ) break;
    col++;
  }
  
  if (col == int (coord::col_tab.size ())) {
    in.setstate (ios_base::badbit);
    return in;
  }

  v = vertex_t (row, col);
  return in;
}

ostream& operator<< (ostream& out, vertex_t& v) { out << v.to_string (); return out; }


#define vertex_for_each_all(vv) for (vertex_t vv = 0; vv.in_range (); vv.next ()) // TODO 0 works??? // TODO player the same way!

// misses some offboard vertices (for speed) 
#define vertex_for_each_faster(vv)                                      \
  for (vertex_t vv = vertex_t(vertex_t::dNS+vertex_t::dWE);             \
       vv.get_idx () <= board_size * (vertex_t::dNS + vertex_t::dWE);   \
       vv.next ())


#define vertex_for_each_nbr(center_v, nbr_v, block) {   \
    center_v.check_is_on_board ();                      \
    vertex_t nbr_v;                                     \
    nbr_v = center_v.N (); block;                       \
    nbr_v = center_v.W (); block;                       \
    nbr_v = center_v.E (); block;                       \
    nbr_v = center_v.S (); block;                       \
  }

#define vertex_for_each_diag_nbr(center_v, nbr_v, block) {      \
    center_v.check_is_on_board ();                              \
    vertex_t nbr_v;                                             \
    nbr_v = center_v.NW (); block;                              \
    nbr_v = center_v.NE (); block;                              \
    nbr_v = center_v.SW (); block;                              \
    nbr_v = center_v.SE (); block;                              \
  }

#define player_vertex_for_each_9_nbr(center_v, pl, nbr_v, i) {  \
    v::check_is_on_board (center_v);                            \
    move_t    nbr_v;                                            \
    player_for_each (pl) {                                      \
      nbr_v = center_v;                                         \
      i;                                                        \
      vertex_for_each_nbr      (center_v, nbr_v, i);            \
      vertex_for_each_diag_nbr (center_v, nbr_v, i);            \
    }                                                           \
  }


//--------------------------------------------------------------------------------

class move_t {
public:

  const static uint cnt = player_t::white_idx << vertex_t::bits_used | vertex_t::cnt;
 
  const static uint no_move_idx = 1;

  uint idx;

  void check () {
    player_t (idx >> vertex_t::bits_used);
    vertex_t (idx & ((1 << vertex_t::bits_used) - 1)).check ();
  }

  move_t (player_t player, vertex_t v) { 
    idx = (player.get_idx () << vertex_t::bits_used) | v.get_idx ();
  }

  move_t () {
    move_t (player_t::black (), vertex_t::any ());
  }

  move_t (int idx_) {
    idx = idx_;
  }

  player_t get_player () { 
    return player_t (idx >> vertex_t::bits_used);
  }

  vertex_t get_vertex () { 
    return vertex_t (idx & ((1 << ::vertex_t::bits_used) - 1)) ; 
  }

  string to_string () {
    return get_player ().to_string () + " " + get_vertex ().to_string ();
  }

  uint get_idx () { return idx; }

  bool operator== (move_t other) const { return idx == other.idx; }
  bool operator!= (move_t other) const { return idx != other.idx; }

  bool in_range ()          const { return idx < cnt; }
  void next ()                    { idx++; }

  template <typename elt_t> class map_t {
  public:
    elt_t tab [cnt];
    elt_t& operator[] (move_t m)             { return tab [m.get_idx ()]; }
    const elt_t& operator[] (move_t m) const { return tab [m.get_idx ()]; }
  };

};

istream& operator>> (istream& in, move_t& m) {
  player_t pl;
  vertex_t v;
  if (!(in >> pl >> v)) return in;
  m = move_t (pl, v);
  return in;
}

#define move_for_each_all(m) for (move_t m = 0; m.in_range (); m.next ())

/********************************************************************************/
