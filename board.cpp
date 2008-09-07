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


// class Hash


class Hash {
public:
  
  uint64 hash;

  Hash () { }

  uint index () const { return hash; }
  uint lock  () const { return hash >> 32; }

  void randomize (PmRandom& pm) { 
    hash =
      (uint64 (pm.rand_int ()) << (0*16)) ^
      (uint64 (pm.rand_int ()) << (1*16)) ^ 
      (uint64 (pm.rand_int ()) << (2*16)) ^ 
      (uint64 (pm.rand_int ()) << (3*16));
  }

  void set_zero () { hash = 0; }

  bool operator== (const Hash& other) const { return hash == other.hash; }
  void operator^= (const Hash& other) { hash ^= other.hash; }

};


// class Zobrist


class Zobrist {
public:

  FastMap<Move, Hash> hashes;

  Zobrist (PmRandom& pm) {
    player_for_each (pl) vertex_for_each_all (v) {
      Move m = Move (pl, v);
      hashes [m].randomize (pm);
    }
  }

  Hash of_move (Move m) const {
    return hashes [m];
  }

  Hash of_pl_v (Player pl,  Vertex v) const {
    return hashes [Move (pl, v)];
  }

};


// class NbrCounter


class NbrCounter {
public:

  static const uint max = 4;                 // maximal number of neighbours
  static const uint f_size = 4;              // size in bits of each of 3 counters in nbr_cnt::t
  static const uint f_mask = (1 << f_size) - 1;
  static const uint f_shift_black = 0 * f_size;
  static const uint f_shift_white = 1 * f_size;
  static const uint f_shift_empty = 2 * f_size;
  static const uint black_inc_val = (1 << f_shift_black) - (1 << f_shift_empty);
  static const uint white_inc_val = (1 << f_shift_white) - (1 << f_shift_empty);
  static const uint off_board_inc_val = (1 << f_shift_black) + (1 << f_shift_white) - (1 << f_shift_empty);
  
public:

  uint bitfield;

  NbrCounter () { }
  
  NbrCounter (uint black_cnt, uint white_cnt, uint empty_cnt) {
    assertc (nbr_cnt_ac, black_cnt <= max);
    assertc (nbr_cnt_ac, white_cnt <= max);
    assertc (nbr_cnt_ac, empty_cnt <= max);
    bitfield = 
      (black_cnt << f_shift_black) +
      (white_cnt << f_shift_white) +
      (empty_cnt << f_shift_empty);
  }
  
  static const uint player_inc_tab [Player::cnt];
  void player_inc (Player player) { bitfield += player_inc_tab [player.get_idx ()]; }
  void player_dec (Player player) { bitfield -= player_inc_tab [player.get_idx ()]; }
  void off_board_inc () { bitfield += off_board_inc_val; }

  uint empty_cnt  () const { return bitfield >> f_shift_empty; }

  static const uint f_shift [3];
  uint player_cnt (Player pl) const { return (bitfield >> f_shift [pl.get_idx ()]) & f_mask; }

  static const uint player_cnt_is_max_mask [Player::cnt];
  uint player_cnt_is_max (Player pl) const { 
    return 
      (player_cnt_is_max_mask [pl.get_idx ()] & bitfield) == 
       player_cnt_is_max_mask [pl.get_idx ()]; 
  }

  void check () {
    if (!nbr_cnt_ac) return;
    assert (empty_cnt () <= max);
    assert (player_cnt (Player::black ()) <= max);
    assert (player_cnt (Player::white ()) <= max);
  }
};

const uint NbrCounter::f_shift [3] = { 
  NbrCounter::f_shift_black, 
  NbrCounter::f_shift_white, 
  NbrCounter::f_shift_empty, 
};

const uint NbrCounter::player_cnt_is_max_mask [Player::cnt] = {  // TODO player_Map
  (max << f_shift_black), 
  (max << f_shift_white) 
};

const uint NbrCounter::player_inc_tab [Player::cnt] = { NbrCounter::black_inc_val, NbrCounter::white_inc_val };


// class Board


enum play_ret_t { play_ok, play_suicide, play_ss_suicide, play_ko };

PmRandom zobrist_pm;
const Zobrist zobrist[1] = { Zobrist (zobrist_pm) }; // TODO move it to board

class Board {

public:


  FastMap<Vertex, Color>       color_at;
  FastMap<Vertex, NbrCounter>  nbr_cnt; // incremental, for fast eye checking
  FastMap<Vertex, uint>        empty_pos;
  FastMap<Vertex, Vertex>      chain_next_v;

  uint                         chain_lib_cnt [Vertex::cnt]; // indexed by chain_id
  FastMap<Vertex, uint>        chain_id;
  
  Vertex                       empty_v [board_area];
  uint                         empty_v_cnt;
  uint                         last_empty_v_cnt;

  FastMap<Player, uint>        player_v_cnt;
  FastMap<Player, Vertex>      player_last_v;

  Hash                         hash;
  int                          komi;

  Vertex                       ko_v;             // vertex forbidden by ko

  Player                       last_player;      // player who made the last play (other::player is forbidden to retake)
  uint                         move_no;

  play_ret_t                   last_move_status;
  Move                         move_history [max_game_length];

public:                         // macros


  #define empty_v_for_each(board, vv, i) {                              \
    Vertex vv = Vertex::any ();                                     \
    rep (ev_i, (board)->empty_v_cnt) {                                  \
      vv = (board)->empty_v [ev_i];                                     \
      i;                                                                \
    }                                                                   \
  }

  
  #define empty_v_for_each_and_pass(board, vv, i) {                     \
    Vertex vv = Vertex::pass ();                                    \
    i;                                                                  \
    rep (ev_i, (board)->empty_v_cnt) {                                  \
      vv = (board)->empty_v [ev_i];                                     \
      i;                                                                \
    }                                                                   \
  }


public:                         // consistency checks


  void check_empty_v () const {
    if (!board_empty_v_ac) return;

    FastMap<Vertex, bool> noticed;
    FastMap<Player, uint> exp_player_v_cnt;

    vertex_for_each_all (v) noticed[v] = false;

    assert (empty_v_cnt <= board_area);

    empty_v_for_each (this, v, {
      assert (noticed [v] == false);
      noticed [v] = true;
    });

    player_for_each (pl) exp_player_v_cnt [pl] = 0;

    vertex_for_each_all (v) {
      assert ((color_at[v] == Color::empty ()) == noticed[v]);
      if (color_at[v] == Color::empty ()) {
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
      assert ((color_at[v] != Color::off_board()) == (v.is_on_board ()));
    }
  }


  void check_nbr_cnt () const {
    if (!board_nbr_cnt_ac) return;
    
    vertex_for_each_all (v) {
      coord::t r;
      coord::t c;
      FastMap<Color, uint> nbr_color_cnt;
      uint expected_nbr_cnt;

      if (color_at[v] == Color::off_board()) continue; // TODO is that right?

      r = v.row ();
      c = v.col ();

      assert (coord::is_on_board (r)); // checking the macro
      assert (coord::is_on_board (c));
          
      color_for_each (col) nbr_color_cnt [col] = 0;
          
      vertex_for_each_nbr (v, nbr_v, nbr_color_cnt [color_at [nbr_v]]++);
          
      expected_nbr_cnt =        // definition of nbr_cnt[v]
        + ((nbr_color_cnt [Color::black ()] + nbr_color_cnt [Color::off_board ()]) 
           << NbrCounter::f_shift_black)
        + ((nbr_color_cnt [Color::white ()] + nbr_color_cnt [Color::off_board ()])
           << NbrCounter::f_shift_white)
        + ((nbr_color_cnt [Color::empty ()]) 
           << NbrCounter::f_shift_empty);
    
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


  void check_no_more_legal (Player player) { // at the end of the playout
    unused (player);

    if (!board_ac) return;

    vertex_for_each_all (v)
      if (color_at[v] == Color::empty ())
        assert (is_eyelike (player, v) || is_pseudo_legal (player, v) == false);
  }


public:                         // board interface


  Board () { 
    clear (); 
    cout << ""; // TODO remove this stupid statement
  }

  
  void clear () {
    uint off_board_cnt;

    set_komi (default_komi);            // standard for chinese rules
    empty_v_cnt  = 0;
    player_for_each (pl) {
      player_v_cnt  [pl] = 0;
      player_last_v [pl] = Vertex::any ();
    }
    move_no      = 0;
    last_player  = Player::white (); // act player is other
    last_move_status = play_ok;
    ko_v         = Vertex::any ();
    vertex_for_each_all (v) {
      color_at      [v] = Color::off_board ();
      nbr_cnt       [v] = NbrCounter (0, 0, NbrCounter::max);
      chain_next_v  [v] = v;
      chain_id      [v] = v.get_idx ();    // TODO is it needed, is it usedt?
      chain_lib_cnt [v.get_idx ()] = NbrCounter::max; // TODO is it logical? (off_boards)

      if (v.is_on_board ()) {
        color_at   [v]              = Color::empty ();
        empty_pos  [v]              = empty_v_cnt;
        empty_v    [empty_v_cnt++]  = v;

        off_board_cnt = 0;
        vertex_for_each_nbr (v, nbr_v, {
            if (!nbr_v.is_on_board ()) 
              off_board_cnt++;
        });
        rep (ii, off_board_cnt) 
          nbr_cnt [v].off_board_inc ();
      }
    }

    hash = recalc_hash ();

    check ();
  }


  Hash recalc_hash () const {
    Hash new_hash;

    new_hash.set_zero ();

    vertex_for_each_all (v) {
      if (color_at [v].is_player ()) {
        new_hash ^= zobrist->of_pl_v (color_at [v].to_player (), v);
      }
    }
    
    return new_hash;
  }


  void load (const Board* save_board) { 
    memcpy(this, save_board, sizeof(Board)); 
    check ();
  }
  

  void set_komi (float fkomi) { 
    komi = int (ceil (-fkomi));
  }


  float get_komi () const { 
    return float(-komi) + 0.5;
  }


public: // legality functions


  // checks for move legality
  // it has to point to empty vertexand empty
  // can't recognize play_suicide
  flatten all_inline 
  bool is_pseudo_legal (Player player, Vertex v) {
    check ();
    //v.check_is_on_board (); // TODO check v = pass || onboard

    return 
      v == Vertex::pass () || 
      !nbr_cnt[v].player_cnt_is_max (player.other ()) || 
      (!play_eye_is_ko (player, v) && 
       !play_eye_is_suicide (v));
  }


  // a very slow function
  bool is_strict_legal (Player pl, Vertex v) {            // slow function
    if (try_play (pl, v) == false) return false;
    sure_undo ();
    return true;
  }


  bool is_eyelike (Player player, Vertex v) { 
    assertc (board_ac, color_at [v] == Color::empty ());
    if (! nbr_cnt[v].player_cnt_is_max (player)) return false;

    FastMap<Color, int> diag_color_cnt; // TODO
    color_for_each (col) 
      diag_color_cnt [col] = 0; // memset is slower

    vertex_for_each_diag_nbr (v, diag_v, {
      diag_color_cnt [color_at [diag_v]]++;
    });

    return diag_color_cnt [player.other ()] + (diag_color_cnt [Color::off_board ()] > 0) < 2;
  }


  // this is very very slow function
  bool is_hash_repeated () {
    Board tmp_board;
    rep (mn, move_no-1) {
      tmp_board.play_legal (move_history [mn].get_player (), move_history [mn].get_vertex ());
      if (hash == tmp_board.hash) 
        return true;
    }
    return false;
  }


public: // play move functions


  // very slow function
  bool try_play (Player player, Vertex v) {
    if (v == Vertex::pass ()) {
      play_legal (player, v);      
      return true;
    }

    v.check_is_on_board ();

    if (color_at [v] != Color::empty ()) 
      return false; 

    if (is_pseudo_legal (player,v) == false)
      return false;

    play_legal (player, v);

    if (last_move_status != play_ok) {
      sure_undo ();
      return false;
    }

    if (is_hash_repeated ()) {
      sure_undo ();
      return false;
    }

    return true;
  }


  // accept pass
  // will ignore simple-ko ban
  // will play single stone suicide
  void play_legal (Player player, Vertex v) flatten all_inline {
    check ();

    if (v == Vertex::pass ()) {
      basic_play (player, Vertex::pass ());
      return;
    }
    
    v.check_is_on_board ();
    assertc (board_ac, color_at[v] == Color::empty ());
    
    if (nbr_cnt[v].player_cnt_is_max (player.other ())) {
      play_eye_legal (player, v);
    } else {
      play_not_eye (player, v);
      assertc (board_ac, last_move_status == play_ok || last_move_status == play_suicide); // TODO invent complete suicide testing
    }
  
  }

  
  // very slow function
  bool undo () {
    Move replay [max_game_length];

    uint   game_length  = move_no;
    float  old_komi     = get_komi ();

    if (game_length == 0) 
      return false;

    rep (mn, game_length-1)
      replay [mn] = move_history [mn];

    clear ();
    set_komi (old_komi); // TODO maybe last_player should be preserverd as well

    rep (mn, game_length-1)
      play_legal (replay [mn].get_player (), replay [mn].get_vertex ());

    return true;
  }


  // very slow function 
  void sure_undo () {
    if (undo () == false) {
      cerr << "sure_undo failed\n";
      exit (1);
    }
  }

public: // auxiliary functions


  bool play_eye_is_ko (Player player, Vertex v) {
    return (v == ko_v) & (player == last_player.other ());
  }


  bool play_eye_is_suicide (Vertex v) {
    uint all_nbr_live = true;
    vertex_for_each_nbr (v, nbr_v, all_nbr_live &= (--chain_lib_cnt [chain_id [nbr_v]] != 0));
    vertex_for_each_nbr (v, nbr_v, chain_lib_cnt [chain_id [nbr_v]]++);
    return all_nbr_live;
  }


  all_inline
  void play_not_eye (Player player, Vertex v) {
    check ();
    v.check_is_on_board ();
    assertc (board_ac, color_at[v] == Color::empty ());

    basic_play (player, v);

    place_stone (player, v);

    vertex_for_each_nbr (v, nbr_v, {

      nbr_cnt [nbr_v].player_inc (player);
        
      if (color_at [nbr_v].is_player ()) {
        chain_lib_cnt [chain_id [nbr_v]] --; // This should be before 'if' to have good lib_cnt for empty vertices
      
        if (color_at [nbr_v] != Color (player)) { // same color of groups
          if (chain_lib_cnt [chain_id [nbr_v]] == 0) 
            remove_chain (nbr_v);
        } else {
          if (chain_id [nbr_v] != chain_id [v]) {
            if (chain_lib_cnt [chain_id [v]] > chain_lib_cnt [chain_id [nbr_v]]) {
               merge_chains (v, nbr_v);
            } else {
               merge_chains (nbr_v, v);
            }         
          }
        }
      }
    });
    
    if (chain_lib_cnt [chain_id [v]] == 0) {
      assertc (board_ac, last_empty_v_cnt - empty_v_cnt == 1);
      remove_chain(v);
      assertc (board_ac, last_empty_v_cnt - empty_v_cnt > 0);
      last_move_status = play_suicide;
    } else {
      last_move_status = play_ok;
    }
  }


  no_inline
  void play_eye_legal (Player player, Vertex v) {
    vertex_for_each_nbr (v, nbr_v, chain_lib_cnt [chain_id [nbr_v]]--);

    basic_play (player, v);
    place_stone (player, v);
    
    vertex_for_each_nbr (v, nbr_v, { 
      nbr_cnt [nbr_v].player_inc (player);
    });

    vertex_for_each_nbr (v, nbr_v, {
      if ((chain_lib_cnt [chain_id [nbr_v]] == 0)) 
        remove_chain (nbr_v);
    });

    assertc (board_ac, chain_lib_cnt [chain_id [v]] != 0);

    if (last_empty_v_cnt == empty_v_cnt) { // if captured exactly one stone, end this was eye
      ko_v = empty_v [empty_v_cnt - 1]; // then ko formed
    } else {
      ko_v = Vertex::any ();
    }
  }


  void basic_play (Player player, Vertex v) { // Warning: has to be called before place_stone, because of hash storing
    assertc (board_ac, move_no <= max_game_length);
    ko_v                    = Vertex::any ();
    last_empty_v_cnt        = empty_v_cnt;
    last_player             = player;
    player_last_v [player]  = v;
    move_history [move_no]  = Move (player, v);
    move_no                += 1;
  }


  void merge_chains (Vertex v_base, Vertex v_new) {
    Vertex act_v;

    chain_lib_cnt [chain_id [v_base]] += chain_lib_cnt [chain_id [v_new]];

    act_v = v_new;
    do {
      chain_id [act_v] = chain_id [v_base];
      act_v = chain_next_v [act_v];
    } while (act_v != v_new);
    
    swap (chain_next_v[v_base], chain_next_v[v_new]);
  }


  no_inline 
  void remove_chain (Vertex v){
    Vertex act_v;
    Vertex tmp_v;
    Color old_color;

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
        nbr_cnt [nbr_v].player_dec (old_color.to_player());
        chain_lib_cnt [chain_id [nbr_v]]++;
      });

      tmp_v = act_v;
      act_v = chain_next_v[act_v];
      chain_next_v [tmp_v] = tmp_v;
      
    } while (act_v != v);
  }


  void place_stone (Player pl, Vertex v) {
    hash ^= zobrist->of_pl_v (pl, v);
    player_v_cnt[pl]++;
    color_at[v] = Color (pl);

    empty_v_cnt--;
    empty_pos [empty_v [empty_v_cnt]] = empty_pos [v];
    empty_v [empty_pos [v]] = empty_v [empty_v_cnt];

    assertc (chain_next_v_ac, chain_next_v[v] == v);

    chain_id [v] = v.get_idx ();
    chain_lib_cnt [v.get_idx ()] = nbr_cnt[v].empty_cnt ();
  }


  void remove_stone (Vertex v) {
    hash ^= zobrist->of_pl_v (color_at [v].to_player (), v);
    player_v_cnt [color_at[v].to_player ()]--;
    color_at [v] = Color::empty ();

    empty_pos [v] = empty_v_cnt;
    empty_v [empty_v_cnt++] = v;
    chain_id [v] = v.get_idx ();

    assertc (board_ac, empty_v_cnt < Vertex::cnt);
  }


public:                         // utils


  Player act_player () const { return last_player.other (); } // TODO/FIXME last_player should be preserverd in undo function


  bool both_player_pass () {
    return 
      (player_last_v [Player::black ()] == Vertex::pass ()) & 
      (player_last_v [Player::white ()] == Vertex::pass ());
  }


  int approx_score () const {
    return komi + player_v_cnt[Player::black ()] -  player_v_cnt[Player::white ()];
  }


  Player approx_winner () { return Player (approx_score () <= 0); }


  int score () const {
    int eye_score = 0;

    empty_v_for_each (this, v, {
        eye_score += nbr_cnt[v].player_cnt_is_max (Player::black ());
        eye_score -= nbr_cnt[v].player_cnt_is_max (Player::white ());
    });

    return approx_score () + eye_score;
  }


  Player winner () const { 
    return Player (score () <= 0); 
  }


  int vertex_score (Vertex v) {
    //     FastMap<Color, int> Coloro_score;
    //     Coloro_score [Color::black ()] = 1;
    //     Coloro_score [Color::white ()] = -1;
    //     Coloro_score [Color::empty ()] =
    //       (nbr_cnt[v].player_cnt_is_max (Player::black ())) -
    //       (nbr_cnt[v].player_cnt_is_max (Player::white ()));
    //     Coloro_score [Color::off_board ()] = 0;
    //     return Coloro_score [color_at [v]];
    switch (color_at [v].get_idx ()) {
    case Color::black_idx: return 1;
    case Color::white_idx: return -1;
    case Color::empty_idx: 
      return 
        (nbr_cnt[v].player_cnt_is_max (Player::black ())) -
        (nbr_cnt[v].player_cnt_is_max (Player::white ()));
    case Color::off_board_idx: return 0;
    default: assert (false);
    }
  }


  string to_string (Vertex mark_v = Vertex::any ()) const {
    ostringstream out;

    #define os(n)      out << " " << n
    #define o_left(n)  out << "(" << n
    #define o_right(n) out << ")" << n
    
    out << " ";
    if (board_size < 10) out << " "; else out << "  ";
    coord_for_each (col) os (coord::col_to_string (col));
    out << endl;

    coord_for_each (row) {
      if (board_size >= 10 && board_size - row < 10) out << " ";
      os (coord::row_to_string (row));
      coord_for_each (col) {
        Vertex v = Vertex (row, col);
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
    coord_for_each (col) os (coord::col_to_string (col));
    out << endl;

    #undef os
    #undef o_left
    #undef o_right

    return out.str ();
  }


  void print_cerr (Vertex v = Vertex::pass ()) const {
    cerr << to_string (v);
  }


  bool load_from_ascii (istream& ifs) {
    uint     bs;
    char     c;

    Player    play_player[board_area];
    Vertex  play_v[board_area];
    uint      play_cnt;

    clear ();

    play_cnt = 0;

    if (!ifs) return false;

    ifs >> bs;
    if (bs != board_size) return false;

    if (getc_non_space (ifs) != '\n') return false;

    coord_for_each (row) {
      coord_for_each (col) {
        Color color;

        c      = getc_non_space (ifs);
        color  = Color (c); // TODO 0 is to choose the constructor

        if (color == Color::wrong_char ()) return false;
        
        if (color.is_player ()) {
          play_player [play_cnt]  = color.to_player ();
          play_v [play_cnt]       = Vertex (row, col);
          play_cnt += 1;
          assertc (board_ac, play_cnt < board_area);
        }
      }

      if (getc_non_space (ifs) != '\n') return false;
    }

    Board  tmp_board [1];
    rep (pi, play_cnt) {
      if (tmp_board->is_strict_legal (play_player[pi], play_v[pi]) == false) 
        return false;
      bool ret = tmp_board->try_play (play_player[pi], play_v[pi]);
      assertc (board_ac, ret == true);
    }

    this->load (tmp_board);

    // if (false)
    if (color_at[0] == Color::white ()) print_cerr (); // TODO LOL hack - need tu use it somwhere 
    return true;
  }

};
