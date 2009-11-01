//
// Copyright 2006 and onwards, Lukasz Lew
//

#include "full_board.hpp"
#include "testing.hpp"


void FullBoard::clear() {
  // move_no = 0;
  Board::clear();
  move_history.clear();
}


void FullBoard::set_komi (float fkomi) {
  Board::set_komi(fkomi);
}


void FullBoard::set_act_player (Player pl) {
  Board::set_act_player(pl);
}


void FullBoard::load (const FullBoard* save_board) {
  memcpy((Board*)this, (Board*)save_board, sizeof(Board));
  move_history = save_board->move_history;
}


void FullBoard::play_legal (Player player, Vertex v) {
  Board::play_legal(player, v);
  Move m = Board::last_move(); // TODO why can't I inline it?
  move_history.push_back(m);
}


bool FullBoard::undo () {
  vector<Move> replay;

  uint   game_length  = move_no;

  if (game_length == 0)
    return false;

  rep (mn, game_length-1)
    replay.push_back (move_history [mn]);

  clear ();  // TODO maybe last_player should be preserverd as well

  rep (mn, game_length-1)
    play_legal (replay [mn].get_player (), replay [mn].get_vertex ());

  return true;
}


bool FullBoard::is_legal (Player pl, Vertex v) const {
  FullBoard tmp;
  tmp.load(this);
  return tmp.try_play (pl, v);
}


bool FullBoard::is_hash_repeated () {
  Board tmp_board;
  rep (mn, move_no-1) {
    tmp_board.play_legal (move_history[mn].get_player (),
                          move_history[mn].get_vertex ());
    if (hash() == tmp_board.hash())
      return true;
  }
  return false;
}


bool FullBoard::try_play (Player player, Vertex v) {
  if (v == Vertex::Pass ()) {
    play_legal (player, v);
    return true;
  }

  // TODO v.check_is_on_board ();

  if (color_at [v] != Color::Empty ())
    return false;

  if (is_pseudo_legal (player,v) == false)
    return false;

  play_legal (player, v);

  if (last_move_status != play_ok) {
    bool ok = undo ();
    assert(ok);
    return false;
  }

  if (is_hash_repeated ()) {
    bool ok = undo ();
    assert(ok);
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
