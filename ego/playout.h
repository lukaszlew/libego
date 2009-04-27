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
  SimplePolicy(uint seed);
  void play_move (Board* board);
private:
  FastRandom random;
};

typedef Playout<SimplePolicy> SimplePlayout;

#endif
