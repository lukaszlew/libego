/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                           *
 *  This file is part of GoBoard library,                                    *
 *                                                                           *
 *  Copyright 2006 Lukasz Lew                                                *
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

#pragma once

#include "utils.cpp"


// constants


const uint board_size        = 9;

const uint board_area        = board_size * board_size;
const uint max_captured_cnt  = board_area;
const uint max_game_length   = board_area * 4;


const bool player_ac          = true;
const bool color_ac           = true;
const bool coord_ac           = true;
const bool v_ac               = true;

const bool chain_ac           = true;
const bool board_captured_ac  = true;
const bool board_hash_ac      = true;
const bool board_color_at_ac  = true;
const bool board_nbr_cnt_ac   = true;
const bool chain_at_ac        = true;
const bool chains_ac          = true;
const bool board_ac           = true;


// namespace player


namespace player {

  enum t {
    black = 0,
    white = 1
  };

  const uint cnt = 2;

  static void check (t player) { 
    unused (player); // TODO how to fix it?
    assertc (player_ac, (player & (-2)) == 0);
  }

  t other (t player) { 
    check (player);
    return (t)(player ^ 1);
  }
  
  //  t operator++ (t pl) { return t (pl + 1); }
  
}

#define player_for_each(pl) for (player::t pl = player::black; pl != player::cnt; pl = player::t(pl+1))


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
    assertc (color_ac, (color & (-4)) == 0); 
  }

  static bool is_player (t color) { return color & (-2) == 0; }

  static t opponent (t color) {
    assertc (color_ac, is_player (color));
    return t (color ^ 1);
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

//   static t of_char (char c) {  // may return t(4)
//     switch (c) {
//     case '#': return black;
//     case 'O': return white;
//     case '.': return empty;
//     case '*': return edge;
//     default : return t(4);
//     }
//   }

}


// namespace coord


namespace coord {

  typedef int t;

  bool is_ok (t coord) { return uint (coord) < board_size; }

  static void check (t coord) { 
    unused (coord);
    assertc (coord_ac, is_ok (coord)); 
  }

}


// namespace v


namespace v {

  typedef uint t;
  
  const uint cnt = (board_size + 2) * (board_size + 2); // TODO may be reduced a'la Gnu Go
  const uint bits_used = 9;     // on 19x19 cnt == 441 < 512 == 1 << 9;
  //static_assert (cnt <= (1 << bits_used));

  const uint dNS = (board_size + 2);
  const uint dWE = 1;
  
  const t pass = 0;

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

  static t N (t v) { check (v); return v - dNS; }
  static t W (t v) { check (v); return v - dWE; }
  static t E (t v) { check (v); return v + dWE; }
  static t S (t v) { check (v); return v + dNS; }

  static t NW (t v) { check (v); return N(W(v)); }
  static t NE (t v) { check (v); return N(E(v)); }
  static t SW (t v) { check (v); return S(W(v)); }
  static t SE (t v) { check (v); return S(E(v)); }

  static t of_rc (coord::t r, coord::t c) {
    coord::check (r);
    coord::check (c);
    return (r+1) * dNS + (c+1) * dWE;
  }

  void print (t v, ostream& out) {
    coord::t r;
    coord::t c;
    check (v);

    r = row (v);
    c = col (v);
    
    out << "(" << r + 1 << "," << c + 1 << ")";
  }

}

#define v_for_each(v) for (v::t v = 0; v < v::cnt; v++)
/*
//#define v_for_each_on_board(v)                                  \
//   for (v::t v = v::of_rc(0,0); ;)                          \
//     for (coord::t r = 0; r < coord::t (board_size); r++, v+=2)  \
//     for (coord::t c = 0; c < coord::t (board_size); c++, v+=1)
*/
// TODO test it
// TODO inline for each line
// #define v_for_each_on_board(v) v_for_each(v) if (v::is_on_board (v))

#define v_for_each_nbr(center_v, nbr_v, block) {  \
  v::t nbr_v;                                     \
  nbr_v = v::N (center_v);                        \
  block;                                          \
  nbr_v = v::W (center_v);                        \
  block;                                          \
  nbr_v = v::E (center_v);                        \
  block;                                          \
  nbr_v = v::S (center_v);                        \
  block;                                          \
}


// namespace move


namespace move {

  typedef uint t;

  void check (t move) {
    player::check (player::t (move >> v::bits_used));
           v::check (move & ((1 << v::bits_used) - 1));
  }

  t of_pl_v (player::t player, v::t v) { 
    player::check (player);
    v::check (v);
    return (player << v::bits_used) | v;
  }

  const uint cnt = player::white << v::bits_used | v::cnt;// TODO should be of_pl_v (player::white, v::cnt); // TODO should I compress it?
 
  color::t color (t move) { 
    check (move);
    return (color::t) (move >> v::bits_used);
  }

  v::t v (t move) { 
    check (move);
    return move & ((1 << v::bits_used) - 1) ; 
  }

}


// namespace hash


namespace hash {

  typedef uint64 t;

  uint index (t hash) { return hash; }
  uint lock  (t hash) { return hash >> 32;  }

  t combine (t h1, t h2) { return h1 + h2; } // xor -> sum to allow double same // TODO better hashing

  t random () { 
    return (t)
      (uint64 (pm::rand ()) << (0*16)) ^
      (uint64 (pm::rand ()) << (1*16)) ^ 
      (uint64 (pm::rand ()) << (2*16)) ^ 
      (uint64 (pm::rand ()) << (3*16));
  }
  
}
  

// class zobrist_t


class zobrist_t {

  hash::t hashes[move::cnt];

public:

  zobrist_t () {
    player_for_each (pl) {
      v_for_each (v) {
        
        move::t m;
        m = move::of_pl_v (pl, v);
        hashes[m] = hash::random ();
      }
    }
  }

  hash::t of_move (move::t m) const { // TODO const
    move::check (m);
    return hashes[m];
  }

  hash::t of_pl_v (player::t pl,  v::t v) const {
    player::check (pl);
    v::check(v);
    return hashes[move::of_pl_v (pl, v)];
  }

};


// class chain_t


class chain_t;

class chain_t {                 // find - union algorithm, with pseudo liberties
  union {
    chain_t* parent;
    int lib_cnt2;               // pseudo_liberties * 2 + 1
  };

public:

  bool is_root () const { return lib_cnt2 & 0x1; }

  void check_is_root () const { assertc (chain_ac, is_root ()); }

  void init (uint lib_cnt) {
    assertc (chain_ac, lib_cnt <= 4);
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

  void update_pointer (int chain_t_delta) {
    if (!is_root ()) parent += chain_t_delta;
  }

};


// class board_t

const static zobrist_t zobrist[1]; // TODO this shoud be inside board_t


class board_t {
  
public:
  // TODO check other alignment
  color::t  color_at[v::cnt];
  chain_t   chain_at[v::cnt];   // TODO additional layer may speed up
  v::t      chain_next_v[v::cnt];
  uint      nbr_cnt[v::cnt]; // incremental, for fast eye checking // TODO maybe indexing with move::t is better
  
  v::t      captured[max_captured_cnt]; // TODO it is only a buffer, fix it
  uint      captured_cnt;

  hash::t   hash;
  float     komi;


  enum play_ret_t { play_ok, play_suicide, play_ss_suicide };

public:  

  // checks 

  void check_captured () const {
    if (!board_captured_ac) return;
    assert (captured_cnt < max_captured_cnt);
    rep (ii, captured_cnt) 
      assert (color_at[captured[ii]] == color::empty);
  }

  void check_hash () const {
    assertc (board_hash_ac, hash == recalc_hash ());
  }

  void check_color_at () const {
    if (!board_color_at_ac) return;

    v_for_each (v) {
      color::check (color_at[v]);
      assert ((color_at[v] != color::edge) == (v::is_on_board (v)));
    }
  }

  void check_nbr_cnt () const {
    if (!board_nbr_cnt_ac) return;
    
    v_for_each (v) {
      coord::t r;
      coord::t c;
      uint nbr_color_cnt[color::cnt];
      uint expected_nbr_cnt;

      if (color_at[v] == color::edge) continue; // TODO is that right?

      r = v::row (v);
      c = v::col (v);

      assert (coord::is_ok (r)); // checking the macro
      assert (coord::is_ok (c));
          
      rep (col, color::cnt) nbr_color_cnt[col] = 0;
          
      nbr_color_cnt[color_at[v::N (v)]]++;
      nbr_color_cnt[color_at[v::W (v)]]++;
      nbr_color_cnt[color_at[v::E (v)]]++;
      nbr_color_cnt[color_at[v::S (v)]]++;
          
      expected_nbr_cnt =        // definition of nbr_cnt[v]
        ((nbr_color_cnt[color::black] + nbr_color_cnt[color::edge]) << 0) +
        ((nbr_color_cnt[color::white] + nbr_color_cnt[color::edge]) << 4) +
        ((nbr_color_cnt[color::empty] << 8));
    
      assert (nbr_cnt[v] == expected_nbr_cnt);
    }
  }

  void check_chain_at () const { // TODO move it to check_chains
    if (!chain_at_ac) return;

    v_for_each (v) { // whether same color neighbours have same root and liberties
      // TODO what aoub edge and empty?
      if (color::is_player (color_at[v])) {

        assert (!chain_at[v].find_root_npc()->lib_cnt_is_zero ());

        if (color_at[v] == color_at[v::S (v)]) 
          assert (chain_at[v].find_root_npc () == chain_at[v::S (v)].find_root_npc ());

        if (color_at[v] == color_at[v::E (v)]) 
          assert (chain_at[v].find_root_npc () == chain_at[v::E (v)].find_root_npc ());

      }
    }
  }

  void check_chains () const {        // ... and chain_next_v
    uint            act_chain_no;
    const chain_t*  chain_root[v::cnt - 1]; // list; could be smaller
    uint            chain_no[v::cnt]; // 

    const uint      no_chain = 100;

    if (!chains_ac) return;
    
    act_chain_no = 0;

    v_for_each (v) chain_no[v] = no_chain;

    // TODO what about empty and edge?
    v_for_each (v) if (color::is_player(color_at[v]) && chain_no[v] == no_chain) { // chain not visited yet
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
      
      for (v::t act_v = v; act_v != v; act_v = chain_next_v[act_v]) {
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
        
      }
      
      assert (forward_edge_cnt == backward_edge_cnt);
      assert (act_root->lib_cnt () == lib_cnt);
      v::check_is_on_board (chain_at-act_root);
      assert (chain_no[chain_at-act_root] == act_chain_no); // root is a part of the chain
      
      act_chain_no++;
    }
  }

  void check () const {
    if (!board_ac) return;

    check_captured ();
    check_hash ();
    check_color_at ();
    check_nbr_cnt ();
    check_chain_at ();
    check_chains ();
  }

  public:

  board_t () { clear (); }
  
  board_t (const board_t* to_copy) { load (to_copy); }

  bool clear () {
    int r, c;

    komi = 7.5;

    rep (v, v::cnt) {
      color_at[v] = color::edge;
      nbr_cnt[v] = 4 << 8;

      r = v::row (v);
      c = v::col (v);

      if (v::is_on_board (v)) color_at[v] = color::empty;

      if (r == 0 || r == board_size-1) nbr_cnt[v] += 1 + (1 << 4) - (1 << 8);
      if (c == 0 || c == board_size-1) nbr_cnt[v] += 1 + (1 << 4) - (1 << 8);
      
      (chain_at+v)->init (nbr_cnt[v] >> 8);
    }

    //captured_top = captured;
    captured_cnt = 0;
    hash = recalc_hash ();

    check ();

    return true;
  }

  hash::t recalc_hash () const {
    hash::t new_hash;

    new_hash = 0;
    rep (v, v::cnt) {
      if (color::is_player (color_at[v])) {
        new_hash ^= zobrist->of_pl_v (player::t (color_at[v]), v);
      }
    }
    
    return new_hash;
  }

  void load (const board_t* save_board) { 
    memcpy(this, save_board, sizeof(board_t)); 
    int delta;

    delta = (chain_t*) (this) - (chain_t*) (save_board);

    rep (v, v::cnt) {       // TODO this is mega hack. Remove it !
      assertc (board_ac, false // TODO unprivate ? friend ?
//                (chain_at+v)->parent + delta == 
//                chain_at + 
//                (save_board->chain_at+v)->parent - 
//                save_board->chain_at;
               );

      chain_at[v].update_pointer (delta);
    }

    check ();
  }
  
  void set_komi (float komi) { this->komi = komi; }

  play_ret_t play (player::t player, v::t v) {
    chain_t* new_chain_root;
    color::t color;

    check ();
    player::check (player);
    v::check_is_on_board (v);
    assertc (board_ac, color_at[v] == color::empty);

    color = color::t (player);

    if (((nbr_cnt[v] >> (4-player*4)) & 0xf) == 4)
      return play_no_lib (player, v); // TODO in MC we always know that we play in eye

    color_at[v] = color;
    hash ^= zobrist->of_pl_v (player, v);
    
    (chain_at+v)->init (nbr_cnt[v] >> 8);
    new_chain_root = &chain_at[v];
    chain_next_v[v] = v;

    process_new_nbr (v::N(v), color, v, &new_chain_root); // TODO is new_chain_root really needed?
    process_new_nbr (v::W(v), color, v, &new_chain_root);
    process_new_nbr (v::E(v), color, v, &new_chain_root);
    process_new_nbr (v::S(v), color, v, &new_chain_root);
             
    if (chain_at[v].find_root ()->lib_cnt_is_zero ()) {
      remove_chain(v);
      assertc (board_ac, chain_next_v[v] != v);
      return play_suicide;
    }

    return play_ok;
  }

  play_ret_t play_no_lib (player::t player, v::t v) {
    chain_t* chain_root_N;
    chain_t* chain_root_W;
    chain_t* chain_root_S;
    chain_t* chain_root_E;
    color::t color;

    color = color::t (player);  // TODO fix it

    assertc (board_ac, color_at[v::N (v)] == color::opponent (color) || color_at[v::N (v)] == color::edge);
    assertc (board_ac, color_at[v::W (v)] == color::opponent (color) || color_at[v::W (v)] == color::edge);
    assertc (board_ac, color_at[v::E (v)] == color::opponent (color) || color_at[v::E (v)] == color::edge);
    assertc (board_ac, color_at[v::S (v)] == color::opponent (color) || color_at[v::S (v)] == color::edge);

    // TODO check common expresion elim

    chain_root_N = (chain_at + v::N(v))->find_root ();
    chain_root_W = (chain_at + v::W(v))->find_root ();
    chain_root_E = (chain_at + v::E(v))->find_root ();
    chain_root_S = (chain_at + v::S(v))->find_root ();

    chain_root_N->dec_lib_cnt ();
    chain_root_W->dec_lib_cnt ();
    chain_root_E->dec_lib_cnt ();
    chain_root_S->dec_lib_cnt ();

    if ((!chain_root_N->lib_cnt_is_zero ()) & // TODO is it optimalized properly ?
        (!chain_root_W->lib_cnt_is_zero ()) & 
        (!chain_root_E->lib_cnt_is_zero ()) & 
        (!chain_root_S->lib_cnt_is_zero ())) 
    {
      chain_root_N->inc_lib_cnt ();
      chain_root_W->inc_lib_cnt ();
      chain_root_S->inc_lib_cnt ();
      chain_root_E->inc_lib_cnt ();
      return play_ss_suicide;
    }

    color_at[v] = color;
    hash ^= zobrist->of_pl_v (player, v);
    
    (chain_at+v)->init (nbr_cnt[v] >> 8);
    chain_next_v[v] = v;

    nbr_cnt[v::N(v)] += (1 << (color * 4)) - (1 << 8);
    nbr_cnt[v::W(v)] += (1 << (color * 4)) - (1 << 8); 
    nbr_cnt[v::E(v)] += (1 << (color * 4)) - (1 << 8);
    nbr_cnt[v::S(v)] += (1 << (color * 4)) - (1 << 8);

    if (chain_root_N->lib_cnt_is_zero ()) remove_chain(v::N(v));
    if (chain_root_W->lib_cnt_is_zero ()) remove_chain(v::W(v));
    if (chain_root_E->lib_cnt_is_zero ()) remove_chain(v::E(v));
    if (chain_root_S->lib_cnt_is_zero ()) remove_chain(v::S(v));

    assertc (board_ac, !chain_at[v].find_root_npc ()->lib_cnt_is_zero ());

    return play_ok;
  }

  void process_new_nbr(v::t v, color::t new_nbr_color, v::t new_nbr_v, chain_t** new_nbr_chain_root) {
    chain_t* chain_root;
    
    nbr_cnt[v] += (1 << (new_nbr_color * 4)) - (1 << 8);

    if (color_at[v] > color::white) return; // not player

    chain_root = (chain_at + v)->find_root ();
    
    chain_root->dec_lib_cnt ();
    if (color_at[v] == new_nbr_color) { // same color of groups
      if (chain_root != *new_nbr_chain_root) { // not merged yet
        *new_nbr_chain_root = chain_root->join (*new_nbr_chain_root);
        swap (chain_next_v[v], chain_next_v[new_nbr_v]);
      }
    } else {
      if (chain_root->lib_cnt_is_zero ()) remove_chain(v);
    }
  }
  
  void remove_chain (v::t v) {
    v::t act_v;
    color::t old_color;

    old_color = color_at[v];
    act_v = v;

    do {
      assertc (board_ac, is_player (old_color));

      hash ^= zobrist->of_pl_v (player::t(color_at[act_v]), act_v);
      color_at[act_v] = color::empty;
      (chain_at+act_v)->init (nbr_cnt[act_v] >> 8);

      captured[captured_cnt++] = act_v;
      assertc (board_ac, captured_cnt < v::cnt);

      act_v = chain_next_v[act_v];
    } while (act_v != v);

    assertc (board_ac, act_v == v);
    do {
      add_liberty (v::N(act_v), old_color);
      add_liberty (v::W(act_v), old_color);
      add_liberty (v::E(act_v), old_color);
      add_liberty (v::S(act_v), old_color);
      
      act_v = chain_next_v[act_v];
    } while (act_v != v);
  }


  void add_liberty (v::t v, color::t nbr_old_color) { // TODO inline it
    nbr_cnt[v] -= (1 << (nbr_old_color * 4)) - (1 << 8);
    (chain_at+v)->find_root ()->inc_lib_cnt (); 
  }

  // utils

  bool is_eyelike (color::t color, v::t v) { 
    int diag_color_cnt[color::cnt];

    if (((nbr_cnt[v] >> (color * 4)) & 0xf) != 4) return false; // TODO insert macro / function here

    rep (c, color::cnt) diag_color_cnt[c] = 0; // TODO bzero, here and everywhere
    diag_color_cnt[color_at[v::NW (v)]]++;
    diag_color_cnt[color_at[v::NE (v)]]++;
    diag_color_cnt[color_at[v::SW (v)]]++;
    diag_color_cnt[color_at[v::SE (v)]]++;

    if (diag_color_cnt[color::edge] > 0) diag_color_cnt[color::opponent(color)]++;
    if (diag_color_cnt[color::opponent(color)] >= 2) return false;
    return true;
  }
  
  int score () {
    int color_score [color::cnt];

    rep (c, 4) color_score[c] = 0;      // TODO bzero
    rep (v, v::cnt) {       // TODO
      color_score[color_at[v]]++;
      if (color_at[v] == color::empty) {
        if ((nbr_cnt[v] & 0xf) == 0x4) color_score[color::black]++; // macro
        else if ((nbr_cnt[v] & 0xf0) == 0x40) color_score[color::white]++; 
        // TODO test it (removin else may speed it up)
      }
    }
    return color_score[color::black] - color_score[color::white];
  }

  color::t winner () {
    float s;

    s = - komi;
    s += (float) score ();
    if (s > 0) return color::black;
    assertc (board_ac, s < 0);
    return color::white;
  }

  
  void print (v::t mark_v) {    // TODO use macro
    v::t v;

    printf("   ");
    rep (c, board_size) printf(" %d ", c+1);
    printf("\n");

    rep (r, board_size) {
      printf(" %d ", r+1);

      rep (c, board_size) {
        v = v::of_rc (r, c);
        if (v == mark_v) {
          printf("(%c)", color::to_char (color_at[v]));
        } else {
          printf(" %c ", color::to_char (color_at[v]));
        }
      }
      
      printf(" %d ", r+1);
      printf("\n");
    }
    
    printf("   ");
    rep (c, board_size) printf(" %d ", c+1);
    printf("\n");
  }
  
};
