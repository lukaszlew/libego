#ifndef _PLAYOUT_H_
#define _PLAYOUT_H_

#include "utils.h"
#include "board.h"
#include "fast_stack.h"

#include <cmath>

// mercy rule

const bool use_mercy_rule      = false;
const uint mercy_threshold     = 25;
const uint max_playout_length  = board_area * 2;

enum PlayoutStatus { pass_pass, mercy, too_long};

template <typename Policy> class Playout {
public:
  Policy*  policy;
  Board*   board;
  FastStack<Move, max_game_length>  move_history;
  PlayoutStatus status;

  Playout (Policy* policy_, Board*  board_) : policy (policy_), board (board_) {}

  all_inline flatten
  bool playout_end() {
    if (board->both_player_pass ()) {
      status = pass_pass;
      return true;
    }

    if (board->move_no >= max_playout_length) {
      status = too_long;
      return true;
    }

    if (use_mercy_rule &&
        uint (abs (float(board->approx_score ()))) > mercy_threshold) {
      status = mercy;
      return true;
    }
    return false;
  }

  all_inline
  PlayoutStatus run () {
    move_history.Clear();

    while (!playout_end()) {
      policy->play_move (board);
      Move m = board->last_move();
      move_history.Push(m);
    }

    return status;
  }
  
};

// -----------------------------------------------------------------------------

class SimplePolicy {
public:
  SimplePolicy(FastRandom& random_) : random(random_) { 
  }

  all_inline
  void play_move (Board* board) {
    uint ii_start = random.rand_int (board->empty_v_cnt); 
    uint ii = ii_start;
    Player act_player = board->act_player ();

    Vertex v;
    while (true) {
      v = board->empty_v [ii];
      if (!board->is_eyelike (act_player, v) &&
          board->is_pseudo_legal (act_player, v)) { 
        board->play_legal(act_player, v);
        return;
      }
      ii += 1;
      ii &= ~(-(ii == board->empty_v_cnt)); // if (ii==board->empty_v_cnt) ii=0;
      if (ii == ii_start) {
        board->play_legal(act_player, Vertex::pass());
        return;
      }
    }
  }

protected:
  FastRandom& random;
};

typedef Playout<SimplePolicy> SimplePlayout;

// -----------------------------------------------------------------------------

// TODO simplify and test performance
class LocalPolicy : protected SimplePolicy {
public:

  LocalPolicy(FastRandom& random_) : SimplePolicy(random_) { 
  }

  all_inline
  void play_move (Board* board) {
    if (play_local(board)) return;
    SimplePolicy::play_move(board);
  }

private:
  all_inline
  bool play_local(Board *board) {
    // P(local) = 1/3
    if (random.rand_int(3)) return false;

    Vertex center = board->last_play();
    if ((center == Vertex::any()) |
        (center == Vertex::pass())) return false;

    Vertex local[8];
    Player act_player = board->act_player ();
    uint ii = 0;

    // TODO this introduces serious bias due to non-empty moves
    vertex_for_each_8_nbr(center, nbr, local[ii++] = nbr);

    uint i_start = random.rand_int(8);
    ii = i_start;

    do {
      Vertex v = local[ii];
      if (board->color_at[v] == Color::empty() &&
          !board->is_eyelike(act_player, v) &&
          board->is_pseudo_legal(act_player, v)) {
        board->play_legal(act_player, v);
        return true;
      }
      ii = (ii + 1) & 7;
    } while (ii != i_start);

    // No possibility of a local move
    return false;
  }

};

#endif
