#ifndef _FULL_BOARD_H_
#define _FULL_BOARD_H_

#include "board.h"

class FullBoard : public Board {
public:
  void clear(float komi = -0.5);

  // Implemented by calling try_play. Slow.
  bool is_legal (Player pl, Vertex v) const;

  // Returns false if move is illegal - forbids suicide and superko. Slow.
  bool try_play (Player player, Vertex v);

  // Undo move. Slow.
  bool undo ();

  const Board& board() const;

  // auxiliary functions

  void set_komi (float fkomi);
  void set_act_player (Player pl);

  void load (const FullBoard* save_board);

private:
  void play_legal (Player player, Vertex v);
  bool is_hash_repeated ();

  FastStack<Move, max_game_length>  move_history;
};

#endif
