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


class Player { // TODO check is check always checked in constructors

  uint idx;

public:

  const static uint black_idx = 0;
  const static uint white_idx = 1;

  const static uint cnt = 2;


  Player () { idx = -1; }
  Player (uint _idx) { idx = _idx; check ();}

  bool operator== (Player other) const { return idx == other.idx; }

  void check () const { 
    assertc (player_ac, (idx & (~1)) == 0);
  }

  Player other () const { 
    return Player(idx ^ 1);
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
  
  static Player black () { return Player (black_idx); }
  static Player white () { return Player (white_idx); }
};

istream& operator>> (istream& in, Player& pl) {
  string s;
  in >> s;
  if (s == "b" || s == "B" || s == "Black" || s == "BLACK "|| s == "black" || s == "#") { pl = Player::black (); return in; }
  if (s == "w" || s == "W" || s == "White" || s == "WHITE "|| s == "white" || s == "O") { pl = Player::white (); return in; }
  in.setstate (ios_base::badbit);
  return in;
}

ostream& operator<< (ostream& out, Player& pl) { out << pl.to_string (); return out; }

// faster than non-loop
#define player_for_each(pl) \
  for (Player pl = Player::black (); pl.in_range (); pl.next ())

//------------------------------------------------------------------------------
// class color

class Color {
  uint idx;
public:

  const static uint black_idx = 0;
  const static uint white_idx = 1;
  const static uint empty_idx = 2;
  const static uint off_board_idx  = 3;
  const static uint wrong_char_idx = 40;

  const static uint cnt = 4;

  Color () { idx = -1; } // TODO test - remove it

  Color (uint idx_) { idx = idx_; } // TODO test - remove it

  Color (Player pl) { idx = pl.get_idx (); }

  Color (char c) {  // may return color_wrong_char
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

  Player to_player () const { return Player (idx); }

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

  bool in_range () const { return idx < Color::cnt; }
  void next () { idx++; }

  uint get_idx () const { return idx; }
  bool operator== (Color other) const { return idx == other.idx; }
  bool operator!= (Color other) const { return idx != other.idx; }

  static Color black ()      { return Color (black_idx); }
  static Color white ()      { return Color (white_idx); }
  static Color empty ()      { return Color (empty_idx); }
  static Color off_board  () { return Color (off_board_idx); }
  static Color wrong_char () { return Color (wrong_char_idx); }
};

// TODO test it for performance
#define color_for_each(col) \
  for (Color col = Color::black (); col.in_range (); col.next ())

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



class Vertex {


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

  Vertex () { } // TODO is it needed
  Vertex (uint _idx) { idx = _idx; }

 // TODO make this constructor a static function
  Vertex (coord::t r, coord::t c) {
    coord::check2 (r, c);
    idx = (r+1) * dNS + (c+1) * dWE;
  }

  uint get_idx () const { return idx; }

  bool operator== (Vertex other) const { return idx == other.idx; }
  bool operator!= (Vertex other) const { return idx != other.idx; }

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

  Vertex N () const { return Vertex (idx - dNS); }
  Vertex W () const { return Vertex (idx - dWE); }
  Vertex E () const { return Vertex (idx + dWE); }
  Vertex S () const { return Vertex (idx + dNS); }

  Vertex NW () const { return N ().W (); } // TODO can it be faster?
  Vertex NE () const { return N ().E (); } // only Go
  Vertex SW () const { return S ().W (); } // only Go
  Vertex SE () const { return S ().E (); }

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

  static Vertex pass   () { return Vertex (Vertex::pass_idx); }
  static Vertex any    () { return Vertex (Vertex::any_idx); }
  static Vertex resign () { return Vertex (Vertex::resign_idx); }

  static Vertex of_sgf_coords (string s) {
    if (s == "") return pass ();
    if (s == "tt" && board_size <= 19) return pass ();
    if (s.size () != 2 ) return any ();
    coord::t col = s[0] - 'a';
    coord::t row = s[1] - 'a';
    
    if (coord::is_on_board (row) &&
        coord::is_on_board (col)) {
      return Vertex (row, col);
    } else {
      return any ();
    }
  }
};

// TODO of_gtp_string
istream& operator>> (istream& in, Vertex& v) {
  char c;
  int n;
  coord::t row, col;

  string str;
  if (!(in >> str)) return in;
  if (str == "pass" || str == "PASS" || str == "Pass") { v = Vertex::pass (); return in; }
  if (str == "resign" || str == "RESIGN" || str == "Resign") { v = Vertex::resign (); return in; }

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

  v = Vertex (row, col);
  return in;
}

ostream& operator<< (ostream& out, Vertex& v) { out << v.to_string (); return out; }


#define vertex_for_each_all(vv) for (Vertex vv = 0; vv.in_range (); vv.next ()) // TODO 0 works??? // TODO player the same way!

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


//--------------------------------------------------------------------------------

class Move {
public:

  const static uint cnt = Player::white_idx << Vertex::bits_used | Vertex::cnt;
 
  const static uint no_move_idx = 1;

  uint idx;

  void check () {
    Player (idx >> Vertex::bits_used);
    Vertex (idx & ((1 << Vertex::bits_used) - 1)).check ();
  }

  Move (Player player, Vertex v) { 
    idx = (player.get_idx () << Vertex::bits_used) | v.get_idx ();
  }

  Move () {
    Move (Player::black (), Vertex::any ());
  }

  Move (int idx_) {
    idx = idx_;
  }

  Player get_player () { 
    return Player (idx >> Vertex::bits_used);
  }

  Vertex get_vertex () { 
    return Vertex (idx & ((1 << ::Vertex::bits_used) - 1)) ; 
  }

  string to_string () {
    return get_player ().to_string () + " " + get_vertex ().to_string ();
  }

  uint get_idx () { return idx; }

  bool operator== (Move other) const { return idx == other.idx; }
  bool operator!= (Move other) const { return idx != other.idx; }

  bool in_range ()          const { return idx < cnt; }
  void next ()                    { idx++; }
};


istream& operator>> (istream& in, Move& m) {
  Player pl;
  Vertex v;
  if (!(in >> pl >> v)) return in;
  m = Move (pl, v);
  return in;
}

  
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
    

#define move_for_each_all(m) for (Move m = 0; m.in_range (); m.next ())

/********************************************************************************/
