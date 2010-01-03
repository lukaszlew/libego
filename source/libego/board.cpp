//
// Copyright 2006 and onwards, Lukasz Lew
//

#include <cmath>
#include <cstring>
#include <iostream>

#include "board.hpp"
#include "fast_stack.hpp"

// TODO    center_v.check_is_on_board ();
#define vertex_for_each_4_nbr(center_v, nbr_v, block) { \
    Vertex nbr_v = Vertex::Invalid();                   \
    nbr_v = center_v.N (); block;                       \
    nbr_v = center_v.W (); block;                       \
    nbr_v = center_v.E (); block;                       \
    nbr_v = center_v.S (); block;                       \
  }

// TODO center_v.check_is_on_board ();
#define vertex_for_each_diag_nbr(center_v, nbr_v, block) {      \
    Vertex nbr_v = Vertex::Invalid();                           \
    nbr_v = center_v.NW (); block;                              \
    nbr_v = center_v.NE (); block;                              \
    nbr_v = center_v.SW (); block;                              \
    nbr_v = center_v.SE (); block;                              \
  }


Board::NbrCounter Board::NbrCounter::OfCounts (uint black_cnt,
                                               uint white_cnt,
                                               uint empty_cnt) {
  ASSERT (black_cnt <= max);
  ASSERT (white_cnt <= max);
  ASSERT (empty_cnt <= max);
  Board::NbrCounter nc;
  nc.bitfield =
    (black_cnt << f_shift[0]) +
    (white_cnt << f_shift[1]) +
    (empty_cnt << f_shift[2]);
  return nc;
}

Board::NbrCounter Board::NbrCounter::Empty () {
  return OfCounts(0, 0, max); 
}

void Board::NbrCounter::player_inc (Player player) {
  bitfield += player_inc_tab [player.GetRaw ()]; 
}

void Board::NbrCounter::player_dec (Player player) {
  bitfield -= player_inc_tab [player.GetRaw ()]; 
}

void Board::NbrCounter::off_board_inc () { 
  static const uint off_board_inc_val = 
    (1 << f_shift[0]) + (1 << f_shift[1]) - (1 << f_shift[2]);
  bitfield += off_board_inc_val; 
}

uint Board::NbrCounter::empty_cnt () const {
  return bitfield >> f_shift[2]; 
}

uint Board::NbrCounter::player_cnt (Player pl) const { 
  static const uint f_mask = (1 << f_size) - 1;
  return (bitfield >> f_shift [pl.GetRaw ()]) & f_mask; 
}

uint Board::NbrCounter::player_cnt_is_max (Player pl) const {
  return
    (player_cnt_is_max_mask [pl.GetRaw ()] & bitfield) ==
    player_cnt_is_max_mask [pl.GetRaw ()];
}

void Board::NbrCounter::check () const {
  if (!kCheckAsserts) return;
  ASSERT (empty_cnt () <= max);
  ASSERT (player_cnt (Player::Black ()) <= max);
  ASSERT (player_cnt (Player::White ()) <= max);
}

void Board::NbrCounter::check(const NatMap<Color, uint>& nbr_color_cnt) const {
  if (!kCheckAsserts) return;

  uint expected_nbr_cnt =        // definition of nbr_cnt[v]
    + ((nbr_color_cnt [Color::Black ()] + nbr_color_cnt [Color::OffBoard ()])
       << f_shift[0])
    + ((nbr_color_cnt [Color::White ()] + nbr_color_cnt [Color::OffBoard ()])
       << f_shift[1])
    + ((nbr_color_cnt [Color::Empty ()])
       << f_shift[2]);
  ASSERT (bitfield == expected_nbr_cnt);
}

const uint Board::NbrCounter::max = 4;    // maximal number of neighbours
const uint Board::NbrCounter::f_size = 4; // size in bits of each of 3 counters
const uint Board::NbrCounter::f_shift [3] = {
  0 * f_size,
  1 * f_size,
  2 * f_size,
};

const uint Board::NbrCounter::player_cnt_is_max_mask [Player::kBound] = {  // TODO player_Map
  (max << f_shift[0]),
  (max << f_shift[1])
};

const uint Board::NbrCounter::player_inc_tab [Player::kBound] = {
  (1 << f_shift[0]) - (1 << f_shift[2]),
  (1 << f_shift[1]) - (1 << f_shift[2]),
};


// -----------------------------------------------------------------------------


string Board::ToAsciiArt (Vertex mark_v) const {
  ostringstream out;

#define coord_for_each(rc) for (int rc = 0; rc < int(board_size); rc += 1)
#define os(n)      out << " " << n
#define o_left(n)  out << "(" << n
#define o_right(n) out << ")" << n

  out << " ";
  if (board_size < 10) out << " "; else out << "  ";
  coord_for_each (col) os (Coord::ColumnToGtpString (col));
  out << endl;

  coord_for_each (row) {
    if (board_size >= 10 && board_size - row < 10) out << " ";
    os (Coord::RowToGtpString (row));
    coord_for_each (col) {
      Vertex v = Vertex::OfCoords (row, col);
      char ch = color_at [v].ToShowboardChar ();
      if      (v == mark_v)        o_left  (ch);
      else if (v == mark_v.E ())   o_right (ch);
      else                         os (ch);
    }
    if (board_size >= 10 && board_size - row < 10) out << " ";
    os (Coord::RowToGtpString (row));
    out << endl;
  }

  if (board_size < 10) out << "  "; else out << "   ";
  coord_for_each (col) os (Coord::ColumnToGtpString (col));
  out << endl;

#undef coord_for_each
#undef os
#undef o_left
#undef o_right

  return out.str ();
}


void Board::DebugPrint (Vertex v) const {
  cerr << ToAsciiArt (v);
}


void Board::Clear () {
  empty_v_cnt = 0;
  ForEachNat (Player, pl) {
    player_v_cnt [pl] = 0;
    last_play [pl]    = Vertex::Invalid();
  }
  move_no      = 0;
  last_player  = Player::White (); // act player is other
  ko_v         = Vertex::Invalid();
  ForEachNat (Vertex, v) {
    color_at      [v] = Color::OffBoard ();
    nbr_cnt       [v] = NbrCounter::Empty();
    chain_next_v  [v] = v;
    chain_id      [v] = v;      // TODO is it needed, is it used?
    chain[v].lib_cnt = NbrCounter::max; // TODO off_boards?

    if (v.IsOnBoard ()) {
      color_at   [v]              = Color::Empty ();
      empty_pos  [v]              = empty_v_cnt;
      empty_v    [empty_v_cnt++]  = v;

      vertex_for_each_4_nbr (v, nbr_v, {
        if (!nbr_v.IsOnBoard()) {
          nbr_cnt [v].off_board_inc ();
        }
      });
    }
  }

  hash = recalc_hash ();

  check ();
}


Hash Board::recalc_hash () const {
  Hash new_hash;

  new_hash.SetZero ();

  ForEachNat (Vertex, v) {
    if (color_at [v].IsPlayer ()) {
      new_hash ^= zobrist->OfPlayerVertex (color_at [v].ToPlayer (), v);
    }
  }

  return new_hash;
}


Board::Board () :
  color_at (Color::OffBoard()),
  last_player (Player::White()),
  last_play (Vertex::Invalid()),
  player_v_cnt (0),
  chain_next_v (Vertex::Invalid()),
  chain_id (Vertex::Invalid()),
  chain (Chain ()),
  nbr_cnt (NbrCounter()),
  empty_pos (0)
{
  Clear ();
  SetKomi (6.5);
}


Color Board::ColorAt (Vertex v) const {
  return color_at [v];
}

uint Board::MoveCount () const {
  return move_no;
}

Vertex Board::EmptyVertex (uint ii) const {
  ASSERT (ii < EmptyVertexCount());
  return empty_v [ii];
}

uint Board::EmptyVertexCount () const {
  return empty_v_cnt;
}


void Board::Load (const Board& save_board) {
  memcpy(this, &save_board, sizeof(Board));
  check ();
}

void Board::SetKomi (float fkomi) {
  komi_inverse = int (ceil (-fkomi));
}


float Board::Komi () const {
  return -float(komi_inverse) + 0.5;
}

uint Board::Size () const {
  return board_size;
}

Vertex Board::KoVertex () const {
  return ko_v;
}

Hash Board::PositionalHash () const {
  return hash;
}


bool Board::IsLegal (Player player, Vertex v) const {
  if (v == Vertex::Pass ()) return true;
  if ((color_at [v] != Color::Empty ()) | (v == ko_v)) return false;

  // check for suicide
  if (nbr_cnt[v].empty_cnt () > 0) return true;
  bool not_suicide = false;

  vertex_for_each_4_nbr (v, nbr_v, chain_at (nbr_v).lib_cnt -= 1);

  vertex_for_each_4_nbr (v, nbr_v, {
    bool atari = chain_at (nbr_v).lib_cnt == 0;
    not_suicide |=
      color_at [nbr_v].IsPlayer () &
      (atari != (color_at [nbr_v].ToPlayer () == player));
  });

  vertex_for_each_4_nbr (v, nbr_v, chain_at(nbr_v).lib_cnt += 1);

  return not_suicide;
}


bool Board::IsLegal (Move move) const {
  return IsLegal (move.GetPlayer (), move.GetVertex());
}


bool Board::IsEyelike (Player player, Vertex v) const {
  ASSERT (color_at [v] == Color::Empty ());
  if (! nbr_cnt[v].player_cnt_is_max (player)) return false;

  NatMap<Color, int> diag_color_cnt (0); // TODO

  vertex_for_each_diag_nbr (v, diag_v, {
    diag_color_cnt [color_at [diag_v]]++;
  });

  return
    diag_color_cnt [Color::OfPlayer (player.Other())] +
    (diag_color_cnt [Color::OffBoard ()] > 0) < 2;
}

bool Board::IsEyelike (Move move) const {
  return IsEyelike (move.GetPlayer (), move.GetVertex());
}

Vertex Board::RandomLightMove (Player pl, FastRandom& random) {
  uint ii_start = random.GetNextUint (EmptyVertexCount()); 
  uint ii = ii_start;

  while (true) { // TODO separate iterator
    Vertex v = EmptyVertex (ii);
    if (!IsEyelike (pl, v) && IsLegal (pl, v)) return v;
    ii += 1;
    ii &= ~(-(ii == EmptyVertexCount())); // if (ii==board->empty_v_cnt) ii=0;
    if (ii == ii_start) return Vertex::Pass();
  }
}


void Board::PlayLegal (Move move) { // TODO test with move
  PlayLegal (move.GetPlayer (), move.GetVertex());
}


flatten all_inline
void Board::PlayLegal (Player player, Vertex v) { // TODO test with move
  check ();

  ASSERT (player.IsValid());
  ASSERT (v.IsValid());
  ASSERT (IsLegal (player, v));

  uint last_empty_v_cnt  = empty_v_cnt;
  ko_v                   = Vertex::Invalid();
  last_player            = player;
  last_play [player]     = v;
  move_no                += 1;

  if (v == Vertex::Pass ()) return;

  place_stone (player, v);

  if (nbr_cnt[v].player_cnt_is_max (player.Other())) {
    vertex_for_each_4_nbr (v, nbr_v, chain_at(nbr_v).lib_cnt -= 1);

    play_eye_legal (v);
    // if captured exactly one stone (and this was eye) then Ko
    if (last_empty_v_cnt == empty_v_cnt) ko_v = empty_v [empty_v_cnt - 1];
  } else {
    vertex_for_each_4_nbr (v, nbr_v, update_neighbour(player, v, nbr_v));
  }

  vertex_for_each_4_nbr (v, nbr_v, nbr_cnt [nbr_v].player_inc (player));

  // suicide support
  // if (chain_at(v).lib_cnt == 0) remove_chain (v);
  ASSERT (chain_at(v).lib_cnt != 0);
}


all_inline
void Board::update_neighbour (Player player, Vertex v, Vertex nbr_v) {
  if (!color_at [nbr_v].IsPlayer ()) return;

  chain_at(nbr_v).lib_cnt -= 1;

  if (color_at [nbr_v] != Color::OfPlayer (player)) { // same color of groups
    if (chain_at(nbr_v).lib_cnt == 0)
      remove_chain (nbr_v);
  } else {
    if (chain_id [nbr_v] != chain_id [v]) {
      if (chain_at(v).lib_cnt > chain_at(nbr_v).lib_cnt) {
        merge_chains (v, nbr_v);
      } else {
        merge_chains (nbr_v, v);
      }
    }
  }
}


no_inline
void Board::play_eye_legal (Vertex v) {
  vertex_for_each_4_nbr (v, nbr_v, {
    if ((chain_at(nbr_v).lib_cnt == 0))
      remove_chain (nbr_v);
  });
}


void Board::merge_chains (Vertex v_base, Vertex v_new) {
  chain_at(v_base).lib_cnt += chain_at(v_new).lib_cnt;

  Vertex act_v = v_new;
  do {
    chain_id [act_v] = chain_id [v_base];
    act_v = chain_next_v [act_v];
  } while (act_v != v_new);

  swap (chain_next_v[v_base], chain_next_v[v_new]);
}

no_inline
void Board::remove_chain (Vertex v) {
  Color old_color = color_at[v];
  Vertex act_v = v;

  ASSERT (old_color.IsPlayer ());

  do {
    remove_stone (act_v);
    act_v = chain_next_v[act_v];
  } while (act_v != v);

  ASSERT (act_v == v);

  do {
    vertex_for_each_4_nbr (act_v, nbr_v, {
      nbr_cnt [nbr_v].player_dec (old_color.ToPlayer());
      chain_at(nbr_v).lib_cnt += 1;
    });

    Vertex tmp_v = act_v;
    act_v = chain_next_v[act_v];
    chain_next_v [tmp_v] = tmp_v;

  } while (act_v != v);
}


void Board::place_stone (Player pl, Vertex v) {
  hash ^= zobrist->OfPlayerVertex (pl, v);
  player_v_cnt[pl]++;
  color_at[v] = Color::OfPlayer (pl);

  empty_v_cnt--;
  empty_pos [empty_v [empty_v_cnt]] = empty_pos [v];
  empty_v [empty_pos [v]] = empty_v [empty_v_cnt];

  ASSERT (chain_next_v[v] == v);

  chain_id [v] = v;
  chain_at(v).lib_cnt = nbr_cnt[v].empty_cnt ();
}


void Board::remove_stone (Vertex v) {
  hash ^= zobrist->OfPlayerVertex (color_at [v].ToPlayer (), v);
  player_v_cnt [color_at[v].ToPlayer ()]--;
  color_at [v] = Color::Empty ();

  empty_pos [v] = empty_v_cnt;
  empty_v [empty_v_cnt++] = v;
  chain_id [v] = v;

  ASSERT (empty_v_cnt < Vertex::kBound);
}


// TODO/FIXME last_player should be preserverd in undo function
Player Board::ActPlayer () const {
  return last_player.Other();
}

void Board::SetActPlayer (Player pl) {
  last_player = pl.Other();
}

Player Board::LastPlayer () const {
  return last_player;
}

Vertex Board::LastVertex() const {
  return last_play [LastPlayer()];
}

Move Board::LastMove() const {
  return Move (LastPlayer(), LastVertex());
}

bool Board::BothPlayerPass () const {
  return
    (last_play [Player::Black ()] == Vertex::Pass ()) &
    (last_play [Player::White ()] == Vertex::Pass ());
}

int Board::TrompTaylorScore() const {
  NatMap<Player, int> score (0);

  ForEachNat (Player, pl) {
    FastStack<Vertex, kArea> queue;
    NatMap<Vertex, bool> visited (false);

    ForEachNat (Vertex, v) {
      if (color_at[v] == Color::OfPlayer (pl)) {
        queue.Push(v);
        visited[v] = true;
      }
    }

    while (!queue.IsEmpty()) {
      Vertex v = queue.PopTop();
      ASSERT (visited[v]);
      score[pl] += 1;
      vertex_for_each_4_nbr(v, nbr, {
        if (!visited[nbr] && color_at[nbr] == Color::Empty()) {
          queue.Push(nbr);
          visited[nbr] = true;
        }
      });
    }
  }
  return komi_inverse + score[Player::Black ()] - score[Player::White ()];
}

Player Board::TrompTaylorWinner() const {
  return Player::WinnerOfBoardScore (TrompTaylorScore ());
}

int Board::StoneScore () const {
  return komi_inverse + player_v_cnt[Player::Black ()] -  player_v_cnt[Player::White ()];
}


int Board::PlayoutScore () const {
  int eye_score = 0;

  empty_v_for_each (this, v, {
    eye_score += nbr_cnt[v].player_cnt_is_max (Player::Black ());
    eye_score -= nbr_cnt[v].player_cnt_is_max (Player::White ());
  });

  return StoneScore () + eye_score;
}


Player Board::StoneWinner () const { 
  return Player::WinnerOfBoardScore (StoneScore ()); 
}


Player Board::PlayoutWinner () const {
  return Player::WinnerOfBoardScore (PlayoutScore ());
}


void Board::check_empty_v () const {
  if (!kCheckAsserts) return;

  NatMap<Vertex, bool> noticed (false);
  NatMap<Player, uint> exp_player_v_cnt (0);

  ASSERT (empty_v_cnt <= kArea);

  empty_v_for_each (this, v, {
      ASSERT (noticed [v] == false);
      noticed [v] = true;
    });

  ForEachNat (Vertex, v) {
    ASSERT ((color_at[v] == Color::Empty ()) == noticed[v]);
    if (color_at[v] == Color::Empty ()) {
      ASSERT (empty_pos[v] < empty_v_cnt);
      ASSERT (empty_v [empty_pos[v]] == v);
    }
    if (color_at [v].IsPlayer ()) exp_player_v_cnt [color_at[v].ToPlayer ()]++;
  }

  ForEachNat (Player, pl)
    ASSERT (exp_player_v_cnt [pl] == player_v_cnt [pl]);
}

Board::Chain& Board::chain_at (Vertex v) {
  return chain[chain_id[v]];
}

const Board::Chain& Board::chain_at (Vertex v) const {
  return chain[chain_id[v]];
}

// -----------------------------------------------------------------------------

void Board::check_hash () const {
  ASSERT (hash == recalc_hash ());
}


void Board::check_color_at () const {
  if (!kCheckAsserts) return;

  ForEachNat (Vertex, v) {
    ASSERT ((color_at[v] != Color::OffBoard()) == (v.IsOnBoard ()));
  }
}


void Board::check_nbr_cnt () const {
  if (!kCheckAsserts) return;

  ForEachNat (Vertex, v) {
    NatMap<Color, uint> nbr_color_cnt (0);
    if (color_at[v] == Color::OffBoard()) continue; // TODO is that right?

    vertex_for_each_4_nbr (v, nbr_v, {
        nbr_color_cnt [color_at [nbr_v]]++;
      });

    nbr_cnt[v].check(nbr_color_cnt);
  }
}


void Board::check_chain_at () const {
  if (!kCheckAsserts) return;

  ForEachNat (Vertex, v) {
    // whether same color neighbours have same root and liberties
    // TODO what about off_board and empty?
    if (color_at [v].IsPlayer ()) {

      ASSERT (chain[chain_id[v]].lib_cnt != 0);

      vertex_for_each_4_nbr (v, nbr_v, {
          if (color_at[v] == color_at[nbr_v])
            ASSERT (chain_id [v] == chain_id [nbr_v]);
        });
    }
  }
}


void Board::check_chain_next_v () const {
  if (!kCheckAsserts) return;
  ForEachNat (Vertex, v) {
    // TODO chain_next_v[v].check ();
    if (!color_at [v].IsPlayer ())
      ASSERT (chain_next_v [v] == v);
  }
}


void Board::check () const {
  if (!kCheckAsserts) return;

  check_empty_v       ();
  check_hash          ();
  check_color_at      ();
  check_nbr_cnt       ();
  check_chain_at      ();
  check_chain_next_v  ();
}


void Board::check_no_more_legal (Player player) const { // at the end of the playout
  unused (player);

  if (!kCheckAsserts) return;

  ForEachNat (Vertex, v)
    ASSERT (IsLegal (player, v) == false || IsEyelike (player, v));
}

const Zobrist Board::zobrist[1] = { Zobrist () };

#undef vertex_for_each_4_nbr
#undef vertex_for_each_diag_nbr


void Board::AlignHack (Board& board) {
  int xxx = board.komi_inverse;
  unused(xxx);
}


