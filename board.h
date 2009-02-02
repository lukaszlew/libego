#ifndef _BOARD_H_
#define _BOARD_H_

class NbrCounter {
public:

  static const uint max;    // maximal number of neighbours
  static const uint f_size; // size in bits of each of 3 counters in nbr_cnt::t
  static const uint f_mask;
  static const uint f_shift_black;
  static const uint f_shift_white;
  static const uint f_shift_empty;
  static const uint black_inc_val;
  static const uint white_inc_val;
  static const uint off_board_inc_val;
  static const uint player_inc_tab [Player::cnt];
  static const uint f_shift [3];
  static const uint player_cnt_is_max_mask [Player::cnt];

  uint bitfield;

  NbrCounter ();
  NbrCounter (uint black_cnt, uint white_cnt, uint empty_cnt);
  void player_inc (Player player);
  void player_dec (Player player);
  void off_board_inc ();
  uint empty_cnt  () const;
  uint player_cnt (Player pl) const;
  uint player_cnt_is_max (Player pl) const;
  void check ();
};

enum play_ret_t { play_ok, play_suicide, play_ss_suicide, play_ko };

#define empty_v_for_each(board, vv, i) {                                \
    Vertex vv = Vertex::any ();                                         \
    rep (ev_i, (board)->empty_v_cnt) {                                  \
      vv = (board)->empty_v [ev_i];                                     \
      i;                                                                \
    }                                                                   \
  }

#define empty_v_for_each_and_pass(board, vv, i) {                       \
    Vertex vv = Vertex::pass ();                                        \
    i;                                                                  \
    rep (ev_i, (board)->empty_v_cnt) {                                  \
      vv = (board)->empty_v [ev_i];                                     \
      i;                                                                \
    }                                                                   \
  }

class Board {
public:                         // board interface

  Board ();

  void clear ();
  Hash recalc_hash () const;
  void load (const Board* save_board);
  void set_komi (float fkomi);
  float get_komi () const;

  // checks for move legality
  // it has to point to empty vertexand empty
  // can't recognize play_suicide
  bool is_pseudo_legal (Player player, Vertex v);

  // a very slow function
  bool is_strict_legal (Player pl, Vertex v);

  bool is_eyelike (Player player, Vertex v);

  // this is very slow function
  bool is_hash_repeated ();


  // accept pass
  // will ignore simple-ko ban
  // will play single stone suicide
  void play_legal (Player player, Vertex v);


  bool try_play (Player player, Vertex v); // slow
  bool undo (); // slow

  bool play_eye_is_ko (Player player, Vertex v);
  bool play_eye_is_suicide (Vertex v);
  void play_not_eye (Player player, Vertex v);
  void play_eye_legal (Player player, Vertex v);
  void basic_play (Player player, Vertex v);
  void merge_chains (Vertex v_base, Vertex v_new);
  void remove_chain (Vertex v);
  void place_stone (Player pl, Vertex v);
  void remove_stone (Vertex v);


  Player act_player () const;
  bool both_player_pass ();
  int approx_score () const;

  Player approx_winner ();
  int score () const;

  Player winner () const;
  int vertex_score (Vertex v);
  bool load_from_ascii (istream& ifs);
  string to_string (Vertex mark_v = Vertex::any ()) const;
  void print_cerr (Vertex v = Vertex::pass ()) const;

  // TODO: move these consistency checks to some some kind of unit testing
  void check_empty_v () const;
  void check_hash () const;
  void check_color_at () const;
  void check_nbr_cnt () const;
  void check_chain_at () const;
  void check_chain_next_v () const;
  void check () const;
  void check_no_more_legal (Player player);

public:

  static const Zobrist zobrist[1];

  FastMap<Vertex, Color>       color_at;
  FastMap<Vertex, NbrCounter>  nbr_cnt; // incremental, for fast eye checking
  FastMap<Vertex, uint>        empty_pos;
  FastMap<Vertex, Vertex>      chain_next_v;

  uint                         chain_lib_cnt [Vertex::cnt]; // indexed by chain_id
  FastMap<Vertex, uint>        chain_id;
  Vertex                       empty_v [board_area];   // TODO use FastStack
  uint                         empty_v_cnt;
  uint                         last_empty_v_cnt;
  FastMap<Player, uint>        player_v_cnt;
  FastMap<Player, Vertex>      player_last_v;
  Hash                         hash;
  int                          komi;
  Vertex                       ko_v;             // vertex forbidden by ko
  Player                       last_player;      // player who made the last play
  uint                         move_no;
  play_ret_t                   last_move_status;
  Move                         move_history [max_game_length]; // TODO FastStack
};

#endif
