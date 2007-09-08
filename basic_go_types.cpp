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

// TODO check is check always checked :)

// namespace player

namespace player_aux {
  const uint black_idx = 0;
  const uint white_idx = 1;

  const uint cnt = 2;
}


class player_t {
  uint idx;

public:

  player_t () { idx = -1; }
  player_t (uint _idx) { idx = _idx; }

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

  bool in_range () const { return idx < player_aux::cnt; }
  void next () { idx++; }

  uint get_idx () const { return idx; }
  
};

const player_t player_black = player_t (player_aux::black_idx);
const player_t player_white = player_t (player_aux::white_idx);


// class player_map_t

template <typename elt_t> class player_map_t {
public:
  elt_t tab [player_aux::cnt];
  elt_t& operator[] (player_t pl)             { return tab [pl.get_idx ()]; }
  const elt_t& operator[] (player_t pl) const { return tab [pl.get_idx ()]; }
};


// faster than non-loop
#define player_for_each(pl) \
  for (player_t pl = player_black; pl.in_range (); pl.next ())


// namespace color


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

// TODO test it for performance
#define color_for_each(col) \
  for (color::t col = color::black; col != color::cnt; col = color::t (col+1))

// namespace coord


namespace coord {

  typedef int t;

  bool is_ok (t coord) { return uint (coord) < board_size; }

  

  void check (t coo) { 
    unused (coo);
    assertc (coord_ac, is_ok (coo)); 
  }

  void check2 (t row, t col) { 
    if (!coord_ac) return;
    if (row == -1 && col == -1) return;
    assertc (coord_ac, is_ok (row)); 
    assertc (coord_ac, is_ok (col)); 
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

#define coord_for_each(rc) \
  for (coord::t rc = 0; rc < int(board_size); rc = coord::t (rc+1))


// namespace v


namespace v_aux {

  const uint cnt = (board_size + 2) * (board_size + 2);

  const uint bits_used = 9;     // on 19x19 cnt == 441 < 512 == 1 << 9;
  //static_assert (cnt <= (1 << bits_used));

  const uint dNS = (board_size + 2);
  const uint dWE = 1;

  const uint pass_idx = 0;
  const uint no_v_idx = 1;
  const uint resign_idx = 2;
}


// class vertex_t


class vertex_t {

  uint idx;

public:


  vertex_t () { } // TODO is it needed
  vertex_t (uint _idx) { idx = _idx; }

  vertex_t (coord::t r, coord::t c) {
    coord::check2 (r, c);
    idx = (r+1) * v_aux::dNS + (c+1) * v_aux::dWE;
  }

  uint get_idx () { return idx; }

  bool operator== (vertex_t other) const { return idx == other.idx; }
  bool operator!= (vertex_t other) const { return idx != other.idx; }

  bool in_range ()          const { return idx < v_aux::cnt; }
  void next ()                    { idx++; }


  void check ()             const { assertc (v_ac, in_range ()); }

  coord::t row () { return idx / v_aux::dNS - 1; }

  coord::t col () { return idx % v_aux::dNS - 1; }

  bool is_on_board () { // TODO Ho here
#ifdef Ho
    coord::t r, c, d;
    r = row ();
    c = col ();
    d = r+c;
    return coord::is_ok (r) & coord::is_ok (c) & (d >= int((board_size-1)/2)) & (d < int(board_size + board_size/2));
#else
    return coord::is_ok (row ()) & coord::is_ok (col ());
#endif
  }

  void check_is_on_board () { 
    assertc (v_ac, is_on_board ()); 
  }

  vertex_t N () { return vertex_t (idx - v_aux::dNS); }
  vertex_t W () { return vertex_t (idx - v_aux::dWE); }
  vertex_t E () { return vertex_t (idx + v_aux::dWE); }
  vertex_t S () { return vertex_t (idx + v_aux::dNS); }

  vertex_t NW () { return N ().W (); } // TODO can it be faster?
  vertex_t NE () { return N ().E (); } // only Go
  vertex_t SW () { return S ().W (); } // only Go
  vertex_t SE () { return S ().E (); }

  string to_string () {
    coord::t r;
    coord::t c;
    
    if (idx == v_aux::pass_idx) {
      return "PASS";
    } else if (idx == v_aux::no_v_idx) {
      return "NO_V";
    } else if (idx == v_aux::resign_idx) {
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
  
const vertex_t vertex_pass    = vertex_t (v_aux::pass_idx);
const vertex_t vertex_no_v    = vertex_t (v_aux::no_v_idx);
const vertex_t vertex_resign  = vertex_t (v_aux::resign_idx); // TODO is it inlined (x2)?


template <typename elt_t> class vertex_map_t {
public:
  elt_t tab [v_aux::cnt];
  elt_t& operator[] (vertex_t v)             { return tab [v.get_idx ()]; }
  const elt_t& operator[] (vertex_t v) const { return tab [v.get_idx ()]; }
};


#define v_for_each_all(vv) for (vertex_t vv = 0; vv.in_range (); vv.next ()) // TODO 0 works??? // TODO player the same way!

#define v_for_each_onboard(vv)                                       \
  for (vertex_t vv = vertex_t(v_aux::dNS+v_aux::dWE);                \
       vv.get_idx () <= board_size * (v_aux::dNS + v_aux::dWE);      \
       vv.next ())


#ifdef Ho

  #define v_for_each_nbr(center_v, nbr_v, block) {  \
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

  #define v_for_each_nbr(center_v, nbr_v, block) {  \
    center_v.check_is_on_board ();                  \
    vertex_t nbr_v;                                 \
    nbr_v = center_v.N (); block;                   \
    nbr_v = center_v.W (); block;                   \
    nbr_v = center_v.E (); block;                   \
    nbr_v = center_v.S (); block;                   \
  }
    
  #define v_for_each_diag_nbr(center_v, nbr_v, block) {  \
    center_v.check_is_on_board ();                       \
    vertex_t nbr_v;                                      \
    nbr_v = center_v.NW (); block;                       \
    nbr_v = center_v.NE (); block;                       \
    nbr_v = center_v.SW (); block;                       \
    nbr_v = center_v.SE (); block;                       \
    }
  
  #define pl_v_for_each_9_nbr(center_v, pl, nbr_v, i) { \
    v::check_is_on_board (center_v);                    \
    move::t    nbr_v;                                   \
    player_for_each (pl) {                              \
      nbr_v = center_v;                                 \
      i;                                                \
      v_for_each_nbr      (center_v, nbr_v, i);         \
      v_for_each_diag_nbr (center_v, nbr_v, i);         \
    }                                                   \
  }
  
#endif

#define pl_v_for_each(pl, vv) player_for_each(pl) v_for_each_onboard(vv)


// namespace move


namespace move {

  typedef uint t;

  void check (t move) {
    player_t (move >> v_aux::bits_used);
    vertex_t (move & ((1 << v_aux::bits_used) - 1)).check ();
  }

  uint player_mask [player_aux::cnt] = {  // TODO map_t ?
    player_aux::black_idx << v_aux::bits_used, 
    player_aux::white_idx << v_aux::bits_used 
  };

  t of_pl_v (player_t player, vertex_t v) { 
    return player_mask [player.get_idx ()] | v.get_idx (); // TODO replace
  }

  const uint cnt = player_aux::white_idx << v_aux::bits_used | v_aux::cnt;
 
  const t no_move = 1;
  
  player_t to_player (t move) { 
    check (move);
    return (::player_t) (move >> v_aux::bits_used);
  }

  vertex_t v (t move) { 
    check (move);
    return move & ((1 << ::v_aux::bits_used) - 1) ; 
  }

  string to_string (t move) {
    return to_player (move).to_string () + " " + v (move).to_string ();
  }

}

#define move_for_each_all(m) for (move::t m = 0; m < move::cnt; m++)
