#ifndef _PLAYOUT_H_
#define _PLAYOUT_H_

#include "utils.h"
#include "board.h"

#include <cmath>

// mercy rule

const bool use_mercy_rule      = false;
const uint mercy_threshold     = 25;
const uint max_playout_length  = board_area * 2;

enum playout_status_t { pass_pass, mercy, too_long };

template <typename Policy> class Playout {
public:
  Policy*  policy;
  Board*   board;
  Move*    move_history;
  uint     move_history_length;

  Playout (Policy* policy_, Board*  board_) : policy (policy_), board (board_) {}

  all_inline
  playout_status_t run () {
    uint begin_move_no = board->move_no;
    move_history = board->move_history + board->move_no;
    
    while (true) {
      if (board->both_player_pass ()) {
        move_history_length = board->move_no - begin_move_no;
        return pass_pass;
      }
      
      if (board->move_no >= max_playout_length) {
        move_history_length = board->move_no - begin_move_no;
        return too_long;
      }
      
      if (use_mercy_rule &&
          uint (abs (float(board->approx_score ()))) > mercy_threshold) {
        move_history_length = board->move_no - begin_move_no;
        return mercy;
      }

      policy->play_move (board);
    }
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
    // No local move when game begins.
    if (board->move_no == 0) return false;
    // P(local) = 1/3
    if (random.rand_int(3)) return false;

    Vertex center = board->move_history[board->move_no-1].get_vertex();

    Vertex legal[8];
    legal[0] = center.NW();
    legal[1] = center.N();
    legal[2] = center.NE();
    legal[3] = center.W();
    legal[4] = center.E();
    legal[5] = center.SW();
    legal[6] = center.S();
    legal[7] = center.SE();

    Player act_player = board->act_player ();

    uint i_start = random.rand_int(8), i = i_start;
    do {
      if (legal[i].is_on_board() &&
          board->color_at[legal[i]] == Color::empty() &&
          !board->is_eyelike(act_player, legal[i]) &&
          board->is_pseudo_legal(act_player, legal[i])) {
        board->play_legal(act_player, legal[i]);
        return true;
      }
      i = (i + 1) & 7;
    } while (i != i_start);

    // No possibility of a local move
    return false;
  }

};

#endif
