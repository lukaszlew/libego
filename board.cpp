/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                           *
 *  This file is part of GoBoard library,                                    *
 *                                                                           *
 *  Copyright 2006, 2007 Lukasz Lew                                          *
 *                                                                           *
 *  GoBoard library is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by     *
 *  the Free Software Foundation; either version 2 of the License, or        *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  GoBoard library is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public License        *
 *  along with GoBoard library; if not, write to the Free Software           *
 *  Foundation, Inc., 51 Franklin St, Fifth Floor,                           *
 *  Boston, MA  02110-1301  USA                                              *
 *                                                                           *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


// constants


const uint board_size        = 9;

const uint board_area        = board_size * board_size;
const uint max_empty_v_cnt   = board_area;
const uint max_game_length   = board_area * 4;

#ifdef NDEBUG
const bool player_ac          = false;
const bool color_ac           = false;
const bool coord_ac           = false;
const bool v_ac               = false;
const bool nbr_cnt_ac         = false;


const bool chain_ac           = false;
const bool board_empty_v_ac   = false;
const bool board_hash_ac      = false;
const bool board_color_at_ac  = false;
const bool board_nbr_cnt_ac   = false;
const bool chain_at_ac        = false;
const bool chain_next_v_ac    = false;
const bool chains_ac          = false;
const bool board_ac           = false;
#endif

#ifdef DEBUG
const bool paranoic           = true;

const bool player_ac          = paranoic;
const bool color_ac           = paranoic;
const bool coord_ac           = paranoic;
const bool v_ac               = paranoic;
const bool nbr_cnt_ac         = paranoic;

const bool chain_ac           = paranoic;
const bool board_empty_v_ac   = paranoic;
const bool board_hash_ac      = paranoic;
const bool board_color_at_ac  = paranoic;
const bool board_nbr_cnt_ac   = paranoic;
const bool chain_at_ac        = paranoic;
const bool chain_next_v_ac    = paranoic;
const bool chains_ac          = paranoic;
const bool board_ac           = true;
#endif

// namespace player


namespace player {

  enum t {
    black = 0,
    white = 1
  };

  const uint cnt = 2;

  static void check (t player) { 
    unused (player);            // TODO why unused is needed ?
    assertc (player_ac, (player & (~1)) == 0);
  }

  t other (t player) { 
    check (player);
    return (t)(player ^ 1);
  }
  
  void print (t player, ostream& out) {
    check (player);
    if (player == black)
      out << "#";
    else
      out << "O";
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
    edge  = 3
  };

  const uint cnt = 4;

  static void check (t color) { 
    unused (color);
    assertc (color_ac, (color & (~3)) == 0); 
  }

  static bool is_player (t color) { return color <= color::white; } // & (~1)) == 0; }
  static bool is_not_player (t color) { return color > color::white; } // & (~1)) == 0; }

  static t opponent (player::t player) {
    unused (opponent);          // avoids warning
    unused (player);
    player::check (player);
    return t (player ^ 1);
  }

  static char to_char (t color) { 
    check (color);
    switch (color) {
    case black: return '#';
    case white: return 'O';
    case empty: return '.';
    case edge:  return '*';
    default : assertc (color_ac, false);
    }
    return '?';                 // should not happen
  }

  const t wrong_char = t(4);

  static t of_char (char c) {  // may return t(4)
     switch (c) {
     case '#': return black;
     case 'O': return white;
     case '.': return empty;
     case '*': return edge;
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

  

  static void check (t coo) { 
    unused (coo);
    assertc (coord_ac, is_ok (coo)); 
  }

  static void check2 (t row, t col) { 
    if (!coord_ac) return;
    if (row == -1 && col == -1) return;
    assertc (coord_ac, is_ok (row)); 
    assertc (coord_ac, is_ok (col)); 
  }

  static char col_tab[20] = "ABCDEFGHJKLMNOPQRST";

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
  
  const t pass = 0;
  const t no_v = 1;

  static void check (t v) { 
    unused (v);
    assertc (v_ac, v < cnt); 
  }

  static coord::t row (t v) {
    check (v);
    return v / dNS - 1;
  }

  static coord::t col (t v) {
    check (v);
    return v % dNS - 1;
  }

  static bool is_on_board (t v) {
    check (v);
    return coord::is_ok (row (v)) & coord::is_ok (col (v)); // TODO is && faster here ?
  }

  static void check_is_on_board (t v) { 
    unused (v);
    assertc (v_ac, is_on_board (v)); 
  }

  static t N (t v) { check (v); check (v - dNS); return v - dNS; }
  static t W (t v) { check (v); check (v - dWE); return v - dWE; }
  static t E (t v) { check (v); check (v + dWE); return v + dWE; }
  static t S (t v) { check (v); check (v + dNS); return v + dNS; }

  static t NW (t v) { check (v); return N(W(v)); }
  static t NE (t v) { check (v); return N(E(v)); }
  static t SW (t v) { check (v); return S(W(v)); }
  static t SE (t v) { check (v); return S(E(v)); }

  static t of_rc (coord::t r, coord::t c) {
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

#define v_for_each_faster(vv) \
  for (v::t vv = v::dNS+v::dWE; vv <= board_size * (v::dNS + v::dWE); vv++)

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

#define pl_v_for_each(pl, vv) player_for_each(pl) v_for_each_faster(vv)

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

  static t of_pl_v (player::t player, v::t v) { 
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

/*
#define move_for_each_and_pass(m, i) do {        \
  move::t m;                                     \
  player_for_each (pl) {                         \
    m = move::of_pl_v (pl, v::pass);             \
    i;                                           \
  }                                              \
  pl_v_for_each (pl, v) {                        \
    m = move::of_pl_v (pl, v);                   \
    i;                                           \
  }                                              \
} while (false)
*/
// namespace hash


namespace hash {

  typedef uint64 t;

  uint index (t hash) { return hash; }
  uint lock  (t hash) { return hash >> 32;  }

  t combine (t h1, t h2) { return h1 + h2; } // not xor. + to allow h1+h2+h1

  t random () { 
    return (t)
      (uint64 (pm::rand_int ()) << (0*16)) ^
      (uint64 (pm::rand_int ()) << (1*16)) ^ 
      (uint64 (pm::rand_int ()) << (2*16)) ^ 
      (uint64 (pm::rand_int ()) << (3*16));
  }
  
}


// class zobrist_t


class zobrist_t {
public:

  hash::t hashes[move::cnt];

  zobrist_t () {
    pl_v_for_each (pl, v) {
      move::t m;
      m = move::of_pl_v (pl, v);
      hashes[m] = hash::random ();
    }
  }

  hash::t of_move (move::t m) const {
    move::check (m);
    return hashes[m];
  }

  hash::t of_pl_v (player::t pl,  v::t v) const {
    player::check (pl);
    v::check(v);
    return hashes[move::of_pl_v (pl, v)];
  }

};


// namespace nbr_cnt

namespace nbr_cnt {

  typedef uint t;

  const uint max = 4;                 // maximal number of neighbours

  const uint f_size = 4;              // size in bits of each of 3 counters in nbr_cnt::t
  const static uint f_shift [3] = { 0*f_size, 1*f_size, 2*f_size };
  const uint f_mask = (1 << f_size) - 1;

  uint empty_cnt  (t nbr_cnt) { 
    return nbr_cnt >> f_shift [color::empty];
  }

  uint player_cnt (t nbr_cnt, player::t pl) { 
    return (nbr_cnt >> f_shift [pl]) & f_mask; 
  }

  const uint player_cnt_is_max_mask [player::cnt] = { 
    (max << f_shift [player::black]), 
    (max << f_shift [player::white]) 
  };

  uint player_cnt_is_max (t nbr_cnt, player::t pl) { 
    return (nbr_cnt & player_cnt_is_max_mask [pl]) == player_cnt_is_max_mask [pl]; 
  }

  void check (t nbr_cnt) {
    unused (nbr_cnt);
    if (!nbr_cnt_ac) return;
    assert (empty_cnt (nbr_cnt) <= max);
    assert (player_cnt (nbr_cnt, player::black) <= max);
    assert (player_cnt (nbr_cnt, player::white) <= max);
  }

  t of_desc (uint black_cnt, uint white_cnt, uint empty_cnt) {
    assertc (nbr_cnt_ac, black_cnt <= max);
    assertc (nbr_cnt_ac, white_cnt <= max);
    assertc (nbr_cnt_ac, empty_cnt <= max);

    return 
      (black_cnt << f_shift [player::black]) +
      (white_cnt << f_shift [player::white]) +
      (empty_cnt << f_shift [color::empty]);
  }
  
  const t player_inc_tab [player::cnt] = { 
    (1 << f_shift [player::black]) - (1 << f_shift [color::empty]), 
    (1 << f_shift [player::white]) - (1 << f_shift [color::empty]) 
  };

  t player_inc (player::t pl) { return player_inc_tab[pl]; }
  
  const t edge_inc = 
    + (1 << f_shift [player::black]) 
    + (1 << f_shift [player::white]) 
    - (1 << f_shift [color::empty]);

}


// class chain_t


class chain_t {                 // find - union algorithm, with pseudo liberties
public:

  union {
    chain_t* parent;
    int lib_cnt2;               // pseudo_liberties * 2 + 1
  };

  bool is_root () const { return lib_cnt2 & 0x1; }

  void check_is_root () const { assertc (chain_ac, is_root ()); }

  void init (uint lib_cnt) {
    assertc (chain_ac, lib_cnt <= nbr_cnt::max);
    lib_cnt2 = lib_cnt << 1 | 0x1;
  }

  chain_t* find_root () {
    chain_t* node = this;
    if (node->is_root ()) return node;
    while (!node->parent->is_root()) 
      node->parent = node->parent->parent; // nice path compression
    return node->parent;
  }

  const chain_t* find_root_npc () const { // no path compression
    const chain_t* node = this;
    while (!node->is_root()) node = node->parent;
    return node;
  }

  chain_t* join (chain_t* root2) {
    this->check_is_root ();
    root2->check_is_root ();

    if (this->lib_cnt2 < root2->lib_cnt2) { // no test at all results in slight inefficiency
      root2->lib_cnt2 += this->lib_cnt2 - 1;
      this->parent = root2;
      return root2;
    } else {
      this->lib_cnt2 += root2->lib_cnt2 - 1;
      root2->parent = this;
      return this;
    }
  }

  bool lib_cnt_is_zero () const {
    check_is_root ();
    return lib_cnt2 == 1;
  }

  uint lib_cnt () const {              // inefficient
    check_is_root ();
    return lib_cnt2 >> 1;
  }

  void inc_lib_cnt () {
    check_is_root ();
    lib_cnt2 += 0x2;
  }
  
  void dec_lib_cnt () {
    check_is_root ();
    lib_cnt2 -= 0x2;
    assertc (chain_ac, (lib_cnt2 >> 1) >= 0);
  }

  void update_pointer (int chain_t_delta) { // in bytes
    if (!is_root ()) parent = parent + chain_t_delta;
  }

};


// class board_t

enum play_ret_t { play_ok, play_suicide, play_ss_suicide };

const static zobrist_t zobrist[1]; // TODO move it to board

class board_t {
  
public:

  color::t    color_at      [v::cnt];
  chain_t     chain_at      [v::cnt];
  v::t        chain_next_v  [v::cnt];
  nbr_cnt::t  nbr_cnt       [v::cnt]; // incremental, for fast eye checking
  
  uint        empty_pos     [v::cnt];
  v::t        empty_v       [board_area];
  uint        empty_v_cnt;
  uint        last_empty_v_cnt;

  uint        player_v_cnt  [player::cnt];

  hash::t     hash;
  int         komi;

  int         padding;

public:                         // macros

  #define empty_v_for_each(board, v, i) {                               \
    v::t v;                                                             \
    rep (ev_i, (board)->empty_v_cnt) {                                  \
      v = (board)->empty_v [ev_i];                                      \
      i;                                                                \
    }                                                                   \
  }
  
  #define empty_v_for_each_and_pass(board, v, i) {                      \
    v::t v;                                                             \
    v = v::pass;                                                        \
    i;                                                                  \
    rep (ev_i, (board)->empty_v_cnt) {                                  \
      v = (board)->empty_v [ev_i];                                      \
      i;                                                                \
    }                                                                   \
  }

public:                         // consistency checks

  void check_empty_v () const {
    if (!board_empty_v_ac) return;

    bool noticed[v::cnt];
    uint exp_player_v_cnt [player::cnt];

    v_for_each_all (v) noticed[v] = false;

    assert (empty_v_cnt <= board_area);

    empty_v_for_each (this, v, {
      assert (noticed [v] == false);
      noticed [v] = true;
    });

    player_for_each (pl) exp_player_v_cnt [pl] = 0;

    v_for_each_faster (v) {
      assert ((color_at[v] == color::empty) == noticed[v]);
      if (color_at[v] == color::empty) {
        assert (empty_pos[v] < empty_v_cnt);
        assert (empty_v [empty_pos[v]] == v);
      }
      if (color::is_player (color_at [v])) exp_player_v_cnt [color_at[v]]++;
    }

    player_for_each (pl) 
      assert (exp_player_v_cnt [pl] == player_v_cnt [pl]);
  }

  void check_hash () const {
    assertc (board_hash_ac, hash == recalc_hash ());
  }

  void check_color_at () const {
    if (!board_color_at_ac) return;

    v_for_each_all (v) {
      color::check (color_at[v]);
      assert ((color_at[v] != color::edge) == (v::is_on_board (v)));
    }
  }

  void check_nbr_cnt () const {
    if (!board_nbr_cnt_ac) return;
    
    v_for_each_faster (v) {
      coord::t r;
      coord::t c;
      uint nbr_color_cnt[color::cnt];
      uint expected_nbr_cnt;

      if (color_at[v] == color::edge) continue; // TODO is that right?

      r = v::row (v);
      c = v::col (v);

      assert (coord::is_ok (r)); // checking the macro
      assert (coord::is_ok (c));
          
      color_for_each (col) nbr_color_cnt [col] = 0;
          
      v_for_each_nbr (v, nbr_v, nbr_color_cnt [color_at [nbr_v]]++);
          
      expected_nbr_cnt =        // definition of nbr_cnt[v]
        + ((nbr_color_cnt [color::black] + nbr_color_cnt [color::edge]) 
           << nbr_cnt::f_shift [player::black])
        + ((nbr_color_cnt [color::white] + nbr_color_cnt [color::edge])
           << nbr_cnt::f_shift [player::white])
        + ((nbr_color_cnt [color::empty]) 
           << nbr_cnt::f_shift [color::empty]);
    
      assert (nbr_cnt[v] == expected_nbr_cnt);
    }
  }

  void check_chain_at () const {
    if (!chain_at_ac) return;

    v_for_each_faster (v) { // whether same color neighbours have same root and liberties
      // TODO what about edge and empty?
      if (color::is_player (color_at[v])) {

        assert (!chain_at[v].find_root_npc()->lib_cnt_is_zero ());

        if (color_at[v] == color_at[v::S (v)]) 
          assert (chain_at[v].find_root_npc () == 
                  chain_at[v::S (v)].find_root_npc ());

        if (color_at[v] == color_at[v::E (v)]) 
          assert (chain_at[v].find_root_npc () == 
                  chain_at[v::E (v)].find_root_npc ());

      }
    }
  }

  void check_chain_next_v () const {
    if (!chain_next_v_ac) return;
    v_for_each_all (v) {
      v::check (chain_next_v[v]);
      if (!color::is_player (color_at[v])) 
        assert (chain_next_v[v] == v);
    }
  }

  void check_chains () const {        // ... and chain_next_v
    uint            act_chain_no;
    const chain_t*  chain_root[v::cnt - 1]; // list; could be smaller
    uint            chain_no[v::cnt]; // 

    const uint      no_chain = 100;

    if (!chains_ac) return;
    
    act_chain_no = 0;

    v_for_each_all (v) chain_no[v] = no_chain;

    // TODO what about empty and edge?
    v_for_each_faster (v) {
      if (color::is_player(color_at[v]) && chain_no[v] == no_chain) { // chain not visited yet
        color::t        act_color;
        const chain_t*  act_root;
        
        uint lib_cnt;
        uint forward_edge_cnt;
        uint backward_edge_cnt;
        
        chain_root[act_chain_no] = chain_at[v].find_root_npc ();
        act_root = chain_root[act_chain_no];
        
        rep (ch_no, act_chain_no) 
          assert (chain_root[ch_no] != act_root); // separate chains, separate roots
        
        act_color          = color_at[v]; 
        lib_cnt            = 0;
        forward_edge_cnt   = 0;
        backward_edge_cnt  = 0;
        
        v::t act_v = v;
        do {
          assert (color_at[act_v] == act_color);
          assert (chain_at[act_v].find_root_npc () == act_root);
          assert (chain_no[act_v] == no_chain);
          chain_no[act_v] = act_chain_no;
          
          v_for_each_nbr (act_v, nbr_v, {
            if (color_at[nbr_v] == color::empty) lib_cnt++;
            if (color_at[nbr_v] == act_color) {
              if (chain_no[nbr_v] == act_chain_no) forward_edge_cnt++; 
              else {
                assert (chain_no[nbr_v] == no_chain);
                backward_edge_cnt++;
              }
            }
            
          });
          
          act_v = chain_next_v[act_v];
        } while (act_v != v);
        
        assert (forward_edge_cnt == backward_edge_cnt);
        assert (act_root->lib_cnt () == lib_cnt);
        v::check_is_on_board (act_root - chain_at);
        assert (chain_no[act_root - chain_at] == act_chain_no); // root is a part of the chain
        
        act_chain_no++;
      }
    }
  }
  
  void check () const {
    if (!board_ac) return;

    check_empty_v       ();
    check_hash          ();
    check_color_at      ();
    check_nbr_cnt       ();
    check_chain_at      ();
    check_chain_next_v  ();
    check_chains        ();

    if (color_at[0] == 11) print_cerr (); // TODO LOL hack - need tu use it somwhere 

  }

  void check_no_more_legal (player::t player) { // at the end of the playout
    unused (player);

    if (!board_ac) return;

    v_for_each_faster (v)
      if (color_at[v] == color::empty)
        assert (is_eyelike(player, v) || play(player, v) == play_ss_suicide);
  }


public:                         // board interface


  board_t () { clear (); }
  
  board_t (const board_t* to_copy) { load (to_copy); }

  void clear () {
    uint edge_cnt;

    set_komi (7.5);            // standard for chinese rules
    empty_v_cnt = 0;
    player_for_each (pl) player_v_cnt [pl] = 0;

    v_for_each_all (v) {
      color_at      [v] = color::edge;
      nbr_cnt       [v] = nbr_cnt::of_desc (0, 0, nbr_cnt::max);
      chain_next_v  [v] = v;

      if (v::is_on_board (v)) {
        color_at   [v]              = color::empty;
        empty_pos  [v]              = empty_v_cnt;
        empty_v    [empty_v_cnt++]  = v;

        edge_cnt = 0;
        v_for_each_nbr (v, nbr_v, if (!v::is_on_board (nbr_v)) edge_cnt++);
        nbr_cnt [v] += edge_cnt * nbr_cnt::edge_inc;
      }
      
      (chain_at+v)->init (nbr_cnt::empty_cnt (nbr_cnt [v]));
    }

    hash = recalc_hash ();

    check ();
  }

  hash::t recalc_hash () const {
    hash::t new_hash;

    new_hash = 0;
    v_for_each_faster (v) {
      if (color::is_player (color_at[v])) {
        new_hash ^= zobrist->of_pl_v (player::t (color_at[v]), v);
      }
    }
    
    return new_hash;
  }

  void load (const board_t* save_board) { 
    memcpy(this, save_board, sizeof(board_t)); 
    long delta;

    assertc (chain_ac, (((char*) (this) - (char*) (save_board)) & 0x3) == 0);
    delta = (chain_t*) (this) - (chain_t*) (save_board); // TODO do something about this hack

    v_for_each_faster (v) {
      assertc (board_ac, 
               (chain_at+v)->parent + delta 
               == 
               chain_at + ((save_board->chain_at + v)->parent - save_board->chain_at)
               );

      chain_at[v].update_pointer (delta);
    }

    check ();
  }
  
  void set_komi (float fkomi) { 
    komi = int (ceil (-fkomi));
  }

  float get_komi () const { 
    return float(-komi) + 0.5;
  }

  play_ret_t play (player::t player, v::t v) { 
    if (v == v::pass) return play_ok;
    return play_no_pass (player, v);
  }

  play_ret_t play_no_pass (player::t player, v::t v) {
    check ();
    player::check (player);
    v::check_is_on_board (v);
    assertc (board_ac, color_at[v] == color::empty);

    if (nbr_cnt::player_cnt_is_max (nbr_cnt[v], player::other (player)))
      return play_eye (player, v);
    else 
      return play_no_eye (player, v);
  }

  play_ret_t play_no_eye (player::t player, v::t v) {
    chain_t* new_chain_root;

    check ();
    player::check (player);
    v::check_is_on_board (v);
    assertc (board_ac, color_at[v] == color::empty);

    last_empty_v_cnt = empty_v_cnt;
    new_chain_root = place_stone (player, v);

    v_for_each_nbr (v, nbr_v, process_new_nbr (nbr_v, player, v, &new_chain_root));

    assertc (board_ac, new_chain_root == chain_at[v].find_root_npc ());
             
    if (new_chain_root->lib_cnt_is_zero ()) {
      assertc (board_ac, last_empty_v_cnt - empty_v_cnt == 1);
      remove_chain(v);
      assertc (board_ac, last_empty_v_cnt - empty_v_cnt > 0);
      return play_suicide;
    }

    return play_ok;
  }

  play_ret_t play_eye (player::t player, v::t v) {
    chain_t* chain_root_N;      // TODO macro !!!
    chain_t* chain_root_W;
    chain_t* chain_root_S;
    chain_t* chain_root_E;

    assertc (board_ac, color_at[v::N (v)] == color::opponent (player) || color_at[v::N (v)] == color::edge);
    assertc (board_ac, color_at[v::W (v)] == color::opponent (player) || color_at[v::W (v)] == color::edge);
    assertc (board_ac, color_at[v::E (v)] == color::opponent (player) || color_at[v::E (v)] == color::edge);
    assertc (board_ac, color_at[v::S (v)] == color::opponent (player) || color_at[v::S (v)] == color::edge);

    chain_root_N = (chain_at + v::N(v))->find_root ();
    chain_root_W = (chain_at + v::W(v))->find_root ();
    chain_root_E = (chain_at + v::E(v))->find_root ();
    chain_root_S = (chain_at + v::S(v))->find_root ();

    chain_root_N->dec_lib_cnt ();
    chain_root_W->dec_lib_cnt ();
    chain_root_E->dec_lib_cnt ();
    chain_root_S->dec_lib_cnt ();

    if ((!chain_root_N->lib_cnt_is_zero ()) &
        (!chain_root_W->lib_cnt_is_zero ()) & 
        (!chain_root_E->lib_cnt_is_zero ()) & 
        (!chain_root_S->lib_cnt_is_zero ())) 
    {
      chain_root_N->inc_lib_cnt ();
      chain_root_W->inc_lib_cnt ();
      chain_root_E->inc_lib_cnt ();
      chain_root_S->inc_lib_cnt ();
      return play_ss_suicide;
    }

    last_empty_v_cnt = empty_v_cnt;

    place_stone (player, v);
    
    nbr_cnt[v::N(v)] += nbr_cnt::player_inc (player);
    nbr_cnt[v::W(v)] += nbr_cnt::player_inc (player);
    nbr_cnt[v::E(v)] += nbr_cnt::player_inc (player);
    nbr_cnt[v::S(v)] += nbr_cnt::player_inc (player);

    if (chain_root_N->lib_cnt_is_zero ()) remove_chain(v::N(v));
    if (chain_root_W->lib_cnt_is_zero ()) remove_chain(v::W(v));
    if (chain_root_E->lib_cnt_is_zero ()) remove_chain(v::E(v));
    if (chain_root_S->lib_cnt_is_zero ()) remove_chain(v::S(v));

    assertc (board_ac, !chain_at[v].find_root_npc ()->lib_cnt_is_zero ());

    return play_ok;
  }

  void process_new_nbr(v::t v, 
                       player::t new_nbr_player,
                       v::t new_nbr_v, 
                       chain_t** new_nbr_chain_root)
  {
    chain_t* chain_root;
    
    nbr_cnt[v] += nbr_cnt::player_inc (new_nbr_player);

    if (color::is_not_player (color_at [v])) return;

    chain_root = (chain_at + v)->find_root ();
    
    if (color_at[v] == color::t (new_nbr_player)) { // same color of groups
      chain_root->dec_lib_cnt ();
      if (chain_root != *new_nbr_chain_root) { // not merged yet
        *new_nbr_chain_root = chain_root->join (*new_nbr_chain_root);
        swap (chain_next_v[v], chain_next_v[new_nbr_v]);
      }
    } else {
      chain_root->dec_lib_cnt ();
      if (chain_root->lib_cnt_is_zero ()) remove_chain(v);
    }
  }
  
  void remove_chain (v::t v) {
    v::t act_v;
    v::t tmp_v;
    color::t old_color;

    old_color = color_at[v];
    act_v = v;

    assertc (board_ac, color::is_player (old_color));

    do {
      remove_stone (act_v);
      act_v = chain_next_v[act_v];
    } while (act_v != v);

    assertc (board_ac, act_v == v);

    do {
      v_for_each_nbr (act_v, nbr_v, {
        nbr_cnt[nbr_v] -= nbr_cnt::player_inc (player::t (old_color));
        (chain_at+nbr_v)->find_root ()->inc_lib_cnt (); 
      });

      tmp_v = act_v;
      act_v = chain_next_v[act_v];
      chain_next_v[tmp_v] = tmp_v;
      
    } while (act_v != v);
  }

  chain_t* place_stone (player::t pl, v::t v) {
    hash ^= zobrist->of_pl_v (pl, v);
    player_v_cnt[pl]++;
    color_at[v] = color::t (pl);

    empty_v_cnt--;
    empty_pos [empty_v [empty_v_cnt]] = empty_pos [v];
    empty_v [empty_pos [v]] = empty_v [empty_v_cnt];

    assertc (chain_next_v_ac, chain_next_v[v] == v);

    (chain_at+v)->init (nbr_cnt::empty_cnt (nbr_cnt[v]));

    return chain_at+v;
  }

  void remove_stone (v::t v) {
    hash ^= zobrist->of_pl_v (player::t(color_at[v]), v);
    player_v_cnt [color_at[v]]--;
    color_at[v] = color::empty;

    empty_pos [v] = empty_v_cnt;
    empty_v [empty_v_cnt++] = v;

    assertc (board_ac, empty_v_cnt < v::cnt);
  }

public:                         // utils

  bool is_eyelike (player::t player, v::t v) { 
    int diag_color_cnt[color::cnt];

    assertc (board_ac, color_at [v] == color::empty);

    if (! nbr_cnt::player_cnt_is_max (nbr_cnt[v], player)) return false;

    color_for_each (col) diag_color_cnt [col] = 0; // memset is slower
    v_for_each_diag_nbr (v, diag_v, {
      diag_color_cnt [color_at [diag_v]]++;
    });

    diag_color_cnt[player::other (player)] += (diag_color_cnt[color::edge] > 0);
    return diag_color_cnt[player::other (player)] < 2;
  }

  int approx_score () const {
    return komi + player_v_cnt[player::black] -  player_v_cnt[player::white];
  }

  player::t approx_winner () { return player::t (approx_score () <= 0); }

  int score () const {
    int eye_score;

    eye_score = 0;

    empty_v_for_each (this, v, {
      if (nbr_cnt::player_cnt_is_max (nbr_cnt[v], player::black)) {
        eye_score++;
      } else if (nbr_cnt::player_cnt_is_max (nbr_cnt[v], player::white)) {
        eye_score--;
      }
    });

    return approx_score () + eye_score;
  }

  player::t winner () const { 
    return player::t (score () <= 0); 
  }

  void print (ostream& out, v::t mark_v = v::pass) const { // TODO defoult out
    #define os(n) out << " " << n << " ";
    #define om(n) out << "(" << n << ")";
    
    out << "   ";
    coord_for_each (col) os (coord::col_to_string (col));
    out << endl;

    coord_for_each (row) {
      os (coord::row_to_string (row));
      coord_for_each (col) {
        v::t v = v::of_rc (row, col);
        char ch = color::to_char (color_at[v]);
        if (v == mark_v)  om (ch) 
        else              os (ch);
      }
      os (coord::row_to_string (row));
      out << endl;
    }
    
    out << "   ";
    coord_for_each (col) os (coord::col_to_string (col));
    out << endl;
  }

  void print_cerr (v::t v = v::pass) const {
    print (cerr, v);
  }

  bool load (istream& ifs) {
    uint       bs;
    char c;

    player::t  play_player[board_area];
    v::t       play_v[board_area];
    uint       play_cnt;

    clear ();

    play_cnt = 0;

    if (!ifs) return false;

    ifs >> bs;
    if (bs != board_size) return false;

    if (getc_non_space (ifs) != '\n') return false;

    coord_for_each (row) {
      coord_for_each (col) {
        color::t pl;

        c = getc_non_space (ifs);
        pl = color::of_char (c);
        if (pl == color::wrong_char) return false;
        
        if (color::is_player (pl)) {
          play_player[play_cnt] = player::t (pl);
          play_v[play_cnt] = v::of_rc (row, col);
          play_cnt++;
          assertc (board_ac, play_cnt < board_area);
        }
      }

      if (getc_non_space (ifs) != '\n') return false;
    }

    rep (pi, play_cnt) {
      play_ret_t ret;
      ret = play_no_pass (play_player[pi], play_v[pi]);
      if (ret != play_ok || last_empty_v_cnt - empty_v_cnt != 1) {
        cerr << "Fatal error: Illegal board configuration in file." << endl;
        exit (1);               // TODO this is a hack
      }
    }

    return true;
  }

};



#ifdef BOARD_TEST

// class board_test_t

class board_test_t {
  board_t board[1];
  board_t board_arch[1];
  player::t act_player;
  bool interactive;

  v::t rnd_empty_v () {
    v::t v;
    do {
      v = pm::rand_int () % v::cnt; 
      if (interactive) cout << ".";
    } while (board->color_at[v] != color::empty || 
             board->is_eyelike (act_player, v));
    if (interactive) cout << endl;
    return v;
  }

  v::t play_one () {
    v::t v;
    v = rnd_empty_v ();
    board->play (act_player, v);
    if (interactive) 
      cout << "Last capture size = " 
           << board->empty_v_cnt - board->last_empty_v_cnt 
           << endl 
           << endl;
    act_player = player::other (act_player);
    board->check ();
    return v;
  }

public:

  board_test_t () {
    unused (color::opponent);
    act_player = player::black;
  }

  void print_playout () {
    v::t v;
    interactive = true;

    ifstream fin;

    fin.open ("test_board.brd");

    if (!fin) 
      board->clear ();
    else {
      if (!board->load (fin)) {
        cerr << "bad file" << endl;
        exit (1);
      }
    }

    rep (ii, 180) {
      board->print (cout, v);
      getc (stdin);
      v = play_one ();
    }
    cout << "End of playout" << endl << endl;
  }
  
  void test_1 () {
    interactive = false;
    rep (kk, 30) {

      rep (ii, 500) play_one ();

      board->check ();
      board->score ();
      board->check ();

      board_arch->load (board);
      board_arch->check ();

      rep (ii, 500) play_one ();

      board->check ();
      board->load (board_arch);
      board->check ();

      board->check ();

      cout << endl;

      rep (ii, 500) play_one ();

      board->clear ();
      board->check ();
    }
  }

  void benchmark () {
    board_arch->clear ();
    rep (kk, 100000) {
      board->load (board_arch);
      rep (ii, 110) {
        v::t v;

        do v = pm::rand_int () % v::cnt; 
        while (board->color_at[v] != color::empty); // || board->is_eyelike (act_player, v)); // most expensive loop

        board->play (act_player, v);
        act_player = player::other (act_player);
      }
    }

  }

};

int main () { 
  board_test_t test[1];
  //rep (ii, 100) test->print_playout ();
  test->benchmark ();

  return 0;
}

#endif
