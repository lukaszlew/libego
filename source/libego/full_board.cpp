//
// Copyright 2006 and onwards, Lukasz Lew
//

#include "full_board.hpp"


void FullBoard::Clear() {
  Board::Clear();
  moves.clear();
}



bool FullBoard::SetBoardSize (uint size) {
  return size == board_size;
}

void FullBoard::Load (const FullBoard& save_board) {
  Board::Load (save_board.GetBoard());
  moves = save_board.moves;
}


void FullBoard::PlayLegal (Move m) {
  ASSERT (IsLegal (m));
  moves.push_back (m);
  Board::PlayLegal (m);
}


bool FullBoard::Undo () {
  vector<Move> replay;

  uint game_length = Board::MoveCount ();

  if (game_length == 0)
    return false;

  rep (mn, game_length-1)
    replay.push_back (moves [mn]);

  Clear ();  // TODO maybe last_player should be preserverd as well

  rep (mn, game_length-1)
    PlayLegal (replay [mn]);

  return true;
}


bool FullBoard::IsReallyLegal (Move move) const {
  FullBoard tmp;
  tmp.Load (*this);
  return tmp.TryPlay (move);
}


bool FullBoard::IsHashRepeated () {
  Board tmp_board;
  rep (mn, Board::MoveCount()-1) {
    tmp_board.PlayLegal (moves[mn].GetPlayer (),
                         moves[mn].GetVertex ());
    if (Board::PositionalHash() == tmp_board.PositionalHash())
      return true;
  }
  return false;
}


bool FullBoard::TryPlay (Move move) {
  if (Board::IsLegal (move.GetPlayer(), move.GetVertex()) == false)
    return false;

  PlayLegal (move);

  if (move.GetVertex () != Vertex::Pass () && IsHashRepeated ()) {
    CHECK (Undo ());
    return false;
  }

  return true;
}


const Board& FullBoard::GetBoard() const {
  return *this;
}


const vector<Move>& FullBoard::Moves () const {
  return moves;
}
