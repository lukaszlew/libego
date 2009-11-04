//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef FULL_BOARD_H_
#define FULL_BOARD_H_

#include <vector>
#include "board.hpp"

class FullBoard : private Board {
public:
  void Clear();

  // Implemented by calling try_play. Slow.
  bool IsLegal (Player pl, Vertex v) const;

  // Returns false if move is illegal - forbids suicide and superko. Slow.
  bool Play (Player player, Vertex v);

  // Undo move. Slow.
  bool Undo ();

  const Board& GetBoard() const;

  // auxiliary functions

  void SetKomi (float fkomi);

  void Load (const FullBoard* save_board);

  const vector<Move>& MoveHistory () const; // TODO rename all to CamelCase

private:
  bool PlayPseudoLegal (Player player, Vertex v);
  bool IsHashRepeated ();

  vector<Move> move_history;
};

#endif
