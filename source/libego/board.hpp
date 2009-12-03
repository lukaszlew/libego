//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef BOARD_H_
#define BOARD_H_

#include "utils.hpp"
#include "hash.hpp"
#include "color.hpp"

// should be power of 2
const uint kMaxPatternsPerVertex = 8; // max 8 3x3 patterns per vertex

// -----------------------------------------------------------------------------

class PatternId : public Nat <PatternId> {
public:
  PatternId () {};
  // TODO OfVertex 
  static const uint kBound = (board_size + 2) * (board_size + 2);
private:
  // maximal number of patterns supported by board.
  // should be power of 2 for maximal efficiency - 1 TODO CHECK IT

private:
  friend class Nat <PatternId>;
  explicit PatternId (uint raw);
};

// -----------------------------------------------------------------------------

class BoardShared {
  BoardShared ()
    : pattern_count_at (0) , pattern_hash_base (), last_pattern (PatternId::Invalid())
  {
    ForEachNat (PatternId, pid) {
      pattern_hash_base [pid].SetZero();
    }
  }

  PatternId AddPattern () {
    CHECK (last_pattern.MoveNext());
    return last_pattern;
  }

  void AddVertexToPattern (PatternId pid,
                           Vertex v,
                           Hash black_addend,
                           Hash white_addend)
  {
    uint& pca = pattern_count_at[v];
    ASSERT (pca < kMaxPatternsPerVertex);
    patterns_at [v] [pca] = pid;
    hash_addends_at [v] [pca] [Player::Black()] = black_addend;
    hash_addends_at [v] [pca] [Player::White()] = white_addend;
    pca += 1;
  }

  static const bool kCheckAsserts = true;

private:
  friend class Board;
  NatMap<Vertex, uint> pattern_count_at;
  NatMap<Vertex, PatternId [kMaxPatternsPerVertex] > patterns_at;
  NatMap<PatternId, Hash> pattern_hash_base;
  NatMap<Vertex, NatMap<Player, Hash> [kMaxPatternsPerVertex] > hash_addends_at;
  PatternId last_pattern;
};

// -----------------------------------------------------------------------------

class Board {
public:

  // Constructs empty board.
  Board ();

  // -------------------------------------------------------
  // Quesring board state

  // Return color of board vertex
  Color ColorAt (Vertex v) const;

  // Returs array of Vertices that are empty.
  Vertex EmptyVertex (uint ii) const;

  // Length of the array return by EmptyVertices()
  uint EmptyVertexCount () const;

  // Returns player on move.
  Player ActPlayer () const;
  
  // Returns player that played last move.
  Player LastPlayer () const;

  // Returns a last played vertex or Vertex::Any()
  Vertex LastVertex () const;

  Move LastMove () const;

  // Return number of already played moves.
  uint MoveCount () const;

  // Returns true if both players pass.
  bool BothPlayerPass () const;

  // Positional hash (just color of stones)
  Hash PositionalHash () const;

  // Returns vertex forbidden by simple ko rule or Vertex::Invalid()
  Vertex KoVertex () const;

  // Gets the komi value. Positive means adventage for white.
  float Komi () const;

  // Gets the board size.
  uint Size () const;

  // -------------------------------------------------------
  // Fast playout functions

  // Loads save_board into this board.
  void Load (const Board& save_board);

  // Sets player on move. Play-undo will forget this set.(use pass)
  void SetActPlayer (Player);
  
  // Returns true iff v is uncut eye of the player.
  bool IsEyelike (Player player, Vertex v) const;

  // Returns false for simple ko and single stone suicide.
  // Returns true despite bigger suicides and superko violation.
  bool IsPseudoLegal (Player player, Vertex v) const;

  // Plays a move, returns false if move was large suicide.
  // Assumes IsPseudoLegal (player, v) - Do not support single stone suicides.
  bool PlayPseudoLegal (Player player, Vertex v);

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

  // Use when you add a pattern to Board::shared.
  void RecalculatePatternHashes ();

  // Difference in (number of stones) of each player - komi. Used with
  // mercy heuristic.
  int StoneScore () const;

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

  bool eye_is_ko (Player player, Vertex v) const;
  bool eye_is_suicide (Vertex v) const;

  void basic_play (Player player, Vertex v);
  bool play_not_eye (Player player, Vertex v);
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
    mutable uint lib_cnt; // TODO move this out.
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
  NatMap<PatternId, Hash>      pattern_hash;

  // Incremantal set of empty Vertices.
  // TODO Merge this four members into NatSet
  uint                         empty_v_cnt;
  uint                         last_empty_v_cnt;
  Vertex                       empty_v [kArea];
  NatMap<Vertex, uint>         empty_pos;

  static const Zobrist zobrist[1];
  static const BoardShared shared;

public:
  // This function does nothing. Read comment in benchmark.cpp.
  static void AlignHack(Board&);
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
