//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef BOARD_H_
#define BOARD_H_

#include "utils.hpp"
#include "hash.hpp"
#include "color.hpp"


class RawBoard {
public:

  // Constructs empty board.
  RawBoard ();

  // -------------------------------------------------------
  // Quesring board state

  // Return color of board vertex
  Color ColorAt (Vertex v) const;

  // Returs ith empty Vertex.
  Vertex EmptyVertex (uint ii) const;

  // Number of Empty board Vertices.
  uint EmptyVertexCount () const;

  // Returns player on move.
  Player ActPlayer () const;
  
  // Returns player that played last move.
  Player LastPlayer () const;

  // Returns a last played vertex or Vertex::Any()
  Vertex LastVertex () const;

  Move LastMove () const;
  Move LastMove2 () const;

  // Return number of already played moves.
  uint MoveCount () const;

  // Number of moves played at a particular location.
  uint PlayCount (Vertex v) const;

  // Returns true if both players pass.
  bool BothPlayerPass () const;

  // Positional hash (just color of stones)
  Hash PositionalHash () const;

  // Returns vertex forbidden by simple ko rule or Vertex::Any()
  Vertex KoVertex () const;

  // Gets the komi value. Positive means adventage for white.
  float Komi () const;

  // Gets the board size.
  uint Size () const;

  // -------------------------------------------------------
  // Fast playout functions

  // Loads save_board into this board.
  void Load (const RawBoard& save_board);

  // Sets player on move. Play-undo will forget this set.(use pass)
  void SetActPlayer (Player);
  
  // Returns true iff v is uncut eye of the player.
  bool IsEyelike (Player player, Vertex v) const;
  bool IsEyelike (Move move) const;

  // Returns false for simple ko and suicides.
  // Returns true despite superko violation.
  bool IsLegal (Player player, Vertex v) const;
  bool IsLegal (Move m) const;

  // Returns a random light playout move. Returns pass if no light move found.
  Vertex RandomLightMove (Player player, FastRandom& random) const;
  Move RandomLightMove (FastRandom& random) const;

  // Plays a move, returns false if move was large suicide.
  // Assumes IsLegal (player, v) - Do not support suicides.
  void PlayLegal (Player player, Vertex v);
  void PlayLegal (Move move);

  // Difference in (number of stones + number of eyes) of each player - komi.
  // See TrompTaylorScore.
  int PlayoutScore () const;

  // Winner according to PlayoutScore.
  Player PlayoutWinner () const;

  // ------------------------------------------------------
  // Some slow functions needed in some playout situations.

  // Tromp-Taylor score - quite slow function.
  // Scoring uses integers, so to get a true result you need to
  // substract 0.5 (convention is that white wins when score == 0).
  int TrompTaylorScore() const;

  // Winner according to TrompTaylorScore.
  Player TrompTaylorWinner() const;

  // -------------------------------------
  // Auxiliary functions. May/will change.

  // Difference in (number of stones) of each player - komi. Used with
  // mercy heuristic.
  int StoneScore () const;

  
  // -1 if white's eye, +1 if black's eye, 0 otherwise.
  int EyeScore (Vertex v) const;

  // Winner according to StoneScore.
  Player StoneWinner () const;

  // Sets the komi value. Positive means adventage for white.
  void SetKomi (float fkomi);

  string ToAsciiArt (Vertex mark_v = Vertex::Invalid ()) const;

  // debugging helper
  void DebugPrint (Vertex v = Vertex::Pass ()) const;

  // Clears the board. (It is faster to Load(empty_board))
  void Clear ();

  static const uint kArea = board_size * board_size;

private: 

  static const bool kCheckAsserts = false;

  Hash recalc_hash () const;

  void play_eye_legal (Vertex v);

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

  class NbrCounter { // TODO update it to a full 3x3 pattern
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
    mutable uint lib_sum;
    mutable uint lib_sum2;
    void AddLib (Vertex v);
    void SubLib (Vertex v);
    void Merge (const Chain& other);
    bool IsCaptured () const;
  };

  Chain& chain_at (Vertex v);
  const Chain& chain_at (Vertex v) const;

private:

  // Unique board data.

  uint                         move_no;
  int                          komi_inverse;
  NatMap<Vertex, Color>        color_at;
  Vertex                       ko_v;             // vertex forbidden by ko
  Player                       last_player;      // player who made the last play
  NatMap<Player, Vertex>       last_play;

  // Reconstructible board data.

  Hash                         hash;         // Positional hash.

  NatMap<Player, uint>         player_v_cnt; // Sum of numer of stones of each color.

  NatMap<Vertex, Vertex>       chain_next_v; // Next Vertex in chain.
  NatMap<Vertex, Vertex>       chain_id;     // Identical for one chain.
  NatMap<Vertex, Chain>        chain;        // Indexed by chain_id[v]

  NatMap<Vertex, NbrCounter>   nbr_cnt; // 3x3 patterns

  // Incremantal set of empty Vertices.
  // TODO Merge this four members into NatSet
  uint                         empty_v_cnt;
  Vertex                       empty_v [kArea];
  NatMap<Vertex, uint>         empty_pos;

  NatMap<Vertex, uint>         play_count;

  static const Zobrist zobrist[1];

public:
  // This function does nothing. Read comment in benchmark.cpp.
  static void AlignHack(RawBoard&);
};

// -----------------------------------------------------------------------------

class Board : public RawBoard {
public:

  // Clears the board.
  void Clear();

  // Returns legality of move. Implemented by calling Play on a board copy.
  // Includes positional superko detection.
  // Very slow.
  bool IsReallyLegal (Move move) const;

  // Play the move. Assert it is legal.
  void PlayLegal (Player pl, Vertex v);
  void PlayLegal (Move move);

  // Undo move.
  // Very slow.
  bool Undo ();

  // Loads position (and history) from other board.
  void Load (const Board& save_board);

  // Returns list of played moves.
  const vector<Move>& Moves () const;

private:

  bool IsHashRepeated ();

  static const bool kCheckAsserts = false;

  vector<Move> moves;
};


#define empty_v_for_each(board, vv, i) {                                \
    Vertex vv = Vertex::Invalid();                                      \
    rep (ev_i, (board)->EmptyVertexCount()) {                           \
      vv = (board)->EmptyVertex (ev_i);                                 \
      i;                                                                \
    }                                                                   \
  }

#define empty_v_for_each_and_pass(board, vv, i) {                       \
    Vertex vv = Vertex::Pass ();                                        \
    i;                                                                  \
    rep (ev_i, (board)->EmptyVertexCount()) {                           \
      vv = (board)->EmptyVertex (ev_i);                                 \
      i;                                                                \
    }                                                                   \
  }

#endif
