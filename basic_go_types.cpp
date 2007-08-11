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



// namespace player


namespace player {

  enum t {
    black = 0,
    white = 1
  };

  const uint cnt = 2;

  void check (t player) { 
    unused (player);            // TODO why unused is needed ?
    assertc (player_ac, (player & (~1)) == 0);
  }

  t other (t player) { 
    check (player);
    return (t)(player ^ 1);
  }
  
  string to_string (t player) {
    if (player == black)
      return "#";
    else
      return "O";
  }
  
}

// faster than non-loop
#define player_for_each(pl) \
  for (player::t pl = player::black; pl != player::cnt; pl = player::t(pl+1))


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

  t opponent (player::t player) {
    unused (opponent);          // avoids warning
    unused (player);
    player::check (player);
    return t (player ^ 1);
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


namespace v {

  typedef uint t;
  
  const uint cnt = (board_size + 2) * (board_size + 2);
  const uint bits_used = 9;     // on 19x19 cnt == 441 < 512 == 1 << 9;
  //static_assert (cnt <= (1 << bits_used));

  const uint dNS = (board_size + 2);
  const uint dWE = 1;

  const t pass    = 0;
  const t no_v    = 1;
  const t resign  = 2;

  void check (t v) { 
    unused (v);
    assertc (v_ac, v < cnt); 
  }

  coord::t row (t v) {
    check (v);
    return v / dNS - 1;
  }

  coord::t col (t v) {
    check (v);
    return v % dNS - 1;
  }

  bool is_on_board (t v) { // TODO Ho here
    check (v);
#ifdef Ho
    coord::t r, c, d;
    r = row (v);
    c = col (v);
    d = r+c;
    return coord::is_ok (r) & coord::is_ok (c) & (d >= int((board_size-1)/2)) & (d < int(board_size + board_size/2));
#else
    return coord::is_ok (row(v)) & coord::is_ok (col(v));
#endif
  }

  void check_is_on_board (t v) { 
    unused (v);
    assertc (v_ac, is_on_board (v)); 
  }

  t N (t v) { check (v); check (v - dNS); return v - dNS; }
  t W (t v) { check (v); check (v - dWE); return v - dWE; }
  t E (t v) { check (v); check (v + dWE); return v + dWE; }
  t S (t v) { check (v); check (v + dNS); return v + dNS; }

  t NW (t v) { check (v); return N(W(v)); }
  t NE (t v) { check (v); return N(E(v)); } // only Go
  t SW (t v) { check (v); return S(W(v)); } // only Go
  t SE (t v) { check (v); return S(E(v)); }

  t of_rc (coord::t r, coord::t c) {
    coord::check2 (r, c);
    return (r+1) * dNS + (c+1) * dWE;
  }

  string to_string (t v) {
    coord::t r;
    coord::t c;
    check (v);
    
    if (v == pass) {
      return "PASS";
    } else if (v == no_v) {
      return "NO_V";
    } else if (v == resign) {
      return "resign";
    } else {
      r = row (v);
      c = col (v);
      ostringstream ss;
      ss << coord::col_to_string (c) << coord::row_to_string (r);
      return ss.str ();
    }
    
  }

}

#define v_for_each_all(vv) for (v::t vv = 0; vv < v::cnt; vv++)

#define v_for_each_onboard(vv) \
  for (v::t vv = v::dNS+v::dWE; vv <= board_size * (v::dNS + v::dWE); vv++)


#ifdef Ho

  #define v_for_each_nbr(center_v, nbr_v, block) {  \
    v::check_is_on_board (center_v);                \
    v::t nbr_v;                                     \
    nbr_v = v::N (center_v); block;                 \
    nbr_v = v::NE(center_v); block;                 \
    nbr_v = v::W (center_v); block;                 \
    nbr_v = v::E (center_v); block;                 \
    nbr_v = v::SW(center_v); block;                 \
    nbr_v = v::S (center_v); block;                 \
  }

#else

  #define v_for_each_nbr(center_v, nbr_v, block) {  \
    v::check_is_on_board (center_v);                \
    v::t nbr_v;                                     \
    nbr_v = v::N (center_v); block;                 \
    nbr_v = v::W (center_v); block;                 \
    nbr_v = v::E (center_v); block;                 \
    nbr_v = v::S (center_v); block;                 \
  }
    
  #define v_for_each_diag_nbr(center_v, nbr_v, block) {  \
    v::check_is_on_board (center_v);                     \
    v::t nbr_v;                                          \
    nbr_v = v::NW (center_v); block;                     \
    nbr_v = v::NE (center_v); block;                     \
    nbr_v = v::SW (center_v); block;                     \
    nbr_v = v::SE (center_v); block;                     \
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
    player::check (player::t (move >> v::bits_used));
           v::check (move & ((1 << v::bits_used) - 1));
  }

  uint player_mask [player::cnt] = { 
    player::black << v::bits_used, 
    player::white << v::bits_used 
  };

  t of_pl_v (player::t player, v::t v) { 
    player::check (player);
    v::check (v);
    return player_mask [player] | v;
  }

  const uint cnt = player::white << v::bits_used | v::cnt;
 
  const t no_move = 1;
  
  player::t player (t move) { 
    check (move);
    return (::player::t) (move >> v::bits_used);
  }

  v::t v (t move) { 
    check (move);
    return move & ((1 << ::v::bits_used) - 1) ; 
  }

  string to_string (t move) {
    return ::player::to_string (player (move)) + " " + ::v::to_string (v (move));
  }

}

#define move_for_each_all(m) for (move::t m = 0; m < move::cnt; m++)
