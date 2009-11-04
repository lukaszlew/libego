//
// Copyright 2006 and onwards, Lukasz Lew
//

#include "full_board.hpp"


void FullBoard::clear() {
  // move_no = 0;
  Board::Clear();
  move_history.clear();
}


void FullBoard::SetKomi (float fkomi) {
  Board::SetKomi(fkomi);
}


void FullBoard::SetActPlayer (Player pl) {
  Board::SetActPlayer(pl);
}


void FullBoard::Load (const FullBoard* save_board) {
  memcpy((Board*)this, (Board*)save_board, sizeof(Board));
  move_history = save_board->move_history;
}


bool FullBoard::PlayPseudoLegal (Player player, Vertex v) {
  bool status = Board::PlayPseudoLegal(player, v);
  move_history.push_back (Board::LastMove());
  return status;
}


bool FullBoard::undo () {
  vector<Move> replay;

  uint game_length = MoveCount ();

  if (game_length == 0)
    return false;

  rep (mn, game_length-1)
    replay.push_back (move_history [mn]);

  clear ();  // TODO maybe last_player should be preserverd as well

  rep (mn, game_length-1)
    PlayPseudoLegal (replay [mn].get_player (), replay [mn].get_vertex ());

  return true;
}


bool FullBoard::is_legal (Player pl, Vertex v) const {
  FullBoard tmp;
  tmp.Load(this);
  return tmp.try_play (pl, v);
}


bool FullBoard::is_hash_repeated () {
  Board tmp_board;
  rep (mn, MoveCount()-1) {
    tmp_board.PlayPseudoLegal (move_history[mn].get_player (),
                               move_history[mn].get_vertex ());
    if (PositionalHash() == tmp_board.PositionalHash())
      return true;
  }
  return false;
}


bool FullBoard::try_play (Player player, Vertex v) {
  if (v == Vertex::Pass ()) {
    PlayPseudoLegal (player, v);
    return true;
  }

  // TODO v.check_is_on_board ();

  if (ColorAt (v) != Color::Empty ())
    return false;

  if (IsPseudoLegal (player,v) == false)
    return false;

  if (!PlayPseudoLegal (player, v)) {
    CHECK (undo ());
    return false;
  }

  if (is_hash_repeated ()) {
    CHECK (undo ());
    return false;
  }

  return true;
}


const Board& FullBoard::board() const {
  return *this;
}

const vector<Move>& FullBoard::MoveHistory () const {
  return move_history;
}
