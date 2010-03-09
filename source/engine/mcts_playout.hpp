//
// Copyright 2006 and onwards, Lukasz Lew
//

class MctsPlayout {
public:

  MctsPlayout (const Board& base_board) :
    base_board (base_board),
    random (TimeSeed()),
    sampler (play_board, gammas)
  {
  }

  void Reset () {
    mcts.Reset ();
  }
 
  Move Genmove () {
    if (Param::reset_tree_on_genmove) mcts.Reset ();

    Player player = base_board.ActPlayer ();

    int playouts = time_control.PlayoutCount (player);

    DoNPlayouts (playouts);

    return mcts.BestMove (player);
  }


  void DoNPlayouts (uint n) {
    mcts.SyncRoot (base_board, gammas);
    rep (ii, n) {
      DoOnePlayout ();
    }
  }

  vector<Move> LastPlayout () {
    return playout_moves;
  }

  void PrepareToPlayout () {
    play_board.Load (base_board);
    playout_moves.clear();
    sampler.NewPlayout ();
    mcts.NewPlayout ();
  }


private:
  void DoOnePlayout () {
    PrepareToPlayout ();

    // do the playout
    while (true) {
      if (play_board.BothPlayerPass()) break;
      if (play_board.MoveCount() >= 3*Board::kArea) return;

      Move m = ChooseMove ();
      PlayMove (m);
    }
    
    double score = Score (mcts.tree_phase);

    // update models
    mcts.UpdateTraceRegular (score);
  }


  Move ChooseMove () {
    Move m = Move::Invalid ();

    if (!m.IsValid()) m = mcts.ChooseMove (play_board, sampler);
    if (!m.IsValid()) m = ChooseLocalMove ();
    //if (!m.IsValid()) m = play_board.RandomLightMove (random);
    if (!m.IsValid()) m = Move (play_board.ActPlayer (), sampler.SampleMove (random));
    return m;
  }


  void PlayMove (Move m) {
    ASSERT (play_board.IsLegal (m));
    play_board.PlayLegal (m);

    mcts.NewMove (m);
    sampler.MovePlayed ();
      
    playout_moves.push_back (m);
  }


  double Score (bool accurate) {
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
  Move ChooseLocalMove () {
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

private:
  
  const Board& base_board;
  TimeControl time_control;

  // playout
  Board play_board;
  vector<Move> playout_moves;

  Mcts mcts;

public:
  friend class MctsGtp;
  FastRandom random;
  Gammas gammas;
  Sampler sampler;
};
