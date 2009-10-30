//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef BOARD_H_
#define BOARD_H_

#include "utils.h"
#include "hash.h"
#include "color.h"

// TODO move these to Board

class Board {
public:                         // board interface

  Board ();
  

  /* Slow full rules implementation */


  // Tromp-Taylor score.
  // Scoring uses integers, so to get a true result you need to
  // substract 0.5 (convention is that white wins when score == 0).
  int tt_score() const;

  // Winner according to tt_score.
  Player tt_winner() const;


  /* Fast playout functions */ 


  // Loads save_board into this board.
  void load (const Board* save_board);

  // Returns false for simple ko and single stone suicide.
  bool is_pseudo_legal (Player player, Vertex v) const;

  // Returns true iff v is uncut eye of the player.
  bool is_eyelike (Player player, Vertex v) const;

  // Plays a move. Accepts passes, suicides and ko moves.
  void play_legal (Player player, Vertex v);

  // Returns player on move.
  Player act_player () const;
  
  // Sets player on move. Play-undo will forget this set.(use pass)
  void set_act_player (Player);
  
  // Returns player that played last move.
  Player last_player () const;

  // Returns a last played vertex or Vertex::Any()
  Vertex last_play () const;

  // Returns true if both players pass.
  bool both_player_pass () const;

  // Difference in (number of stones + number of eyes) of each player - komi.
  // See tt_score.
  int playout_score () const;

  // Winner according to playout_score.
  Player playout_winner () const;


  /* Auxiliary functions. May/will change. */


  // Returns 1 (-1) if v is occupied by or is an eye of Black(White).
  // Returns 0 for other empty vertices.
  int vertex_score (Vertex v) const;

  // Difference in (number of stones) of each player - komi. Used with
  // mercy heuristic.
  int approx_score () const;

  // Winner according to approx_score.
  Player approx_winner () const;

  // Gets, sets the komi value. Positive means adventage for white.
  float komi () const;
  void set_komi (float fkomi);

  // Positional hash (just color of stones)
  Hash hash () const;

  // Returns vertex forbidden by simple ko rule or Vertex::Any()
  Vertex ko_v () const;

  string to_string (Vertex mark_v = Vertex::Any ()) const;

  // debugging helper
  void print_cerr (Vertex v = Vertex::Pass ()) const;

  uint last_capture_size () const;

  Move last_move () const;

  // Clears the board. (It is faster to load(empty_board))
  void clear ();

public:

  static const uint area = board_size * board_size;

private: 

  Hash recalc_hash () const;

  bool eye_is_ko (Player player, Vertex v) const;
  bool eye_is_suicide (Vertex v) const;

  void basic_play (Player player, Vertex v);
  void play_not_eye (Player player, Vertex v);
  void play_eye_legal (Player player, Vertex v);

  void update_neighbour (Player player, Vertex v, Vertex nbr_v);
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
  void check_no_more_legal (Player player) const;

  class NbrCounter {
  public:
    static NbrCounter Empty();

    void player_inc (Player player);
    void player_dec (Player player);
    void off_board_inc ();
    uint empty_cnt  () const;
    uint player_cnt (Player pl) const;
    uint player_cnt_is_max (Player pl) const;
    void check () const;
    void check (const NatMap<Color, uint>& nbr_color_cnt) const;

    static const uint max;    // maximal number of neighbours

  private:
    uint bitfield;

    static NbrCounter OfCounts(uint black_cnt, uint white_cnt, uint empty_cnt);

    static const uint f_size; // size in bits of each of 3 counters in nbr_cnt::t
    static const uint player_inc_tab [Player::kBound];
    static const uint f_shift [3];
    static const uint player_cnt_is_max_mask [Player::kBound];
  };

  struct Chain {
    mutable uint lib_cnt;
  };

  Chain& chain_at (Vertex v);
  const Chain& chain_at (Vertex v) const;

public:
  // TODO make iterators / accessors
  NatMap<Vertex, Color>   color_at;

  Vertex                   empty_v [area]; // TODO use FastSet (empty_pos)
  uint                     empty_v_cnt;
  uint                     move_no;

  enum {
    play_ok,
    play_suicide,
    play_ss_suicide,
    play_ko
  } last_move_status;

private:
  int komi_inverse_;

  static const Zobrist zobrist[1];

  NatMap<Vertex, NbrCounter>   nbr_cnt; // incremental, for fast eye checking
  NatMap<Vertex, uint>         empty_pos; // liberty position in empty_v
  NatMap<Vertex, Vertex>       chain_next_v;

  NatMap<Vertex, Vertex>       chain_id_;
  NatMap<Vertex, Chain>        chain_; // indexed by chain_id[v]

  uint                         last_empty_v_cnt;
  NatMap<Player, uint>         player_v_cnt;
  NatMap<Player, Vertex>       last_play_;
  Hash                         hash_;
  Vertex                       ko_v_;             // vertex forbidden by ko
  Player                       last_player_;      // player who made the last play
};

#define empty_v_for_each(board, vv, i) {                                \
    Vertex vv = Vertex::Any ();                                         \
    rep (ev_i, (board)->empty_v_cnt) {                                  \
      vv = (board)->empty_v [ev_i];                                     \
      i;                                                                \
    }                                                                   \
  }

#define empty_v_for_each_and_pass(board, vv, i) {                       \
    Vertex vv = Vertex::Pass ();                                        \
    i;                                                                  \
    rep (ev_i, (board)->empty_v_cnt) {                                  \
      vv = (board)->empty_v [ev_i];                                     \
      i;                                                                \
    }                                                                   \
  }

#endif
