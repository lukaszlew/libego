//
// Copyright 2006 and onwards, Lukasz Lew
//

#include "engine.hpp"

Engine::Engine () :
  random (TimeSeed()),
  sampler (play_board, gammas)
{
}


bool Engine::SetBoardSize (uint board_size) {
  return board_size == ::board_size;
}


void Engine::SetKomi (float komi) {
  base_board.SetKomi (komi);
}


void Engine::ClearBoard () {
  base_board.Clear ();
  mcts.Reset ();
}


bool Engine::Play (Move move) {
  bool ok = base_board.IsReallyLegal (move);
  if (ok) {
    base_board.PlayLegal (move);
    base_board.Dump();
  }
  return ok;
}


bool Engine::Undo () {
  bool ok = base_board.Undo ();
  return ok;
}


const Board& Engine::GetBoard () const {
  return base_board;
}


Move Engine::Genmove (Player player) {
  base_board.SetActPlayer (player);
  Move m = Genmove ();

  if (m.IsValid ()) {
    CHECK (base_board.IsReallyLegal (m));
    base_board.PlayLegal (m);
    base_board.Dump();
  }

  return m;
}


double Engine::GetStatForVertex (Vertex /*v*/) {
  return (double)(rand()%201-100)/(double)100;
}


std::string Engine::GetStringForVertex (Vertex v) {
  return "Vertex: " + v.ToGtpString();
}


Move Engine::Genmove () {
  if (Param::reset_tree_on_genmove) mcts.Reset ();
  Player player = base_board.ActPlayer ();
  int playouts = time_control.PlayoutCount (player);
  DoNPlayouts (playouts);
  return mcts.BestMove (player);
}


void Engine::DoNPlayouts (uint n) {
  mcts.SyncRoot (base_board, gammas);
  rep (ii, n) {
    DoOnePlayout ();
  }
}


void Engine::DoOnePlayout () {
  PrepareToPlayout();

  // do the playout
  while (true) {
    if (play_board.BothPlayerPass()) break;
    if (play_board.MoveCount() >= 3*Board::kArea) return;

    Move m = ChooseMove ();
    PlayMove (m);
  }

  double score = Score (mcts.tree_phase);
  mcts.trace.UpdateTraceRegular (score);
}


void Engine::PrepareToPlayout () {
  play_board.Load (base_board);
  playout_moves.clear();
  sampler.NewPlayout ();
  mcts.NewPlayout ();
}


Move Engine::ChooseMove () {
  Move m = Move::Invalid ();
  if (!m.IsValid()) m = mcts.ChooseMove (play_board, sampler);
  if (!m.IsValid()) m = Move (play_board.ActPlayer (), sampler.SampleMove (random));
  return m;
}


void Engine::PlayMove (Move m) {
  ASSERT (play_board.IsLegal (m));
  play_board.PlayLegal (m);

  mcts.trace.NewMove (m);
  sampler.MovePlayed ();

  playout_moves.push_back (m);
}


void Engine::Sync () {
  play_board.Load (base_board);
  playout_moves.clear();
  sampler.NewPlayout ();
}


vector<Move> Engine::LastPlayout () {
  return playout_moves;
}


double Engine::Score (bool accurate) {
  // TODO game replay i update wszystkich modeli
  double score;
  if (accurate) {
    score = play_board.TrompTaylorWinner().ToScore();
  } else {
    int sc = play_board.PlayoutScore();
    score = Player::WinnerOfBoardScore (sc).ToScore (); // +- 1
    score += double(sc) / 10000.0; // small bonus for bigger win.
  }
  return score;
}
