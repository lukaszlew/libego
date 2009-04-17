#ifndef _BOARD_H_
#define _BOARD_H_

// TODO move these to Board
const float default_komi       = 7.5;
const uint  board_area         = board_size * board_size;
const uint  max_empty_v_cnt    = board_area;
const uint  max_game_length    = board_area * 4;

class Board {
public:                         // board interface

  Board ();

  /* slow game functions */

  void clear ();

  void set_komi (float fkomi);

  float get_komi () const;

  bool is_strict_legal (Player pl, Vertex v);

  bool try_play (Player player, Vertex v);

  bool undo ();

  bool is_hash_repeated ();

  /* playout functions */ 

  // can be use to initialize playout board
  void load (const Board* save_board);

  // v has to point to empty vertex
  // can't recognize play_suicide, will recognize ko
  bool is_pseudo_legal (Player player, Vertex v);

  bool is_eyelike (Player player, Vertex v);

  // accept pass
  // will ignore simple-ko ban
  // will play single stone suicide
  void play_legal (Player player, Vertex v);

  Player act_player () const;

  bool both_player_pass ();

  /* scoring functions */

  // single vertex ownership
  int vertex_score (Vertex v);

  int approx_score () const;
  int score () const;

  Player approx_winner () const;
  Player winner () const;

  /* auxiliary functions */

  bool load_from_ascii (istream& ifs);
  string to_string (Vertex mark_v = Vertex::any ()) const;

  // debugging helper
  void print_cerr (Vertex v = Vertex::pass ()) const;

public:
  // TODO make iterators / accessors
  FastMap<Vertex, Color>   color_at;

  Vertex                   empty_v [board_area]; // TODO use FastSet (empty_pos)
  uint                     empty_v_cnt;

  Move                     move_history [max_game_length]; // TODO FastStack
  uint                     move_no;

  enum {
    play_ok,
    play_suicide,
    play_ss_suicide,
    play_ko
  } last_move_status;

  int                      komi;

private: 
  Hash recalc_hash () const;

  bool eye_is_ko (Player player, Vertex v);
  bool eye_is_suicide (Vertex v);

  void basic_play (Player player, Vertex v);
  void play_not_eye (Player player, Vertex v);
  void play_eye_legal (Player player, Vertex v);

  void merge_chains (Vertex v_base, Vertex v_new);
  void remove_chain (Vertex v);
  void place_stone (Player pl, Vertex v);
  void remove_stone (Vertex v);


  // TODO: move these consistency checks to some some kind of unit testing
  void check_empty_v () const;
  void check_hash () const;
  void check_color_at () const;
  void check_nbr_cnt () const;
  void check_chain_at () const;
  void check_chain_next_v () const;
  void check () const;
  void check_no_more_legal (Player player);

  class NbrCounter {
  public:
    static NbrCounter Empty();

    void player_inc (Player player);
    void player_dec (Player player);
    void off_board_inc ();
    uint empty_cnt  () const;
    uint player_cnt (Player pl) const;
    uint player_cnt_is_max (Player pl) const;
    void check ();
    void check (const FastMap<Color, uint>& nbr_color_cnt) const;

    static const uint max;    // maximal number of neighbours

  private:
    uint bitfield;

    static NbrCounter OfCounts(uint black_cnt, uint white_cnt, uint empty_cnt);

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
  };

private:

  static const Zobrist zobrist[1];

  FastMap<Vertex, NbrCounter>  nbr_cnt; // incremental, for fast eye checking
  FastMap<Vertex, uint>        empty_pos; // liberty position in empty_v
  FastMap<Vertex, Vertex>      chain_next_v;
  FastMap<Vertex, Vertex>      chain_id;
  FastMap<Vertex, uint>        chain_lib_cnt; // indexed by chain_id

  uint                         last_empty_v_cnt;
  FastMap<Player, uint>        player_v_cnt;
  FastMap<Player, Vertex>      player_last_v;
  Hash                         hash;
  Vertex                       ko_v;             // vertex forbidden by ko
  Player                       last_player;      // player who made the last play
};

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

#endif
