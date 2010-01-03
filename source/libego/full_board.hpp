//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef FULL_BOARD_H_
#define FULL_BOARD_H_

#include <vector>
#include "board.hpp"

// All functions in this class are ineffictient and should not be used
// for performance tasks. Use Board instead.

class FullBoard {
public:
  // Clears the board.
  void Clear();

  // Returns legality of move. Implemented by calling Play on a FullBoard copy.
  bool IsLegal (Move move) const;

  // Play if the move is legal and return true.
  // Otherwise do not play and return false. 
  // (positional superko detection).
  bool Play (Move move);

  // Undo move. Replays the game
  bool Undo ();

  // Get underlying fast board implementation.
  const Board& GetBoard() const;

  // Sets komi value.
  void SetKomi (float fkomi);

  // Sets komi value.
  bool SetBoardSize (uint size);

  // Loads position (and history) from other board.
  void Load (const FullBoard& save_board);

  // Returns list of played moves.
  const vector<Move>& MoveHistory () const; // TODO rename all to CamelCase

private:
  void PlayLegal (Move move);
  bool IsHashRepeated ();

private:
  Board board;
  vector<Move> move_history;
};

#endif
