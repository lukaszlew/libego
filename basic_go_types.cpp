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


/* auxiliary constants that can't won't be inlined if static in class */


namespace player_aux {
  const uint black_idx = 0;
  const uint white_idx = 1;

  const uint cnt = 2;
}


namespace vertex_aux {

  const uint cnt = (board_size + 2) * (board_size + 2);

  const uint bits_used = 9;     // on 19x19 cnt == 441 < 512 == 1 << 9;
  //static_assert (cnt <= (1 << bits_used));
  //static_assert (cnt > (1 << (bits_used-1)));
  const uint dNS = (board_size + 2);
  const uint dWE = 1;

  const uint pass_idx = 0;
  const uint any_idx  = 1; // TODO any
  const uint resign_idx = 2;
}


namespace move_aux {
  uint player_mask [player_aux::cnt] = {  // TODO map_t ?
    player_aux::black_idx << vertex_aux::bits_used, 
    player_aux::white_idx << vertex_aux::bits_used 
  };

  const uint cnt = player_aux::white_idx << vertex_aux::bits_used | vertex_aux::cnt;
 
  const uint no_move_idx = 1;

}



/********************************************************************************/



class player_t { // TODO check is check always checked in constructors

  uint idx;

public:

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
    if (idx == player_aux::black_idx)
      return "#";
    else
      return "O";
  }

  bool in_range () const { return idx < player_aux::cnt; } // TODO do it like check
  void next () { idx++; }

  uint get_idx () const { return idx; }
  
};

const player_t player_black = player_t (player_aux::black_idx);
const player_t player_white = player_t (player_aux::white_idx);


//------------------------------------------------------------------------------

namespace color {

  enum t {
    black = 0,
    white = 1,
    empty = 2,
    off_board  = 3
  };

  const uint cnt = 4;

  void check (t color) { 
    unused (color);
    assertc (color_ac, (color & (~3)) == 0); 
  }

  bool is_player (t color) { return color <= color::white; } // & (~1)) == 0; }
  bool is_not_player (t color) { return color > color::white; } // & (~1)) == 0; }

  t opponent (player_t player) {
    unused (opponent);          // avoids warning
    unused (player);
    return t (player.get_idx () ^ 1);
  }

  char to_char (t color) { 
    check (color);
    switch (color) {
    case black: return '#';
    case white: return 'O';
    case empty: return '.';
    case off_board:  return ' ';
    default : assertc (color_ac, false);
    }
    return '?';                 // should not happen
  }

  const t wrong_char = t(4);

  t of_char (char c) {  // may return t(4)
     switch (c) {
     case '#': return black;
     case 'O': return white;
     case '.': return empty;
     case '*': return off_board;
     default : return wrong_char;
     }
  }

}


//--------------------------------------------------------------------------------


namespace coord {

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

  char col_tab[20] = "ABCDEFGHJKLMNOPQRST";

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


//--------------------------------------------------------------------------------


class vertex_t {

  uint idx;

public:


  vertex_t () { } // TODO is it needed
  vertex_t (uint _idx) { idx = _idx; }

  vertex_t (coord::t r, coord::t c) {
    coord::check2 (r, c);
    idx = (r+1) * vertex_aux::dNS + (c+1) * vertex_aux::dWE;
  }

  uint get_idx () { return idx; }

  bool operator== (vertex_t other) const { return idx == other.idx; }
  bool operator!= (vertex_t other) const { return idx != other.idx; }

  bool in_range ()          const { return idx < vertex_aux::cnt; }
  void next ()                    { idx++; }

  void check ()             const { assertc (vertex_ac, in_range ()); }

  coord::t row () { return idx / vertex_aux::dNS - 1; }

  coord::t col () { return idx % vertex_aux::dNS - 1; }

  bool is_on_board () { // TODO Ho here
#ifdef Ho
    coord::t r, c, d;
    r = row ();
    c = col ();
    d = r+c;
    return coord::is_ok (r) & coord::is_ok (c) & (d >= int((board_size-1)/2)) & (d < int(board_size + board_size/2));
#else
    return coord::is_on_board (row ()) & coord::is_on_board (col ());
#endif
  }

  void check_is_on_board () { 
    assertc (vertex_ac, is_on_board ()); 
  }

  vertex_t N () { return vertex_t (idx - vertex_aux::dNS); }
  vertex_t W () { return vertex_t (idx - vertex_aux::dWE); }
  vertex_t E () { return vertex_t (idx + vertex_aux::dWE); }
  vertex_t S () { return vertex_t (idx + vertex_aux::dNS); }

  vertex_t NW () { return N ().W (); } // TODO can it be faster?
  vertex_t NE () { return N ().E (); } // only Go
  vertex_t SW () { return S ().W (); } // only Go
  vertex_t SE () { return S ().E (); }

  string to_string () {
    coord::t r;
    coord::t c;
    
    if (idx == vertex_aux::pass_idx) {
      return "PASS";
    } else if (idx == vertex_aux::any_idx) {
      return "NO_V";
    } else if (idx == vertex_aux::resign_idx) {
      return "resign";
    } else {
      r = row ();
      c = col ();
      ostringstream ss;
      ss << coord::col_to_string (c) << coord::row_to_string (r);
      return ss.str ();
    }
  }

};
  
const vertex_t vertex_pass    = vertex_t (vertex_aux::pass_idx);
const vertex_t vertex_any     = vertex_t (vertex_aux::any_idx);
const vertex_t vertex_resign  = vertex_t (vertex_aux::resign_idx); // TODO is it inlined (x2)?


//--------------------------------------------------------------------------------


class move_t {
public:

  uint idx;

  void check () {
    player_t (idx >> vertex_aux::bits_used);
    vertex_t (idx & ((1 << vertex_aux::bits_used) - 1)).check ();
  }

  move_t (player_t player, vertex_t v) { 
    idx = move_aux::player_mask [player.get_idx ()] | v.get_idx (); // TODO replace
  }

  move_t () {
    move_t (player_black, vertex_any);
  }

  move_t (int idx_) {
    idx = idx_;
  }

  player_t get_player () { 
    return player_t (idx >> vertex_aux::bits_used);
  }

  vertex_t get_vertex () { 
    return vertex_t (idx & ((1 << ::vertex_aux::bits_used) - 1)) ; 
  }

  string to_string () {
    return get_player ().to_string () + " " + get_vertex ().to_string ();
  }

  uint get_idx () { return idx; }

  bool operator== (move_t other) const { return idx == other.idx; }
  bool operator!= (move_t other) const { return idx != other.idx; }

  bool in_range ()          const { return idx < move_aux::cnt; }
  void next ()                    { idx++; }

};


/********************************************************************************/

// macros *_for_each

// faster than non-loop
#define player_for_each(pl) \
  for (player_t pl = player_black; pl.in_range (); pl.next ())



// TODO test it for performance
#define color_for_each(col) \
  for (color::t col = color::black; col != color::cnt; col = color::t (col+1))


#define coord_for_each(rc) \
  for (coord::t rc = 0; rc < int(board_size); rc = coord::t (rc+1))


#define vertex_for_each_all(vv) for (vertex_t vv = 0; vv.in_range (); vv.next ()) // TODO 0 works??? // TODO player the same way!

// misses some offboard vertices (for speed) 
#define vertex_for_each_faster(vv)                                       \
  for (vertex_t vv = vertex_t(vertex_aux::dNS+vertex_aux::dWE);                \
       vv.get_idx () <= board_size * (vertex_aux::dNS + vertex_aux::dWE);      \
       vv.next ())


#ifdef Ho

  #define vertex_for_each_nbr(center_v, nbr_v, block) {  \
    center_v.check_is_on_board ();                  \
    vertex_t nbr_v;                                 \
    nbr_v = center_v.N (); block;                   \
    nbr_v = center_v.NE(); block;                   \
    nbr_v = center_v.W (); block;                   \
    nbr_v = center_v.E (); block;                   \
    nbr_v = center_v.SW(); block;                   \
    nbr_v = center_v.S (); block;                   \
  }

#else

  #define vertex_for_each_nbr(center_v, nbr_v, block) {  \
    center_v.check_is_on_board ();                  \
    vertex_t nbr_v;                                 \
    nbr_v = center_v.N (); block;                   \
    nbr_v = center_v.W (); block;                   \
    nbr_v = center_v.E (); block;                   \
    nbr_v = center_v.S (); block;                   \
  }
    
  #define vertex_for_each_diag_nbr(center_v, nbr_v, block) {  \
    center_v.check_is_on_board ();                       \
    vertex_t nbr_v;                                      \
    nbr_v = center_v.NW (); block;                       \
    nbr_v = center_v.NE (); block;                       \
    nbr_v = center_v.SW (); block;                       \
    nbr_v = center_v.SE (); block;                       \
    }
  
  #define player_vertex_for_each_9_nbr(center_v, pl, nbr_v, i) { \
    v::check_is_on_board (center_v);                    \
    move_t    nbr_v;                                    \
    player_for_each (pl) {                              \
      nbr_v = center_v;                                 \
      i;                                                \
      vertex_for_each_nbr      (center_v, nbr_v, i);         \
      vertex_for_each_diag_nbr (center_v, nbr_v, i);         \
    }                                                   \
  }
  
#endif

#define move_for_each_all(m) for (move_t m = 0; m.in_range (); m.next ())


/********************************************************************************/

// dictionaries indexed with basic go types

template <typename elt_t> class player_map_t {
public:
  elt_t tab [player_aux::cnt];
  elt_t& operator[] (player_t pl)             { return tab [pl.get_idx ()]; }
  const elt_t& operator[] (player_t pl) const { return tab [pl.get_idx ()]; }
};


template <typename elt_t> class vertex_map_t {
public:
  elt_t tab [vertex_aux::cnt];
  elt_t& operator[] (vertex_t v)             { return tab [v.get_idx ()]; }
  const elt_t& operator[] (vertex_t v) const { return tab [v.get_idx ()]; }

  string to_string () {
    ostringstream out;
    coord_for_each (row) {
      coord_for_each (col) {
        vertex_t v = vertex_t (row, col);
        out << tab [v.get_idx ()] << " ";
      }
      out << endl;
    }
    return out.str ();
  }

};


template <typename elt_t> class move_map_t {
public:
  elt_t tab [move_aux::cnt];
  elt_t& operator[] (move_t m)             { return tab [m.get_idx ()]; }
  const elt_t& operator[] (move_t m) const { return tab [m.get_idx ()]; }
};
