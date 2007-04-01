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


// constants


const uint board_size        = 9;

const uint board_area        = board_size * board_size;
const uint max_empty_v_cnt   = board_area;
const uint max_game_length   = board_area * 4;

#ifdef NDEBUG
const bool paranoic           = false;
const bool board_ac           = false;
#endif

#ifdef DEBUG
const bool paranoic           = false;
const bool board_ac           = false;
// const bool paranoic           = true;
// const bool board_ac           = true;
#endif


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
    edge  = 3
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
    case edge:  return '*';
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

  bool is_on_board (t v) {
    check (v);
    return coord::is_ok (row (v)) & coord::is_ok (col (v));
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
  t NE (t v) { check (v); return N(E(v)); }
  t SW (t v) { check (v); return S(W(v)); }
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

#define pl_v_for_each(pl, vv) player_for_each(pl) v_for_each_onboard(vv)

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
  const uint f_shift [3] = { 0*f_size, 1*f_size, 2*f_size };
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


// class board_t

enum play_ret_t { play_ok, play_suicide, play_ss_suicide, play_ko, play_non_empty };

const zobrist_t zobrist[1]; // TODO move it to board

class board_t {
  
public:

  color::t    color_at      [v::cnt];
  nbr_cnt::t  nbr_cnt       [v::cnt]; // incremental, for fast eye checking
  uint        empty_pos     [v::cnt];
  v::t        chain_next_v  [v::cnt];

  uint        chain_lib_cnt [v::cnt]; // indexed by chain_id
  uint        chain_id      [v::cnt];
  
  v::t        empty_v       [board_area];
  uint        empty_v_cnt;
  uint        last_empty_v_cnt;

  uint        player_v_cnt  [player::cnt];

  hash::t     hash;
  int         komi;

  v::t        ko_v;             // vertex forbidden by ko
  player::t   last_player;      // player who made the last play (other::player is forbidden to retake)

public:                         // macros

  #define empty_v_for_each(board, vv, i) {                              \
    v::t vv;                                                            \
    rep (ev_i, (board)->empty_v_cnt) {                                  \
      vv = (board)->empty_v [ev_i];                                     \
      i;                                                                \
    }                                                                   \
  }
  
  #define empty_v_for_each_and_pass(board, vv, i) {                     \
    v::t vv;                                                            \
    vv = v::pass;                                                       \
    i;                                                                  \
    rep (ev_i, (board)->empty_v_cnt) {                                  \
      vv = (board)->empty_v [ev_i];                                     \
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

    v_for_each_onboard (v) {
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
    
    v_for_each_onboard (v) {
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

    v_for_each_onboard (v) { // whether same color neighbours have same root and liberties
      // TODO what about edge and empty?
      if (color::is_player (color_at[v])) {

        assert (chain_lib_cnt[ chain_id [v]] != 0);

        if (color_at[v] == color_at[v::S (v)]) 
          assert (chain_id [v] == chain_id [v::S (v)]);

        if (color_at[v] == color_at[v::E (v)]) 
          assert (chain_id [v] == chain_id [v::E (v)]);

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
    uint            chain_id_list [v::cnt - 1]; // list; could be smaller
    uint            chain_no [v::cnt]; // 

    const uint      no_chain = 10000;

    if (!chains_ac) return;
    
    act_chain_no = 0;

    v_for_each_all (v) chain_no[v] = no_chain;

    // TODO what about empty and edge?
    v_for_each_onboard (v) {
      if (color::is_player(color_at[v]) && chain_no[v] == no_chain) { // chain not visited yet
        color::t        act_color;

        uint lib_cnt;
        uint forward_edge_cnt;
        uint backward_edge_cnt;

        chain_id_list [act_chain_no] = chain_id [v];

        rep (ch_no, act_chain_no) 
          assert (chain_id_list [ch_no] != chain_id [v]); // separate chains, separate roots
        
        act_color          = color_at[v]; 
        lib_cnt            = 0;
        forward_edge_cnt   = 0;
        backward_edge_cnt  = 0;
        
        v::t act_v = v;
        do {
          assert (color_at[act_v] == act_color);
          assert (chain_id[act_v] == chain_id_list [act_chain_no]);
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
        assert (chain_lib_cnt [chain_id [v]] == lib_cnt);
        
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
  }

  void check_no_more_legal (player::t player) { // at the end of the playout
    unused (player);

    if (!board_ac) return;

    v_for_each_onboard (v)
      if (color_at[v] == color::empty)
        assert (is_eyelike (player, v) || play_no_pass (player, v) >= play_ss_suicide);
  }


public:                         // board interface

  board_t () { 
    clear (); 
    if (color_at[0] == 11) print_cerr (); // TODO LOL hack - need tu use it somwhere 
  }
  
  void clear () {
    uint edge_cnt;

    set_komi (7.5);            // standard for chinese rules
    empty_v_cnt = 0;
    player_for_each (pl) player_v_cnt [pl] = 0;
    ko_v = v::no_v;

    v_for_each_all (v) {
      color_at      [v] = color::edge;
      nbr_cnt       [v] = nbr_cnt::of_desc (0, 0, nbr_cnt::max);
      chain_next_v  [v] = v;
      chain_id      [v] = v;    // TODO is it needed, is it usedt?
      chain_lib_cnt [v] = nbr_cnt::max; // TODO is it logical? (edges)

      if (v::is_on_board (v)) {
        color_at   [v]              = color::empty;
        empty_pos  [v]              = empty_v_cnt;
        empty_v    [empty_v_cnt++]  = v;

        edge_cnt = 0;
        v_for_each_nbr (v, nbr_v, if (!v::is_on_board (nbr_v)) edge_cnt++);
        nbr_cnt [v] += edge_cnt * nbr_cnt::edge_inc;
      }
    }

    hash = recalc_hash ();

    check ();
  }

  hash::t recalc_hash () const {
    hash::t new_hash;

    new_hash = 0;
    v_for_each_onboard (v) {
      if (color::is_player (color_at[v])) {
        new_hash ^= zobrist->of_pl_v (player::t (color_at[v]), v);
      }
    }
    
    return new_hash;
  }

  void load (const board_t* save_board) { 
    memcpy(this, save_board, sizeof(board_t)); 
    check ();
  }
  
  void set_komi (float fkomi) { 
    komi = int (ceil (-fkomi));
  }

  float get_komi () const { 
    return float(-komi) + 0.5;
  }

  flatten all_inline 
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
    check ();
    player::check (player);
    v::check_is_on_board (v);
    assertc (board_ac, color_at[v] == color::empty);

    last_empty_v_cnt = empty_v_cnt;
    ko_v             = v::no_v;
    last_player      = player;

    place_stone (player, v);

    v_for_each_nbr (v, nbr_v, process_new_nbr (nbr_v, player, v));

    if (chain_lib_cnt [chain_id [v]] == 0) {
      assertc (board_ac, last_empty_v_cnt - empty_v_cnt == 1);
      remove_chain(v);
      assertc (board_ac, last_empty_v_cnt - empty_v_cnt > 0);
      return play_suicide;
    }

    return play_ok;
  }

  no_inline
  play_ret_t play_eye (player::t player, v::t v) {
    // TODO test order
    // TODO check (na zmiane dec i if

    v_for_each_nbr (v, nbr_v, assertc (board_ac, color_at[nbr_v] == color::opponent (player) || color_at[nbr_v] == color::edge));

    if (v == ko_v && player == player::other (last_player)) 
      return play_ko;

    uint all_nbr_live = true;
    v_for_each_nbr (v, nbr_v, all_nbr_live &= (--chain_lib_cnt [chain_id [nbr_v]] != 0));

    if (all_nbr_live) {
      v_for_each_nbr (v, nbr_v, chain_lib_cnt [chain_id [nbr_v]]++);
      return play_ss_suicide;
    }

    last_empty_v_cnt = empty_v_cnt;
    last_player      = player;

    place_stone (player, v);
    
    v_for_each_nbr (v, nbr_v, nbr_cnt [nbr_v] += nbr_cnt::player_inc (player));

    v_for_each_nbr (v, nbr_v, if ((chain_lib_cnt [chain_id [nbr_v]] == 0)) remove_chain (nbr_v));

    assertc (board_ac, chain_lib_cnt [chain_id [v]] != 0);

    if (last_empty_v_cnt == empty_v_cnt) { // if captured exactly one stone, end this was eye
      ko_v = empty_v [empty_v_cnt - 1]; // then ko formed
    } else {
      ko_v = v::no_v;
    }

    return play_ok;
  }

  void process_new_nbr(v::t v, 
                       player::t new_nbr_player,
                       v::t new_nbr_v) // TODO moze warto chain id przekazywc do ustalenia?
  {
    nbr_cnt[v] += nbr_cnt::player_inc (new_nbr_player);

    if (color::is_not_player (color_at [v])) return;
    chain_lib_cnt [chain_id [v]] --;

    if (color_at[v] != color::t (new_nbr_player)) { // same color of groups
      if (chain_lib_cnt [chain_id [v]] == 0) remove_chain (v);
      return;
    }

    if (chain_id [v] == chain_id [new_nbr_v]) return;
      
    if (chain_lib_cnt [chain_id [new_nbr_v]] > chain_lib_cnt [chain_id [v]])
      merge_chains (new_nbr_v, v);
    else
      merge_chains (v, new_nbr_v);

  }

  void merge_chains (v::t v_base, v::t v_new) {
    v::t act_v;

    chain_lib_cnt [chain_id [v_base]] += chain_lib_cnt [chain_id [v_new]];

    act_v = v_new;
    do {
      chain_id [act_v] = chain_id [v_base];
      act_v = chain_next_v [act_v];
    } while (act_v != v_new);
    
    swap (chain_next_v[v_base], chain_next_v[v_new]);
  }

  no_inline 
  void remove_chain (v::t v){
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
        chain_lib_cnt [chain_id [nbr_v]]++;
      });

      tmp_v = act_v;
      act_v = chain_next_v[act_v];
      chain_next_v[tmp_v] = tmp_v;
      
    } while (act_v != v);
  }

  void place_stone (player::t pl, v::t v) {
    hash ^= zobrist->of_pl_v (pl, v);
    player_v_cnt[pl]++;
    color_at[v] = color::t (pl);

    empty_v_cnt--;
    empty_pos [empty_v [empty_v_cnt]] = empty_pos [v];
    empty_v [empty_pos [v]] = empty_v [empty_v_cnt];

    assertc (chain_next_v_ac, chain_next_v[v] == v);

    chain_id [v] = v;
    chain_lib_cnt [v] = nbr_cnt::empty_cnt (nbr_cnt[v]);
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

  string to_string (v::t mark_v = v::no_v) const {
    ostringstream out;

    #define os(n)      out << " " << n
    #define o_left(n)  out << "(" << n
    #define o_right(n) out << ")" << n
    
    if (board_size < 10) out << "  "; else out << "   ";
    coord_for_each (col) os (coord::col_to_string (col));
    out << endl;

    coord_for_each (row) {
      if (board_size >= 10 && board_size - row < 10) out << " ";
      os (coord::row_to_string (row));
      coord_for_each (col) {
        v::t v = v::of_rc (row, col);
        char ch = color::to_char (color_at[v]);
        if      (v == mark_v)        o_left  (ch);
        else if (v == v::E(mark_v))  o_right (ch);
        else                         os (ch);
      }
      if (board_size >= 10 && board_size - row < 10) out << " ";
      os (coord::row_to_string (row));
      out << endl;
    }
    
    if (board_size < 10) out << "  "; else out << "   ";
    coord_for_each (col) os (coord::col_to_string (col));
    out << endl;

    return out.str ();
  }

  void print_cerr (v::t v = v::pass) const {
    cerr << to_string (v);
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
