#ifndef _PLAYOUT_H_
#define _PLAYOUT_H_

#include "utils.h"
#include "board.h"

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
  Move*    move_history;
  uint     move_history_length;
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
    uint begin_move_no = board->move_no;
    move_history = board->move_history + board->move_no;

    while (!playout_end()) {
      policy->play_move (board);
    }

    move_history_length = board->move_no - begin_move_no;
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

private:
  FastRandom& random;
};

typedef Playout<SimplePolicy> SimplePlayout;

#endif
