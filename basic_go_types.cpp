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

  const uint cnt = (board_size + 2) * (board_size + 2);

  const uint bits_used = 9;     // on 19x19 cnt == 441 < 512 == 1 << 9;
  //static_assert (cnt <= (1 << bits_used));

  const uint dNS = (board_size + 2);
  const uint dWE = 1;

  const uint pass_idx = 0;
  const uint no_v_idx = 1;
  const uint resign_idx = 2;


  class t {
  public:

    uint idx;

    t () { } // TODO is it needed
    t (uint _idx) { idx = _idx; }

    t (coord::t r, coord::t c) {
      coord::check2 (r, c);
      idx = (r+1) * dNS + (c+1) * dWE;
    }

    bool operator== (t other) const { return idx == other.idx; }
    bool operator!= (t other) const { return idx != other.idx; }

    bool in_range ()          const { return idx < v::cnt; }
    void next ()                    { idx++; }


    void check ()             const { assertc (v_ac, idx < cnt); }

    coord::t row () { return idx / dNS - 1; }

    coord::t col () { return idx % dNS - 1; }

    bool is_on_board () { // TODO Ho here
      check (); // TODO remove here and there
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

    t N () { return t (idx - dNS); }
    t W () { return t (idx - dWE); }
    t E () { return t (idx + dWE); }
    t S () { return t (idx + dNS); }

    t NW () { return N ().W (); } // TODO can it be faster?
    t NE () { return N ().E (); } // only Go
    t SW () { return S ().W (); } // only Go
    t SE () { return S ().E (); }

    string to_string () {
      coord::t r;
      coord::t c;
      check ();
    
      if (idx == pass_idx) {
        return "PASS";
      } else if (idx == no_v_idx) {
        return "NO_V";
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

  };
  
  const t pass    = t (pass_idx);
  const t no_v    = t (no_v_idx);
  const t resign  = t (resign_idx); // TODO is it inlined (x2)?


  template <typename elt_t> class map_t {
  public:
    elt_t tab [v::cnt];
    elt_t& operator[] (t v) { return tab [v.idx]; }
    const elt_t& operator[] (t v) const { return tab [v.idx]; }
  };

}

#define v_for_each_all(vv) for (v::t vv = 0; vv.in_range (); vv.next ())

#define v_for_each_onboard(vv) \
  for (v::t vv = v::dNS+v::dWE; vv.idx <= board_size * (v::dNS + v::dWE); vv.next ())


#ifdef Ho

  #define v_for_each_nbr(center_v, nbr_v, block) {  \
    center_v.check_is_on_board ();                  \
    v::t nbr_v;                                     \
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
    v::t nbr_v;                                     \
    nbr_v = center_v.N (); block;                   \
    nbr_v = center_v.W (); block;                   \
    nbr_v = center_v.E (); block;                   \
    nbr_v = center_v.S (); block;                   \
  }
    
  #define v_for_each_diag_nbr(center_v, nbr_v, block) {  \
    center_v.check_is_on_board ();                       \
    v::t nbr_v;                                          \
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
    player::check (player::t (move >> v::bits_used));
    v::t (move & ((1 << v::bits_used) - 1)).check ();
  }

  uint player_mask [player::cnt] = { 
    player::black << v::bits_used, 
    player::white << v::bits_used 
  };

  t of_pl_v (player::t player, v::t v) { 
    player::check (player);
    v.check ();
    return player_mask [player] | v.idx;
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
    return ::player::to_string (player (move)) + " " + v (move).to_string ();
  }

}

#define move_for_each_all(m) for (move::t m = 0; m < move::cnt; m++)
