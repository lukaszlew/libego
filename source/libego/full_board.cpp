//
// Copyright 2006 and onwards, Lukasz Lew
//

#include "full_board.hpp"


void FullBoard::Clear() {
  // move_no = 0;
  board.Clear();
  move_history.clear();
}


void FullBoard::SetKomi (float fkomi) {
  board.SetKomi(fkomi);
}


void FullBoard::Load (const FullBoard& save_board) {
  board.Load (save_board.GetBoard());
  move_history = save_board.move_history;
}


bool FullBoard::PlayPseudoLegal (Move m) {
  move_history.push_back (m);
  bool status = board.PlayPseudoLegal (m.GetPlayer(), m.GetVertex());
  CHECK (m == board.LastMove());
  return status;
}


bool FullBoard::Undo () {
  vector<Move> replay;

  uint game_length = board.MoveCount ();

  if (game_length == 0)
    return false;

  rep (mn, game_length-1)
    replay.push_back (move_history [mn]);

  Clear ();  // TODO maybe last_player should be preserverd as well

  rep (mn, game_length-1)
    PlayPseudoLegal (replay [mn]);

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
    tmp_board.PlayPseudoLegal (move_history[mn].GetPlayer (),
                               move_history[mn].GetVertex ());
    if (board.PositionalHash() == tmp_board.PositionalHash())
      return true;
  }
  return false;
}


bool FullBoard::Play (Move move) {
  if (move.GetVertex() == Vertex::Pass ()) {
    PlayPseudoLegal (move);
    return true;
  }

  if (board.ColorAt (move.GetVertex()) != Color::Empty ())
    return false;

  if (board.IsPseudoLegal (move.GetPlayer(), move.GetVertex()) == false)
    return false;

  if (!PlayPseudoLegal (move)) {
    CHECK (Undo ());
    return false;
  }

  if (IsHashRepeated ()) {
    CHECK (Undo ());
    return false;
  }

  return true;
}


const Board& FullBoard::GetBoard() const {
  return board;
}

const vector<Move>& FullBoard::MoveHistory () const {
  return move_history;
}
