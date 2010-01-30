//
// Copyright 2006 and onwards, Lukasz Lew
//

#include "full_board.hpp"


void FullBoard::Clear () {
  Board::Clear();
  moves.clear();
}

void FullBoard::Load (const FullBoard& save_board) {
  Board::Load (save_board);
  moves = save_board.moves;
}


flatten
void FullBoard::PlayLegal (Player pl, Vertex v) {
  ASSERT (IsLegal (pl, v));
  moves.push_back (Move (pl, v));
  Board::PlayLegal (pl, v);
}


void FullBoard::PlayLegal (Move m) {
  ASSERT (IsLegal (m));
  moves.push_back (m);
  Board::PlayLegal (m);
}


bool FullBoard::Undo () {
  ASSERT (MoveCount() == moves.size());
  if (MoveCount () == 0) return false;

  vector<Move> replay = moves;
  Clear ();

  rep (ii, replay.size() - 1)
    PlayLegal (replay [ii]);

  return true;
}


bool FullBoard::IsReallyLegal (Move move) const {
  if (IsLegal (move) == false) return false;

  // Pass would repeat the hash.
  if (move.GetVertex () == Vertex::Pass ()) return true;

  // Check for superko.
  FullBoard tmp;
  tmp.Load (*this);
  tmp.PlayLegal (move);
  return !tmp.IsHashRepeated ();
}


bool FullBoard::IsHashRepeated () {
  Board tmp_board;
  rep (mn, MoveCount()-1) {
    tmp_board.PlayLegal (moves[mn]);
    if (PositionalHash() == tmp_board.PositionalHash())
      return true;
  }
  return false;
}


const vector<Move>& FullBoard::Moves () const {
  return moves;
}
