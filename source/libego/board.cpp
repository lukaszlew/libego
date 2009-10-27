/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                           *
 *  This file is part of Library of Effective GO routines - EGO library      *
 *                                                                           *
 *  Copyright 2006 and onwards, Lukasz Lew                                   *
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
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <cmath>
#include <cstring>
#include <iostream>

#include "board.h"
#include "fast_stack.h"
#include "testing.h"

Board::NbrCounter Board::NbrCounter::OfCounts (uint black_cnt,
                                               uint white_cnt,
                                               uint empty_cnt) {
  assertc (nbr_cnt_ac, black_cnt <= max);
  assertc (nbr_cnt_ac, white_cnt <= max);
  assertc (nbr_cnt_ac, empty_cnt <= max);
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
  if (!nbr_cnt_ac) return;
  assert (empty_cnt () <= max);
  assert (player_cnt (Player::black ()) <= max);
  assert (player_cnt (Player::white ()) <= max);
}

void Board::NbrCounter::check(const NatMap<Color, uint>& nbr_color_cnt) const {
  if (!nbr_cnt_ac) return;

  uint expected_nbr_cnt =        // definition of nbr_cnt[v]
    + ((nbr_color_cnt [Color::black ()] + nbr_color_cnt [Color::off_board ()])
       << f_shift[0])
    + ((nbr_color_cnt [Color::white ()] + nbr_color_cnt [Color::off_board ()])
       << f_shift[1])
    + ((nbr_color_cnt [Color::empty ()])
       << f_shift[2]);
  assert (bitfield == expected_nbr_cnt);
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


string Board::to_string (Vertex mark_v) const {
  ostringstream out;

#define os(n)      out << " " << n
#define o_left(n)  out << "(" << n
#define o_right(n) out << ")" << n

  out << " ";
  if (board_size < 10) out << " "; else out << "  ";
  coord_for_each (col) os (col.col_to_string ());
  out << endl;

  coord_for_each (row) {
    if (board_size >= 10 && board_size - row.idx < 10) out << " ";
    os (row.row_to_string ());
    coord_for_each (col) {
      Vertex v = Vertex (row, col);
      char ch = color_at [v].to_char ();
      if      (v == mark_v)        o_left  (ch);
      else if (v == mark_v.E ())   o_right (ch);
      else                         os (ch);
    }
    if (board_size >= 10 && board_size - row.idx < 10) out << " ";
    os (row.row_to_string ());
    out << endl;
  }

  if (board_size < 10) out << "  "; else out << "   ";
  coord_for_each (col) os (col.col_to_string ());
  out << endl;

#undef os
#undef o_left
#undef o_right

  return out.str ();
}


void Board::print_cerr (Vertex v) const {
  cerr << to_string (v);
}


void Board::clear () {
  empty_v_cnt = 0;
  player_for_each (pl) {
    player_v_cnt [pl] = 0;
    last_play_ [pl]   = Vertex::any ();
  }
  move_no      = 0;
  last_player_ = Player::white (); // act player is other
  last_move_status = play_ok;
  ko_v_        = Vertex::any ();
  vertex_for_each_all (v) {
    color_at      [v] = Color::off_board ();
    nbr_cnt       [v] = NbrCounter::Empty();
    chain_next_v  [v] = v;
    chain_id_     [v] = v;      // TODO is it needed, is it used?
    chain_[v].lib_cnt = NbrCounter::max; // TODO off_boards?

    if (v.is_on_board ()) {
      color_at   [v]              = Color::empty ();
      empty_pos  [v]              = empty_v_cnt;
      empty_v    [empty_v_cnt++]  = v;

      vertex_for_each_4_nbr (v, nbr_v, {
        if (!nbr_v.is_on_board ()) {
          nbr_cnt [v].off_board_inc ();
        }
      });
    }
  }

  hash_ = recalc_hash ();

  check ();
}


Hash Board::recalc_hash () const {
  Hash new_hash;

  new_hash.set_zero ();

  vertex_for_each_all (v) {
    if (color_at [v].is_player ()) {
      new_hash ^= zobrist->of_pl_v (color_at [v].to_player (), v);
    }
  }

  return new_hash;
}

Board::Board () {
  clear ();
}

void Board::load (const Board* save_board) {
  memcpy(this, save_board, sizeof(Board));
  check ();
}

void Board::set_komi (float fkomi) {
  komi_inverse_ = int (ceil (-fkomi));
}


float Board::komi () const {
  return -float(komi_inverse_) + 0.5;
}

Vertex Board::ko_v () const {
  return ko_v_;
}

Hash Board::hash () const {
  return hash_;
}

bool Board::is_pseudo_legal (Player player, Vertex v) const {
  check ();
  return
    v == Vertex::pass () ||
    !nbr_cnt[v].player_cnt_is_max (player.other ()) ||
    (!eye_is_ko (player, v) &&
     !eye_is_suicide (v));
}


bool Board::is_eyelike (Player player, Vertex v) const {
  assertc (board_ac, color_at [v] == Color::empty ());
  if (! nbr_cnt[v].player_cnt_is_max (player)) return false;

  NatMap<Color, int> diag_color_cnt; // TODO
  color_for_each (col)
    diag_color_cnt [col] = 0;

  vertex_for_each_diag_nbr (v, diag_v, {
      diag_color_cnt [color_at [diag_v]]++;
    });

  return
    diag_color_cnt [Color (player.other ())] +
    (diag_color_cnt [Color::off_board ()] > 0) < 2;
}

flatten all_inline
void Board::play_legal (Player player, Vertex v) { // TODO test with move
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
    assertc (board_ac, last_move_status == play_ok ||
             last_move_status == play_suicide);
    // TODO invent complete suicide testing
  }
}


bool Board::eye_is_ko (Player player, Vertex v) const {
  return (v == ko_v_) & (player == last_player_.other ());
}


bool Board::eye_is_suicide (Vertex v) const {
  uint all_nbr_live = true;
  vertex_for_each_4_nbr (v, nbr_v, all_nbr_live &= (--chain_at(nbr_v).lib_cnt != 0));
  vertex_for_each_4_nbr (v, nbr_v, chain_at(nbr_v).lib_cnt += 1);
  return all_nbr_live;
}


all_inline
void Board::update_neighbour (Player player, Vertex v, Vertex nbr_v) {
  nbr_cnt [nbr_v].player_inc (player);

  if (!color_at [nbr_v].is_player ()) return;

  chain_at(nbr_v).lib_cnt -= 1;

  if (color_at [nbr_v] != Color (player)) { // same color of groups
    if (chain_at(nbr_v).lib_cnt == 0)
      remove_chain (nbr_v);
  } else {
    if (chain_id_ [nbr_v] != chain_id_ [v]) {
      if (chain_at(v).lib_cnt > chain_at(nbr_v).lib_cnt) {
        merge_chains (v, nbr_v);
      } else {
        merge_chains (nbr_v, v);
      }
    }
  }
}


all_inline
void Board::play_not_eye (Player player, Vertex v) {
  check ();
  v.check_is_on_board ();
  assertc (board_ac, color_at[v] == Color::empty ());

  basic_play (player, v);

  place_stone (player, v);

  vertex_for_each_4_nbr (v, nbr_v, update_neighbour(player, v, nbr_v));

  if (chain_at(v).lib_cnt == 0) {
    assertc (board_ac, last_empty_v_cnt - empty_v_cnt == 1);
    remove_chain(v);
    assertc (board_ac, last_empty_v_cnt - empty_v_cnt > 0);
    last_move_status = play_suicide;
  } else {
    last_move_status = play_ok;
  }
}


no_inline
void Board::play_eye_legal (Player player, Vertex v) {
  vertex_for_each_4_nbr (v, nbr_v, chain_at(nbr_v).lib_cnt -= 1);

  basic_play (player, v);
  place_stone (player, v);

  vertex_for_each_4_nbr (v, nbr_v, {
      nbr_cnt [nbr_v].player_inc (player);
    });

  vertex_for_each_4_nbr (v, nbr_v, {
      if ((chain_at(nbr_v).lib_cnt == 0))
        remove_chain (nbr_v);
    });

  assertc (board_ac, chain_at(v).lib_cnt != 0);

  // if captured exactly one stone (and this was eye)
  if (last_empty_v_cnt == empty_v_cnt) {
    ko_v_ = empty_v [empty_v_cnt - 1]; // then ko is formed
  }
}

// Warning: has to be called before place_stone, because of hash storing
void Board::basic_play (Player player, Vertex v) {
  ko_v_                   = Vertex::any ();
  last_empty_v_cnt        = empty_v_cnt;
  last_player_            = player;
  last_play_ [player]     = v;
  move_no                += 1;
}


void Board::merge_chains (Vertex v_base, Vertex v_new) {
  Vertex act_v;

  chain_at(v_base).lib_cnt += chain_at(v_new).lib_cnt;

  act_v = v_new;
  do {
    chain_id_ [act_v] = chain_id_ [v_base];
    act_v = chain_next_v [act_v];
  } while (act_v != v_new);

  swap (chain_next_v[v_base], chain_next_v[v_new]);
}

no_inline
void Board::remove_chain (Vertex v) {
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
    vertex_for_each_4_nbr (act_v, nbr_v, {
        nbr_cnt [nbr_v].player_dec (old_color.to_player());
        chain_at(nbr_v).lib_cnt += 1;
      });

    tmp_v = act_v;
    act_v = chain_next_v[act_v];
    chain_next_v [tmp_v] = tmp_v;

  } while (act_v != v);
}


void Board::place_stone (Player pl, Vertex v) {
  hash_ ^= zobrist->of_pl_v (pl, v);
  player_v_cnt[pl]++;
  color_at[v] = Color (pl);

  empty_v_cnt--;
  empty_pos [empty_v [empty_v_cnt]] = empty_pos [v];
  empty_v [empty_pos [v]] = empty_v [empty_v_cnt];

  assertc (chain_next_v_ac, chain_next_v[v] == v);

  chain_id_ [v] = v;
  chain_at(v).lib_cnt = nbr_cnt[v].empty_cnt ();
}


void Board::remove_stone (Vertex v) {
  hash_ ^= zobrist->of_pl_v (color_at [v].to_player (), v);
  player_v_cnt [color_at[v].to_player ()]--;
  color_at [v] = Color::empty ();

  empty_pos [v] = empty_v_cnt;
  empty_v [empty_v_cnt++] = v;
  chain_id_ [v] = v;

  assertc (board_ac, empty_v_cnt < Vertex::kBound);
}


// TODO/FIXME last_player should be preserverd in undo function
Player Board::act_player () const {
  return last_player_.other ();
}

void Board::set_act_player (Player pl) {
  last_player_ = pl.other ();
}

Player Board::last_player () const {
  return last_player_;
}

Vertex Board::last_play() const {
  return last_play_[last_player()];
}

Move Board::last_move() const {
  return Move(last_player(), last_play());
}

bool Board::both_player_pass () const {
  return
    (last_play_ [Player::black ()] == Vertex::pass ()) &
    (last_play_ [Player::white ()] == Vertex::pass ());
}

int Board::tt_score() const {
  NatMap<Player, int> score;
  player_for_each(pl) score[pl] = 0;

  player_for_each(pl) {
    FastStack<Vertex, area> queue;
    NatMap<Vertex, bool> visited;

    vertex_for_each_all(v) {
      visited[v] = false;
      if (color_at[v] == Color(pl)) {
        queue.Push(v);
        visited[v] = true;
      }
    }

    while (!queue.IsEmpty()) {
      Vertex v = queue.PopTop();
      assertc (board_ac, visited[v]);
      score[pl] += 1;
      vertex_for_each_4_nbr(v, nbr, {
        if (!visited[nbr] && color_at[nbr] == Color::empty()) {
          queue.Push(nbr);
          visited[nbr] = true;
        }
      });
    }
  }
  return komi_inverse_ + score[Player::black ()] - score[Player::white ()];
}

Player Board::tt_winner() const {
  return Player (tt_score () <= 0);
}

int Board::approx_score () const {
  return komi_inverse_ + player_v_cnt[Player::black ()] -  player_v_cnt[Player::white ()];
}


int Board::playout_score () const {
  int eye_score = 0;

  empty_v_for_each (this, v, {
      eye_score += nbr_cnt[v].player_cnt_is_max (Player::black ());
      eye_score -= nbr_cnt[v].player_cnt_is_max (Player::white ());
    });

  return approx_score () + eye_score;
}


Player Board::approx_winner () const { 
  return Player (approx_score () <= 0); 
}


Player Board::playout_winner () const {
  return Player (playout_score () <= 0);
}


int Board::vertex_score (Vertex v) const {
  //     NatMap<Color, int> Coloro_score;
  //     Coloro_score [Color::black ()] = 1;
  //     Coloro_score [Color::white ()] = -1;
  //     Coloro_score [Color::empty ()] =
  //       (nbr_cnt[v].player_cnt_is_max (Player::black ())) -
  //       (nbr_cnt[v].player_cnt_is_max (Player::white ()));
  //     Coloro_score [Color::off_board ()] = 0;
  //     return Coloro_score [color_at [v]];
  switch (color_at [v].GetRaw ()) {
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


void Board::check_empty_v () const {
  if (!board_empty_v_ac) return;

  NatMap<Vertex, bool> noticed;
  NatMap<Player, uint> exp_player_v_cnt;

  vertex_for_each_all (v) noticed[v] = false;

  assert (empty_v_cnt <= area);

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

Board::Chain& Board::chain_at (Vertex v) {
  return chain_[chain_id_[v]];
}

const Board::Chain& Board::chain_at (Vertex v) const {
  return chain_[chain_id_[v]];
}

uint Board::last_capture_size () const {
  return empty_v_cnt + 1 - last_empty_v_cnt;
}

// -----------------------------------------------------------------------------

void Board::check_hash () const {
  assertc (board_hash_ac, hash_ == recalc_hash ());
}


void Board::check_color_at () const {
  if (!board_color_at_ac) return;

  vertex_for_each_all (v) {
    assert ((color_at[v] != Color::off_board()) == (v.is_on_board ()));
  }
}


void Board::check_nbr_cnt () const {
  if (!board_nbr_cnt_ac) return;

  vertex_for_each_all (v) {
    NatMap<Color, uint> nbr_color_cnt;
    if (color_at[v] == Color::off_board()) continue; // TODO is that right?

    color_for_each (col) {
      nbr_color_cnt [col] = 0;
    }
    vertex_for_each_4_nbr (v, nbr_v, {
        nbr_color_cnt [color_at [nbr_v]]++;
      });

    nbr_cnt[v].check(nbr_color_cnt);
  }
}


void Board::check_chain_at () const {
  if (!chain_at_ac) return;

  vertex_for_each_all (v) {
    // whether same color neighbours have same root and liberties
    // TODO what about off_board and empty?
    if (color_at [v].is_player ()) {

      assert (chain_[chain_id_[v]].lib_cnt != 0);

      vertex_for_each_4_nbr (v, nbr_v, {
          if (color_at[v] == color_at[nbr_v])
            assert (chain_id_ [v] == chain_id_ [nbr_v]);
        });
    }
  }
}


void Board::check_chain_next_v () const {
  if (!chain_next_v_ac) return;
  vertex_for_each_all (v) {
    chain_next_v[v].check ();
    if (!color_at [v].is_player ())
      assert (chain_next_v [v] == v);
  }
}


void Board::check () const {
  if (!board_ac) return;

  check_empty_v       ();
  check_hash          ();
  check_color_at      ();
  check_nbr_cnt       ();
  check_chain_at      ();
  check_chain_next_v  ();
}


void Board::check_no_more_legal (Player player) const { // at the end of the playout
  unused (player);

  if (!board_ac) return;

  vertex_for_each_all (v)
    if (color_at[v] == Color::empty ())
      assert (is_eyelike (player, v) || is_pseudo_legal (player, v) == false);
}

const Zobrist Board::zobrist[1] = { Zobrist () };
