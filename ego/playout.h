#ifndef _PLAYOUT_H_
#define _PLAYOUT_H_

#include "utils.h"
#include "board.h"
#include "fast_stack.h"

#include <cmath>

// mercy rule


class Playout {
public:
  enum Status { pass_pass, mercy, too_long };
  static const uint default_max_length = board_area * 2;
  typedef FastStack <Move, default_max_length> MoveHistory;

public:
  Playout (Board* board_,
           FastRandom& random_ = global_random,
           uint max_length_ = default_max_length)
    : board (board_), random (random_), max_length (max_length_)
  {
  }

  template <uint stack_size>
  all_inline
  Status DoLightPlayout (FastStack<Move, stack_size>& history) {
    while (true) {
      if (board->both_player_pass ())       return pass_pass;
      if (board->move_no >= max_length)     return too_long;
      //if (abs(board->approx_score ()) > 25) return mercy;
      PlayLightMove ();
      if (!history.IsFull ())
        history.Push (board->last_move());
    }
  }

  all_inline
  Status DoLightPlayout () {
    while (true) {
      if (board->both_player_pass ())       return pass_pass;
      if (board->move_no >= max_length)     return too_long;
      //if (abs(board->approx_score ()) > 25) return mercy;
      PlayLightMove ();
    }
  }

  all_inline
  void PlayLightMove () {
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

  bool PlayLocalMove () {
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

private:
  Board* board;
  FastRandom& random;
  uint max_length;
};

// -----------------------------------------------------------------------------

#endif
