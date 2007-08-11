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

  void randomize () { 
    hash =
      (uint64 (pm::rand_int ()) << (0*16)) ^
      (uint64 (pm::rand_int ()) << (1*16)) ^ 
      (uint64 (pm::rand_int ()) << (2*16)) ^ 
      (uint64 (pm::rand_int ()) << (3*16));
  }

  void set_zero () { hash = 0; }

  bool operator== (const hash_t& other) const { return hash == other.hash; }
  void operator^= (const hash_t& other) { hash ^= other.hash; }

};


// class zobrist_t


class zobrist_t {
public:

  hash_t hashes[move::cnt];

  zobrist_t () {
    pl_v_for_each (pl, v) {
      move::t m;
      m = move::of_pl_v (pl, v);
      hashes[m].randomize ();
    }
  }

  hash_t of_move (move::t m) const {
    move::check (m);
    return hashes[m];
  }

  hash_t of_pl_v (player::t pl,  v::t v) const {
    player::check (pl);
    v.check ();
    return hashes[move::of_pl_v (pl, v)];
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
  
  const uint player_cnt_is_max_mask [player::cnt] = { 
    (max << f_shift [player::black]), 
    (max << f_shift [player::white]) 
  };
  
  const uint black_inc_val = (1 << f_shift [player::black]) - (1 << f_shift [color::empty]);
  const uint white_inc_val = (1 << f_shift [player::white]) - (1 << f_shift [color::empty]);
  const uint off_board_inc_val  = 
    + (1 << f_shift [player::black]) 
    + (1 << f_shift [player::white]) 
    - (1 << f_shift [color::empty]);
  
  const uint player_inc_tab [player::cnt] = { black_inc_val, white_inc_val };
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
        (black_cnt << nbr_cnt_aux::f_shift [player::black]) +
        (white_cnt << nbr_cnt_aux::f_shift [player::white]) +
        (empty_cnt << nbr_cnt_aux::f_shift [color::empty]);
    }
  
    //void operator+= (const uint delta) { bitfield += delta; }

    void off_board_inc () { bitfield += nbr_cnt_aux::off_board_inc_val; }
    void player_inc (player::t player) { bitfield += nbr_cnt_aux::player_inc_tab [player]; }
    void player_dec (player::t player) { bitfield -= nbr_cnt_aux::player_inc_tab [player]; }

    uint empty_cnt  () const { 
      return bitfield >> nbr_cnt_aux::f_shift [color::empty];
    }

    uint player_cnt (player::t pl) const { 
      return (bitfield >> nbr_cnt_aux::f_shift [pl]) & nbr_cnt_aux::f_mask; 
    }


    uint player_cnt_is_max (player::t pl) const { 
      return (bitfield & nbr_cnt_aux::player_cnt_is_max_mask [pl]) == nbr_cnt_aux::player_cnt_is_max_mask [pl]; 
    }

    void check () {
      if (!nbr_cnt_ac) return;
      assert (empty_cnt () <= nbr_cnt_aux::max);
      assert (player_cnt (player::black) <= nbr_cnt_aux::max);
      assert (player_cnt (player::white) <= nbr_cnt_aux::max);
    }

};


// class board_t


enum play_ret_t { play_ok, play_suicide, play_ss_suicide, play_ko, play_non_empty };

const zobrist_t zobrist[1]; // TODO move it to board

class board_t {
  
public:

  v::map_t <color::t>    color_at;
  v::map_t <nbr_cnt_t>   nbr_cnt; // incremental, for fast eye checking
  v::map_t <uint>        empty_pos;
  v::map_t <v::t>        chain_next_v;

  uint        chain_lib_cnt [v::cnt]; // indexed by chain_id
  v::map_t <uint>        chain_id;
  
  v::t        empty_v       [board_area];
  uint        empty_v_cnt;
  uint        last_empty_v_cnt;

  uint        player_v_cnt  [player::cnt];

  hash_t      hash;
  int         komi;

#ifndef Ho
  v::t        ko_v;             // vertex forbidden by ko (only Go)
#endif

  player::t   last_player;      // player who made the last play (other::player is forbidden to retake)

public:                         // macros

  #define empty_v_for_each(board, vv, i) {                              \
    v::t vv = v::no_v;                                                  \
    rep (ev_i, (board)->empty_v_cnt) {                                  \
      vv = (board)->empty_v [ev_i];                                     \
      i;                                                                \
    }                                                                   \
  }
  
  #define empty_v_for_each_and_pass(board, vv, i) {                     \
    v::t vv = v::pass;                                                  \
    i;                                                                  \
    rep (ev_i, (board)->empty_v_cnt) {                                  \
      vv = (board)->empty_v [ev_i];                                     \
      i;                                                                \
    }                                                                   \
  }

public:                         // consistency checks

  void check_empty_v () const {
    if (!board_empty_v_ac) return;

    v::map_t <bool> noticed;
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
      assert ((color_at[v] != color::off_board) == (v.is_on_board ()));
    }
  }

  void check_nbr_cnt () const {
    if (!board_nbr_cnt_ac) return;
    
    v_for_each_onboard (v) {
      coord::t r;
      coord::t c;
      uint nbr_color_cnt[color::cnt];
      uint expected_nbr_cnt;

      if (color_at[v] == color::off_board) continue; // TODO is that right?

      r = v.row ();
      c = v.col ();

      assert (coord::is_ok (r)); // checking the macro
      assert (coord::is_ok (c));
          
      color_for_each (col) nbr_color_cnt [col] = 0;
          
      v_for_each_nbr (v, nbr_v, nbr_color_cnt [color_at [nbr_v]]++);
          
      expected_nbr_cnt =        // definition of nbr_cnt[v]
        + ((nbr_color_cnt [color::black] + nbr_color_cnt [color::off_board]) 
           << nbr_cnt_aux::f_shift [player::black])
        + ((nbr_color_cnt [color::white] + nbr_color_cnt [color::off_board])
           << nbr_cnt_aux::f_shift [player::white])
        + ((nbr_color_cnt [color::empty]) 
           << nbr_cnt_aux::f_shift [color::empty]);
    
      assert (nbr_cnt[v].bitfield == expected_nbr_cnt);
    }
  }

  void check_chain_at () const {
    if (!chain_at_ac) return;

    v_for_each_onboard (v) { // whether same color neighbours have same root and liberties
      // TODO what about off_board and empty?
      if (color::is_player (color_at[v])) {

        assert (chain_lib_cnt[ chain_id [v]] != 0);

        v_for_each_nbr (v, nbr_v, {
          if (color_at[v] == color_at[nbr_v]) 
            assert (chain_id [v] == chain_id [nbr_v]);
        });
      }
    }
  }

  void check_chain_next_v () const {
    if (!chain_next_v_ac) return;
    v_for_each_all (v) {
      chain_next_v[v].check ();
      if (!color::is_player (color_at[v])) 
        assert (chain_next_v[v] == v);
    }
  }

  void check_chains () const {        // ... and chain_next_v
    uint            act_chain_no;
    uint            chain_id_list [v::cnt - 1]; // list; could be smaller
    v::map_t <uint> chain_no;

    const uint      no_chain = 10000;

    if (!chains_ac) return;
    
    act_chain_no = 0;

    v_for_each_all (v) chain_no[v] = no_chain;

    // TODO what about empty and off_board?
    v_for_each_onboard (v) {
      if (color::is_player(color_at[v]) && chain_no[v] == no_chain) { // chain not visited yet
        color::t        act_color;

        uint lib_cnt;
        uint forward_off_board_cnt;
        uint backward_off_board_cnt;

        chain_id_list [act_chain_no] = chain_id [v];

        rep (ch_no, act_chain_no) 
          assert (chain_id_list [ch_no] != chain_id [v]); // separate chains, separate roots
        
        act_color          = color_at[v]; 
        lib_cnt            = 0;
        forward_off_board_cnt   = 0;
        backward_off_board_cnt  = 0;
        
        v::t act_v = v;
        do {
          assert (color_at[act_v] == act_color);
          assert (chain_id[act_v] == chain_id_list [act_chain_no]);
          assert (chain_no[act_v] == no_chain);
          chain_no[act_v] = act_chain_no;
          
          v_for_each_nbr (act_v, nbr_v, {
            if (color_at[nbr_v] == color::empty) lib_cnt++;
            if (color_at[nbr_v] == act_color) {
              if (chain_no[nbr_v] == act_chain_no) forward_off_board_cnt++; 
              else {
                assert (chain_no[nbr_v] == no_chain);
                backward_off_board_cnt++;
              }
            }
            
          });
          
          act_v = chain_next_v[act_v];
        } while (act_v != v);
        
        assert (forward_off_board_cnt == backward_off_board_cnt);
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
    uint off_board_cnt;

    set_komi (7.5);            // standard for chinese rules
    empty_v_cnt = 0;
    player_for_each (pl) player_v_cnt [pl] = 0;
#ifndef Ho
    ko_v = v::no_v;             // only Go
#endif
    v_for_each_all (v) {
      color_at      [v] = color::off_board;
      nbr_cnt       [v] = nbr_cnt_t (0, 0, nbr_cnt_aux::max);
      chain_next_v  [v] = v;
      chain_id      [v] = v.idx;    // TODO is it needed, is it usedt?
      chain_lib_cnt [v.idx] = nbr_cnt_aux::max; // TODO is it logical? (off_boards)

      if (v.is_on_board ()) {
        color_at   [v]              = color::empty;
        empty_pos  [v]              = empty_v_cnt;
        empty_v    [empty_v_cnt++]  = v;

        off_board_cnt = 0;
        v_for_each_nbr (v, nbr_v, if (!nbr_v.is_on_board ()) off_board_cnt++);
        rep (ii, off_board_cnt) nbr_cnt [v].off_board_inc ();
      }
    }

    hash = recalc_hash ();

    check ();
  }

  hash_t recalc_hash () const {
    hash_t new_hash;

    new_hash.set_zero ();

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
    v.check_is_on_board ();
    assertc (board_ac, color_at[v] == color::empty);

    if (nbr_cnt[v].player_cnt_is_max (player::other (player)))
      return play_eye (player, v);
    else 
      return play_no_eye (player, v);
  }

  play_ret_t play_no_eye (player::t player, v::t v) {
    check ();
    player::check (player);
    v.check_is_on_board ();
    assertc (board_ac, color_at[v] == color::empty);

    last_empty_v_cnt = empty_v_cnt;
#ifndef Ho
    ko_v             = v::no_v;
#endif
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
    v_for_each_nbr (v, nbr_v, assertc (board_ac, color_at[nbr_v] == color::opponent (player) || color_at[nbr_v] == color::off_board));

#ifndef Ho
    if (v == ko_v && player == player::other (last_player)) // only Go
      return play_ko;
#endif

    uint all_nbr_live = true;
    v_for_each_nbr (v, nbr_v, all_nbr_live &= (--chain_lib_cnt [chain_id [nbr_v]] != 0));

    if (all_nbr_live) {
      v_for_each_nbr (v, nbr_v, chain_lib_cnt [chain_id [nbr_v]]++);
      return play_ss_suicide;
    }

    last_empty_v_cnt = empty_v_cnt;
    last_player      = player;

    place_stone (player, v);
    
    v_for_each_nbr (v, nbr_v, nbr_cnt [nbr_v].player_inc (player));

    v_for_each_nbr (v, nbr_v, if ((chain_lib_cnt [chain_id [nbr_v]] == 0)) remove_chain (nbr_v));

    assertc (board_ac, chain_lib_cnt [chain_id [v]] != 0);

#ifndef Ho
    if (last_empty_v_cnt == empty_v_cnt) { // if captured exactly one stone, end this was eye (only Go)
      ko_v = empty_v [empty_v_cnt - 1]; // then ko formed
    } else {
      ko_v = v::no_v;
    }
#endif

    return play_ok;
  }

  void process_new_nbr(v::t v, 
                       player::t new_nbr_player,
                       v::t new_nbr_v) // TODO moze warto chain id przekazywc do ustalenia?
  {
    nbr_cnt[v].player_inc (new_nbr_player);

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
        nbr_cnt[nbr_v].player_dec (player::t (old_color));
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

    chain_id [v] = v.idx;
    chain_lib_cnt [v.idx] = nbr_cnt[v].empty_cnt ();
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
#ifdef Ho
    return nbr_cnt::player_cnt_is_max (nbr_cnt[v], player);
#else
    int diag_color_cnt[color::cnt];

    assertc (board_ac, color_at [v] == color::empty);

    if (! nbr_cnt[v].player_cnt_is_max (player)) return false;

    color_for_each (col) diag_color_cnt [col] = 0; // memset is slower
    v_for_each_diag_nbr (v, diag_v, {
      diag_color_cnt [color_at [diag_v]]++;
    });

    diag_color_cnt[player::other (player)] += (diag_color_cnt[color::off_board] > 0);
    return diag_color_cnt[player::other (player)] < 2;
#endif
  }

  int approx_score () const {
    return komi + player_v_cnt[player::black] -  player_v_cnt[player::white];
  }

  player::t approx_winner () { return player::t (approx_score () <= 0); }

  int score () const {
    int eye_score;

    eye_score = 0;

    empty_v_for_each (this, v, {
      if (nbr_cnt[v].player_cnt_is_max (player::black)) {
        eye_score++;
      } else if (nbr_cnt[v].player_cnt_is_max (player::white)) {
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
        v::t v = v::t (row, col);
        char ch = color::to_char (color_at[v]);
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
          play_v[play_cnt] = v::t (row, col);
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
