//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef FULL_BOARD_H_
#define FULL_BOARD_H_

#include <vector>
#include "board.hpp"

class FullBoard : public Board {
public:
  void clear();

  // Implemented by calling try_play. Slow.
  bool is_legal (Player pl, Vertex v) const;

  // Returns false if move is illegal - forbids suicide and superko. Slow.
  bool try_play (Player player, Vertex v);

  // Undo move. Slow.
  bool undo ();

  const Board& board() const;

  // auxiliary functions

  void set_komi (float fkomi);
  void SetActPlayer (Player pl);

  void Load (const FullBoard* save_board);

  const vector<Move>& MoveHistory () const; // TODO rename all to CamelCase

private:
  void PlayLegal (Player player, Vertex v);
  bool is_hash_repeated ();

  vector<Move> move_history;
};

#endif
