
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
  float old_komi = base_board.Komi ();
  base_board.SetKomi (komi);
  if (komi != old_komi) {
    logger.LogLine("komi "+ToString(komi));
    logger.LogLine ("");
  }
}

void Engine::ClearBoard () {
  base_board.Clear ();
  Reset ();
  logger.NewLog ();
  logger.LogLine ("clear_board");
  logger.LogLine ("komi " + ToString (base_board.Komi()));
  logger.LogLine ("");
}

bool Engine::Play (Move move) {
  bool ok = base_board.IsReallyLegal (move);
  if (ok) {
    base_board.PlayLegal (move);
    base_board.Dump();
    logger.LogLine ("play " + move.ToGtpString());
    logger.LogLine ("");
  }
  return ok;
}

bool Engine::Undo () {
  bool ok = base_board.Undo ();
  if (ok) {
    logger.LogLine ("undo");
    logger.LogLine ("");
  }
  return ok;
}

const Board& Engine::GetBoard () const {
  return base_board;
}

Move Engine::Genmove (Player player) {
  logger.LogLine ("param.other seed " + ToString (random.GetSeed ()));
    
  base_board.SetActPlayer (player);
  Move m = Genmove ();

  if (m.IsValid ()) {
    CHECK (base_board.IsReallyLegal (m));
    base_board.PlayLegal (m);
    base_board.Dump();

    logger.LogLine ("reg_genmove " + player.ToGtpString() +
                    "   #? [" + m.GetVertex().ToGtpString() + "]");
    logger.LogLine ("play " + m.ToGtpString() + " # move " +
                    ToString(base_board.MoveCount()));
    logger.LogLine ("");
  }

  return m;
}

string Engine::BoardAsciiArt () {
  return base_board.ToAsciiArt();
}
  

Gtp::GoguiGfx Engine::LastPlayoutGfx (uint move_count) {
  vector<Move> last_playout = LastPlayout ();

  move_count = max(move_count, 0u);
  move_count = min(move_count, uint(last_playout.size()));

  Gtp::GoguiGfx gfx;

  rep(ii, move_count) {
    gfx.AddVariationMove (last_playout[ii].ToGtpString());
  }

  if (move_count > 0) {
    gfx.SetSymbol (last_playout[move_count-1].GetVertex().ToGtpString(),
                   Gtp::GoguiGfx::circle);
  }

  //     rep (ii, mcmc.moves.size()) {
  //       gfx.SetSymbol (mcmc.moves[ii].GetVertex().ToGtpString(),
  //                      Gtp::GoguiGfx::triangle);
  //     }


  return gfx;
}

double Engine::GetStatForVertex (Vertex /*v*/) {
  return (double)(rand()%201-100)/(double)100;
}

std::string Engine::GetStringForVertex (Vertex v) {
  return "Vertex: " + v.ToGtpString();
}


void Engine::Reset () {
  mcts.Reset ();
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

  // update models
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
  if (!m.IsValid()) m = ChooseLocalMove ();
  //if (!m.IsValid()) m = play_board.RandomLightMove (random);
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

// TODO policy randomization
Move Engine::ChooseLocalMove () {
  if (!Param::use_local) return Move::Invalid();
  Vertex last_v = play_board.LastVertex ();
  Player pl = play_board.ActPlayer ();

  if (last_v == Vertex::Any () || last_v == Vertex::Pass ())
    return Move::Invalid();

  FastStack <Vertex, 8> tab;
  for_each_8_nbr (last_v, v, {
    if (play_board.IsLegal(pl, v)) {
      tab.Push (v);
    }
  });

  if (tab.Size() <= 0) return Move::Invalid();

  uint i = random.GetNextUint (tab.Size());
  return Move (pl, tab[i]);
}

