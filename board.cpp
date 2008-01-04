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


// class hash_t


class hash_t {
public:
  
  uint64 hash;

  hash_t () { }

  uint index () const { return hash; }
  uint lock  () const { return hash >> 32; }

  void randomize (random_pm_t& pm) { 
    hash =
      (uint64 (pm.rand_int ()) << (0*16)) ^
      (uint64 (pm.rand_int ()) << (1*16)) ^ 
      (uint64 (pm.rand_int ()) << (2*16)) ^ 
      (uint64 (pm.rand_int ()) << (3*16));
  }

  void set_zero () { hash = 0; }

  bool operator== (const hash_t& other) const { return hash == other.hash; }
  void operator^= (const hash_t& other) { hash ^= other.hash; }

};


// class zobrist_t


class zobrist_t {
public:

  move_map_t <hash_t> hashes;

  zobrist_t (random_pm_t& pm) {
    player_for_each (pl) vertex_for_each_all (v) {
      move_t m = move_t (pl, v);
      hashes [m].randomize (pm);
    }
  }

  hash_t of_move (move_t m) const {
    return hashes [m];
  }

  hash_t of_pl_v (player_t pl,  vertex_t v) const {
    return hashes [move_t (pl, v)];
  }

};



// namespace nbr_cnt_aux


namespace nbr_cnt_aux { // TODO this namespace exists only because we can't have inlined cont arrays in classes

#ifdef Ho
  static const uint max = 6;                 // maximal number of neighbours
#else
  static const uint max = 4;                 // maximal number of neighbours
#endif
  
  const uint f_size = 4;              // size in bits of each of 3 counters in nbr_cnt::t
  const uint f_shift [3] = { 0 * f_size, 1 * f_size, 2 * f_size };
  const uint f_mask = (1 << f_size) - 1;
  
  const uint player_cnt_is_max_mask [player_aux::cnt] = {  // TODO player_map_t
    (max << f_shift [player_aux::black_idx]), 
    (max << f_shift [player_aux::white_idx]) 
  };
  
  const uint black_inc_val = (1 << f_shift [player_aux::black_idx]) - (1 << f_shift [color_aux::empty_idx]);
  const uint white_inc_val = (1 << f_shift [player_aux::white_idx]) - (1 << f_shift [color_aux::empty_idx]);
  const uint off_board_inc_val  = 
    + (1 << f_shift [player_aux::black_idx]) 
    + (1 << f_shift [player_aux::white_idx]) 
    - (1 << f_shift [color_aux::empty_idx]);
  
  const uint player_inc_tab [player_aux::cnt] = { black_inc_val, white_inc_val };
}


// class nbr_cnt_t


class nbr_cnt_t {

  public:
    uint bitfield;

    nbr_cnt_t () { }

    nbr_cnt_t (uint black_cnt, uint white_cnt, uint empty_cnt) {
      assertc (nbr_cnt_ac, black_cnt <= nbr_cnt_aux::max);
      assertc (nbr_cnt_ac, white_cnt <= nbr_cnt_aux::max);
      assertc (nbr_cnt_ac, empty_cnt <= nbr_cnt_aux::max);
      
      bitfield = 
        (black_cnt << nbr_cnt_aux::f_shift [player_aux::black_idx]) +
        (white_cnt << nbr_cnt_aux::f_shift [player_aux::white_idx]) +
        (empty_cnt << nbr_cnt_aux::f_shift [color_aux::empty_idx]);
    }
  
    //void operator+= (const uint delta) { bitfield += delta; }

    void off_board_inc () { bitfield += nbr_cnt_aux::off_board_inc_val; }
    void player_inc (player_t player) { bitfield += nbr_cnt_aux::player_inc_tab [player.get_idx ()]; }
    void player_dec (player_t player) { bitfield -= nbr_cnt_aux::player_inc_tab [player.get_idx ()]; }

    uint empty_cnt  () const { 
      return bitfield >> nbr_cnt_aux::f_shift [color_aux::empty_idx];
    }

    uint player_cnt (player_t pl) const { 
      return (bitfield >> nbr_cnt_aux::f_shift [pl.get_idx ()]) & nbr_cnt_aux::f_mask; 
    }


    uint player_cnt_is_max (player_t pl) const { 
      return (bitfield & nbr_cnt_aux::player_cnt_is_max_mask [pl.get_idx ()]) == nbr_cnt_aux::player_cnt_is_max_mask [pl.get_idx ()]; 
    }

    void check () {
      if (!nbr_cnt_ac) return;
      assert (empty_cnt () <= nbr_cnt_aux::max);
      assert (player_cnt (player_black) <= nbr_cnt_aux::max);
      assert (player_cnt (player_white) <= nbr_cnt_aux::max);
    }

};


// class board_t


enum play_ret_t { play_ok, play_suicide, play_ss_suicide, play_ko, play_non_empty };

random_pm_t zobrist_pm;
const zobrist_t zobrist[1] = { zobrist_t (zobrist_pm) }; // TODO move it to board

class board_t {
  
public:

  vertex_map_t <color_t>    color_at;
  vertex_map_t <nbr_cnt_t>   nbr_cnt; // incremental, for fast eye checking
  vertex_map_t <uint>        empty_pos;
  vertex_map_t <vertex_t>    chain_next_v;

  uint                       chain_lib_cnt [vertex_aux::cnt]; // indexed by chain_id
  vertex_map_t <uint>        chain_id;
  
  vertex_t    empty_v [board_area];
  uint        empty_v_cnt;
  uint        last_empty_v_cnt;

  player_map_t <uint>        player_v_cnt;
  player_map_t <vertex_t>    player_last_v;

  hash_t      hash;
  int         komi;

#ifndef Ho
  vertex_t        ko_v;             // vertex forbidden by ko (only Go)
#endif

  player_t   last_player;      // player who made the last play (other::player is forbidden to retake)
  uint       move_no;

public:                         // macros

  #define empty_v_for_each(board, vv, i) {                              \
    vertex_t vv = vertex_any;                                          \
    rep (ev_i, (board)->empty_v_cnt) {                                  \
      vv = (board)->empty_v [ev_i];                                     \
      i;                                                                \
    }                                                                   \
  }
  
  #define empty_v_for_each_and_pass(board, vv, i) {                     \
    vertex_t vv = vertex_pass;                                          \
    i;                                                                  \
    rep (ev_i, (board)->empty_v_cnt) {                                  \
      vv = (board)->empty_v [ev_i];                                     \
      i;                                                                \
    }                                                                   \
  }

public:                         // consistency checks

  void check_empty_v () const {
    if (!board_empty_v_ac) return;

    vertex_map_t <bool> noticed;
    player_map_t <uint> exp_player_v_cnt;

    vertex_for_each_all (v) noticed[v] = false;

    assert (empty_v_cnt <= board_area);

    empty_v_for_each (this, v, {
      assert (noticed [v] == false);
      noticed [v] = true;
    });

    player_for_each (pl) exp_player_v_cnt [pl] = 0;

    vertex_for_each_all (v) {
      assert ((color_at[v] == color_empty) == noticed[v]);
      if (color_at[v] == color_empty) {
        assert (empty_pos[v] < empty_v_cnt);
        assert (empty_v [empty_pos[v]] == v);
      }
      if (color_at [v].is_player ()) exp_player_v_cnt [color_at[v].to_player ()]++;
    }

    player_for_each (pl) 
      assert (exp_player_v_cnt [pl] == player_v_cnt [pl]);
  }

  void check_hash () const {
    assertc (board_hash_ac, hash == recalc_hash ());
  }

  void check_color_at () const {
    if (!board_color_at_ac) return;

    vertex_for_each_all (v) {
      color_at [v].check ();
      assert ((color_at[v] != color_off_board) == (v.is_on_board ()));
    }
  }

  void check_nbr_cnt () const {
    if (!board_nbr_cnt_ac) return;
    
    vertex_for_each_all (v) {
      coord::t r;
      coord::t c;
      color_map_t <uint> nbr_color_cnt;
      uint expected_nbr_cnt;

      if (color_at[v] == color_off_board) continue; // TODO is that right?

      r = v.row ();
      c = v.col ();

      assert (coord::is_on_board (r)); // checking the macro
      assert (coord::is_on_board (c));
          
      color_for_each (col) nbr_color_cnt [col] = 0;
          
      vertex_for_each_nbr (v, nbr_v, nbr_color_cnt [color_at [nbr_v]]++);
          
      expected_nbr_cnt =        // definition of nbr_cnt[v]
        + ((nbr_color_cnt [color_black] + nbr_color_cnt [color_off_board]) 
           << nbr_cnt_aux::f_shift [player_aux::black_idx])
        + ((nbr_color_cnt [color_white] + nbr_color_cnt [color_off_board])
           << nbr_cnt_aux::f_shift [player_aux::white_idx])
        + ((nbr_color_cnt [color_empty]) 
           << nbr_cnt_aux::f_shift [color_aux::empty_idx]);
    
      assert (nbr_cnt[v].bitfield == expected_nbr_cnt);
    }
  }

  void check_chain_at () const {
    if (!chain_at_ac) return;

    vertex_for_each_all (v) { // whether same color neighbours have same root and liberties
      // TODO what about off_board and empty?
      if (color_at [v].is_player ()) {

        assert (chain_lib_cnt[ chain_id [v]] != 0);

        vertex_for_each_nbr (v, nbr_v, {
          if (color_at[v] == color_at[nbr_v]) 
            assert (chain_id [v] == chain_id [nbr_v]);
        });
      }
    }
  }

  void check_chain_next_v () const {
    if (!chain_next_v_ac) return;
    vertex_for_each_all (v) {
      chain_next_v[v].check ();
      if (!color_at [v].is_player ()) 
        assert (chain_next_v [v] == v);
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
  }

  void check_no_more_legal (player_t player) { // at the end of the playout
    unused (player);

    if (!board_ac) return;

    vertex_for_each_all (v)
      if (color_at[v] == color_empty)
        assert (is_eyelike (player, v) || play_not_pass (player, v) >= play_ss_suicide);
  }


public:                         // board interface

  board_t () { 
    clear (); 
    if (color_at[0] == 11) print_cerr (); // TODO LOL hack - need tu use it somwhere 
  }
  
  void clear () {
    uint off_board_cnt;

    set_komi (7.5);            // standard for chinese rules
    empty_v_cnt  = 0;
    player_for_each (pl) {
      player_v_cnt  [pl] = 0;
      player_last_v [pl] = vertex_any;
    }
    move_no      = 0;
    last_player  = player_white; // act player is other
#ifndef Ho
    ko_v         = vertex_any;             // only Go
#endif
    vertex_for_each_all (v) {
      color_at      [v] = color_off_board;
      nbr_cnt       [v] = nbr_cnt_t (0, 0, nbr_cnt_aux::max);
      chain_next_v  [v] = v;
      chain_id      [v] = v.get_idx ();    // TODO is it needed, is it usedt?
      chain_lib_cnt [v.get_idx ()] = nbr_cnt_aux::max; // TODO is it logical? (off_boards)

      if (v.is_on_board ()) {
        color_at   [v]              = color_empty;
        empty_pos  [v]              = empty_v_cnt;
        empty_v    [empty_v_cnt++]  = v;

        off_board_cnt = 0;
        vertex_for_each_nbr (v, nbr_v, if (!nbr_v.is_on_board ()) off_board_cnt++);
        rep (ii, off_board_cnt) nbr_cnt [v].off_board_inc ();
      }
    }

    hash = recalc_hash ();

    check ();
  }

  hash_t recalc_hash () const {
    hash_t new_hash;

    new_hash.set_zero ();

    vertex_for_each_all (v) {
      if (color_at [v].is_player ()) {
        new_hash ^= zobrist->of_pl_v (color_at [v].to_player (), v);
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

  // PLAY FUNCTIONS

  flatten all_inline 
  play_ret_t play_act_player (vertex_t v) {
    return play (act_player (), v);
  }

  flatten all_inline 
  play_ret_t play (player_t player, vertex_t v) {
    // assertions in lower functions
    if (v == vertex_pass) { 
      play_pass (player); 
      return play_ok; 
    }
    return play_not_pass (player, v);
  }


  void play_pass (player_t player) {
    check ();
    last_empty_v_cnt        = empty_v_cnt;
#ifndef Ho
    ko_v                    = vertex_any;
#endif
    last_player             = player;
    player_last_v [player]  = vertex_pass;
    move_no                += 1;
  }

  flatten all_inline 
  play_ret_t play_not_pass (player_t player, vertex_t v) {
    check ();
    v.check_is_on_board ();
    assertc (board_ac, color_at[v] == color_empty);

    if (nbr_cnt[v].player_cnt_is_max (player.other ()))
      return play_eye (player, v);
    else 
      return play_not_eye (player, v);
  }

  play_ret_t play_not_eye (player_t player, vertex_t v) {
    check ();
    v.check_is_on_board ();
    assertc (board_ac, color_at[v] == color_empty);

    last_empty_v_cnt        = empty_v_cnt;
#ifndef Ho
    ko_v                    = vertex_any;
#endif
    last_player             = player;
    player_last_v [player]  = v;
    move_no                += 1;

    place_stone (player, v);

    vertex_for_each_nbr (v, nbr_v, process_new_nbr (nbr_v, player, v));

    if (chain_lib_cnt [chain_id [v]] == 0) {
      assertc (board_ac, last_empty_v_cnt - empty_v_cnt == 1);
      remove_chain(v);
      assertc (board_ac, last_empty_v_cnt - empty_v_cnt > 0);
      return play_suicide;
    }

    return play_ok;
  }

  no_inline
  play_ret_t play_eye (player_t player, vertex_t v) {
    vertex_for_each_nbr (v, nbr_v, {
      assertc (board_ac, 
               color_at [nbr_v] == color_t (player.other ()) || 
               color_at[nbr_v] == color_off_board);
    });


#ifndef Ho
    if (v == ko_v && player == last_player.other ()) // only Go
      return play_ko;
#endif

    uint all_nbr_live = true;
    vertex_for_each_nbr (v, nbr_v, all_nbr_live &= (--chain_lib_cnt [chain_id [nbr_v]] != 0));

    if (all_nbr_live) {
      vertex_for_each_nbr (v, nbr_v, chain_lib_cnt [chain_id [nbr_v]]++);
      return play_ss_suicide;
    }

    last_empty_v_cnt        = empty_v_cnt;
    last_player             = player;
    player_last_v [player]  = v;
    move_no                += 1;

    place_stone (player, v);
    
    vertex_for_each_nbr (v, nbr_v, nbr_cnt [nbr_v].player_inc (player));

    vertex_for_each_nbr (v, nbr_v, if ((chain_lib_cnt [chain_id [nbr_v]] == 0)) remove_chain (nbr_v));

    assertc (board_ac, chain_lib_cnt [chain_id [v]] != 0);

#ifndef Ho
    if (last_empty_v_cnt == empty_v_cnt) { // if captured exactly one stone, end this was eye (only Go)
      ko_v = empty_v [empty_v_cnt - 1]; // then ko formed
    } else {
      ko_v = vertex_any;
    }
#endif

    return play_ok;
  }

  void process_new_nbr(vertex_t v, 
                       player_t new_nbr_player,
                       vertex_t new_nbr_v) // TODO moze warto chain id przekazywc do ustalenia?
  {
    nbr_cnt[v].player_inc (new_nbr_player);

    if (color_at [v].is_not_player ()) return;
    chain_lib_cnt [chain_id [v]] --;

    if (color_at[v] != color_t (new_nbr_player.get_idx ())) { // same color of groups
      if (chain_lib_cnt [chain_id [v]] == 0) remove_chain (v);
      return;
    }

    if (chain_id [v] == chain_id [new_nbr_v]) return;
      
    if (chain_lib_cnt [chain_id [new_nbr_v]] > chain_lib_cnt [chain_id [v]])
      merge_chains (new_nbr_v, v);
    else
      merge_chains (v, new_nbr_v);

  }

  void merge_chains (vertex_t v_base, vertex_t v_new) {
    vertex_t act_v;

    chain_lib_cnt [chain_id [v_base]] += chain_lib_cnt [chain_id [v_new]];

    act_v = v_new;
    do {
      chain_id [act_v] = chain_id [v_base];
      act_v = chain_next_v [act_v];
    } while (act_v != v_new);
    
    swap (chain_next_v[v_base], chain_next_v[v_new]);
  }

  no_inline 
  void remove_chain (vertex_t v){
    vertex_t act_v;
    vertex_t tmp_v;
    color_t old_color;

    old_color = color_at[v];
    act_v = v;

    assertc (board_ac, old_color.is_player ());

    do {
      remove_stone (act_v);
      act_v = chain_next_v[act_v];
    } while (act_v != v);

    assertc (board_ac, act_v == v);

    do {
      vertex_for_each_nbr (act_v, nbr_v, {
        nbr_cnt[nbr_v].player_dec (old_color.to_player());
        chain_lib_cnt [chain_id [nbr_v]]++;
      });

      tmp_v = act_v;
      act_v = chain_next_v[act_v];
      chain_next_v[tmp_v] = tmp_v;
      
    } while (act_v != v);
  }

  void place_stone (player_t pl, vertex_t v) {
    hash ^= zobrist->of_pl_v (pl, v);
    player_v_cnt[pl]++;
    color_at[v] = color_t (pl.get_idx ());

    empty_v_cnt--;
    empty_pos [empty_v [empty_v_cnt]] = empty_pos [v];
    empty_v [empty_pos [v]] = empty_v [empty_v_cnt];

    assertc (chain_next_v_ac, chain_next_v[v] == v);

    chain_id [v] = v.get_idx ();
    chain_lib_cnt [v.get_idx ()] = nbr_cnt[v].empty_cnt ();
  }

  void remove_stone (vertex_t v) {
    hash ^= zobrist->of_pl_v (color_at [v].to_player (), v);
    player_v_cnt [color_at[v].to_player ()]--;
    color_at[v] = color_empty;

    empty_pos [v] = empty_v_cnt;
    empty_v [empty_v_cnt++] = v;

    assertc (board_ac, empty_v_cnt < vertex_aux::cnt);
  }

public:                         // utils

  player_t act_player () const { return last_player.other (); }

  bool both_player_pass () {
    return 
      (player_last_v [player_black] == vertex_pass) & 
      (player_last_v [player_white] == vertex_pass);
  }

  bool is_eyelike (player_t player, vertex_t v) { 
#ifdef Ho
    return nbr_cnt::player_cnt_is_max (nbr_cnt[v], player);
#else
    color_map_t <int> diag_color_cnt; // TODO
    assertc (board_ac, color_at [v] == color_empty);

    if (! nbr_cnt[v].player_cnt_is_max (player)) return false;

    color_for_each (col) 
      diag_color_cnt [col] = 0; // memset is slower

    vertex_for_each_diag_nbr (v, diag_v, {
      diag_color_cnt [color_at [diag_v]]++;
    });

    diag_color_cnt [player.other ()] += (diag_color_cnt[color_off_board] > 0);
    return diag_color_cnt [player.other ()] < 2;
#endif
  }

  int approx_score () const {
    return komi + player_v_cnt[player_black] -  player_v_cnt[player_white];
  }

  player_t approx_winner () { return player_t (approx_score () <= 0); }

  int score () const {
    int eye_score = 0;

    empty_v_for_each (this, v, {
        eye_score += nbr_cnt[v].player_cnt_is_max (player_black);
        eye_score -= nbr_cnt[v].player_cnt_is_max (player_white);
    });

    return approx_score () + eye_score;
  }

  player_t winner () const { 
    return player_t (score () <= 0); 
  }

  int vertex_score (vertex_t v) {
    switch (color_at [v].get_idx ()) {
    case color_aux::black_idx: return 1;
    case color_aux::white_idx: return -1;
    case color_aux::empty_idx: 
      return 
        (nbr_cnt[v].player_cnt_is_max (player_black)) -
        (nbr_cnt[v].player_cnt_is_max (player_white));
    case color_aux::off_board_idx: return 0;
    default: assert (false);
    }
  }

  string to_string (vertex_t mark_v = vertex_any) const {
    ostringstream out;

    #define os(n)      out << " " << n
    #define o_left(n)  out << "(" << n
    #define o_right(n) out << ")" << n
    
#ifndef Ho
    out << " ";
#endif
    if (board_size < 10) out << " "; else out << "  ";
    coord_for_each (col) os (coord::col_to_string (col));
    out << endl;

    coord_for_each (row) {
      if (board_size >= 10 && board_size - row < 10) out << " ";
#ifdef Ho
      rep (ii, row) out << " ";
#endif
      os (coord::row_to_string (row));
      coord_for_each (col) {
        vertex_t v = vertex_t (row, col);
        char ch = color_at [v].to_char ();
        if      (v == mark_v)        o_left  (ch);
        else if (v == mark_v.E ())   o_right (ch);
        else                         os (ch);
      }
      if (board_size >= 10 && board_size - row < 10) out << " ";
      os (coord::row_to_string (row));
      out << endl;
    }
    
    if (board_size < 10) out << "  "; else out << "   ";
#ifdef Ho
    rep (ii, board_size) out << " ";
#endif
    coord_for_each (col) os (coord::col_to_string (col));
    out << endl;

    #undef os
    #undef o_left
    #undef o_right

    return out.str ();
  }

  void print_cerr (vertex_t v = vertex_pass) const {
    cerr << to_string (v);
  }

  bool load (istream& ifs) {
    uint       bs;
    char c;

    player_t  play_player[board_area];
    vertex_t       play_v[board_area];
    uint       play_cnt;

    clear ();

    play_cnt = 0;

    if (!ifs) return false;

    ifs >> bs;
    if (bs != board_size) return false;

    if (getc_non_space (ifs) != '\n') return false;

    coord_for_each (row) {
      coord_for_each (col) {
        color_t color;

        c = getc_non_space (ifs);
        color = color_t (c, 0); // TODO 0 is to choose the constructor
        if (color == color_wrong_char) return false;
        
        if (color.is_player ()) {
          play_player [play_cnt] = color.to_player ();
          play_v [play_cnt] = vertex_t (row, col);
          play_cnt++;
          assertc (board_ac, play_cnt < board_area);
        }
      }

      if (getc_non_space (ifs) != '\n') return false;
    }

    rep (pi, play_cnt) {
      play_ret_t ret;
      ret = play_not_pass (play_player[pi], play_v[pi]);
      if (ret != play_ok || last_empty_v_cnt - empty_v_cnt != 1) {
        cerr << "Fatal error: Illegal board configuration in file." << endl;
        exit (1);               // TODO this is a hack
      }
    }

    return true;
  }

};
