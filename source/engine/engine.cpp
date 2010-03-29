//
// Copyright 2006 and onwards, Lukasz Lew
//

#include <boost/foreach.hpp>

#include "engine.hpp"

Engine::Engine () :
  random (TimeSeed()),
  mcts(),
  sampler (play_board, gammas),
  act_root (&mcts.root)
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
  Move m = ChooseBestMove ();

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


Move Engine::ChooseBestMove () {
  if (Param::reset_tree_on_genmove) mcts.Reset ();
  Player player = base_board.ActPlayer ();
  int playouts = time_control.PlayoutCount (player);
  DoNPlayouts (playouts);

  const MctsNode& best_node = act_root->MostExploredChild (player);

  return
    best_node.SubjectiveMean() < Param::resign_mean ?
    Move::Invalid() :
    Move (player, best_node.v);
}


void Engine::DoNPlayouts (uint n) {
  SyncRoot ();
  rep (ii, n) {
    DoOnePlayout ();
  }
}


void Engine::SyncRoot () {
  // TODO replace this by FatBoard
  Board sync_board;
  Sampler sampler(sync_board, gammas);
  sampler.NewPlayout ();

  act_root = &mcts.root;
  BOOST_FOREACH (Move m, base_board.Moves ()) {
    mcts.EnsureAllLegalChildren (act_root, m.GetPlayer(), sync_board, sampler);
    act_root = act_root->FindChild (m);
    CHECK (sync_board.IsLegal (m));
    sync_board.PlayLegal (m);
    sampler.MovePlayed();
  }

  Player pl = base_board.ActPlayer();
  mcts.EnsureAllLegalChildren (act_root, pl, base_board, sampler);
  mcts.RemoveIllegalChildren (act_root, pl, base_board);
}


void Engine::DoOnePlayout () {
  PrepareToPlayout();

  // do the playout
  while (true) {
    if (play_board.BothPlayerPass()) break;
    if (play_board.MoveCount() >= 3*Board::kArea) return;

    Move m = Move::Invalid ();
    if (!m.IsValid()) m = mcts.ChooseMove (play_board, sampler);
    if (!m.IsValid()) m = Move (play_board.ActPlayer (), sampler.SampleMove (random));
    PlayMove (m);
  }

  double score = Score (mcts.tree_phase);
  mcts.trace.UpdateTraceRegular (score);
}


void Engine::PrepareToPlayout () {
  play_board.Load (base_board);
  playout_moves.clear();
  sampler.NewPlayout ();

  mcts.trace.Reset (*act_root);
  mcts.act_node = act_root;
  mcts.tree_phase = Param::tree_use;
  mcts.tree_move_count = 0;
}


void Engine::PlayMove (Move m) {
  ASSERT (play_board.IsLegal (m));
  play_board.PlayLegal (m);

  mcts.trace.NewMove (m);
  sampler.MovePlayed ();

  playout_moves.push_back (m);
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
