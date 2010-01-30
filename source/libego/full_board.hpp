//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef FULL_BOARD_H_
#define FULL_BOARD_H_

#include <vector>
#include "board.hpp"

// All functions in this class are ineffictient and should not be used
// for performance tasks. Use Board instead.

class FullBoard : public Board {
public:

  // Clears the board.
  void Clear();

  // Returns legality of move. Implemented by calling Play on a board copy.
  // Includes positional superko detection.
  bool IsReallyLegal (Move move) const;

  // Play the move. Assert it is legal.
  void PlayLegal (Player pl, Vertex v);
  void PlayLegal (Move move);

  // Undo move. Replays the game
  bool Undo ();

  // Loads position (and history) from other board.
  void Load (const FullBoard& save_board);

  // Returns list of played moves.
  const vector<Move>& Moves () const;

private:

  bool IsHashRepeated ();

  static const bool kCheckAsserts = false;

  vector<Move> moves;
};

#endif
