#ifndef _PLAYOUT_H_
#define _PLAYOUT_H_

#include "utils.h"
#include "board.h"
#include "fast_stack.h"

#include <cmath>

// mercy rule


class LightPlayout {
public:

  enum Status { pass_pass, too_long, mercy };

  static const uint default_max_moves = Board::area * 2;

  typedef FastStack <Move, default_max_moves> MoveHistory;

public:

  LightPlayout (Board* board_,
                FastRandom& random_,
                uint max_moves_ = default_max_moves);

  // Look at Run() implementation in playout.cpp
  Status Run ();
  
  // The same as Run() but stores the moves on given stack as long as possible.
  template <uint stack_size>
  Status Run (FastStack<Move, stack_size>& history);

  // Plays one move according to Light policy
  void PlayOneMove ();

private:
  Board* board;
  FastRandom& random;
  uint max_moves;
};

// -----------------------------------------------------------------------------
// internatl implementation

template <uint stack_size> all_inline
LightPlayout::Status LightPlayout::Run (FastStack<Move, stack_size>& history) {
  uint last_move = board->move_no + max_moves;
  while (true) {
    if (board->both_player_pass ())  return pass_pass;
    if (board->move_no >= last_move) return too_long;
    // if (abs(board->approx_score ()) > 25) return mercy;
    PlayOneMove ();
    if (!history.IsFull ()) history.Push (board->last_move());
  }
}

#endif
