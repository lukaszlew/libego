//
// Copyright 2006 and onwards, Lukasz Lew
//

#include "full_board.hpp"


void FullBoard::Clear() {
  // move_no = 0;
  board.Clear();
  moves.clear();
}


void FullBoard::SetKomi (float fkomi) {
  board.SetKomi(fkomi);
}

bool FullBoard::SetBoardSize (uint size) {
  return size == board_size;
}

void FullBoard::Load (const FullBoard& save_board) {
  board.Load (save_board.GetBoard());
  moves = save_board.moves;
}


void FullBoard::PlayLegal (Move m) {
  moves.push_back (m);
  board.PlayLegal (m.GetPlayer(), m.GetVertex());
  CHECK (m == board.LastMove());
}


bool FullBoard::Undo () {
  vector<Move> replay;

  uint game_length = board.MoveCount ();

  if (game_length == 0)
    return false;

  rep (mn, game_length-1)
    replay.push_back (moves [mn]);

  Clear ();  // TODO maybe last_player should be preserverd as well

  rep (mn, game_length-1)
    PlayLegal (replay [mn]);

  return true;
}


bool FullBoard::IsLegal (Move move) const {
  FullBoard tmp;
  tmp.Load (*this);
  return tmp.Play (move);
}


bool FullBoard::IsHashRepeated () {
  Board tmp_board;
  rep (mn, board.MoveCount()-1) {
    tmp_board.PlayLegal (moves[mn].GetPlayer (),
                         moves[mn].GetVertex ());
    if (board.PositionalHash() == tmp_board.PositionalHash())
      return true;
  }
  return false;
}


bool FullBoard::Play (Move move) {
  if (board.IsLegal (move.GetPlayer(), move.GetVertex()) == false)
    return false;

  PlayLegal (move);

  if (move.GetVertex () != Vertex::Pass () && IsHashRepeated ()) {
    CHECK (Undo ());
    return false;
  }

  return true;
}


const Board& FullBoard::GetBoard() const {
  return board;
}

const vector<Move>& FullBoard::Moves () const {
  return moves;
}
