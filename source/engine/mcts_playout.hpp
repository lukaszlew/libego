//
// Copyright 2006 and onwards, Lukasz Lew
//

class MctsPlayout {
public:

  MctsPlayout (const Board& base_board) :
    base_board (base_board),
    random (TimeSeed())
  {
  }

  void Reset () {
    mcts.Reset ();
    mcmc.Reset ();
  }
 
  Move Genmove () {
    if (Param::reset_tree_on_genmove) mcts.Reset ();
    mcmc.Reset ();

    Player player = base_board.ActPlayer ();

    int playouts = time_control.PlayoutCount (player);

    DoNPlayouts (playouts);

    mcts.Sync (base_board);
    return mcts.BestMove (player);
  }


  void DoNPlayouts (uint n) {
    mcts.Sync (base_board);
    rep (ii, n) {
      mcts.NewPlayout ();
      DoOnePlayout ();
    }
  }

  void DoOnePlayout () {
    // Prepare simulation board and tree iterator.
    play_board.Load (base_board);
    mcmc.NewPlayout ();
    playout_moves.clear();


    // TODO setup nonempty as played once already

    // do the playout
    while (true) {
      if (play_board.BothPlayerPass()) break;
      if (play_board.MoveCount() >= 3*Board::kArea) return;

      Move m = Move::Invalid ();

      if (!m.IsValid()) m = mcts.ChooseMove (play_board);
      if (!m.IsValid()) m = mcmc.ChooseMove (play_board);
      if (!m.IsValid()) m = ChooseLocalMove ();
      if (!m.IsValid()) m = play_board.RandomLightMove (random);

      play_board.PlayLegal (m);

      mcts.NewMove (m);
      
      playout_moves.push_back (m);
      
    }
    
    double score = Score (mcts.tree_phase);


    // update models
    mcts.UpdateTraceRegular (score);


    // TODO remove stupid LastMove2
    mcmc.Update (score,
                 base_board.LastMove2(),
                 base_board.LastMove(),
                 playout_moves);

  }


  vector<Move> LastPlayout () {
    return playout_moves;
  }

private:

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

  static const bool kCheckAsserts = false;
  Mcts mcts;
  Mcmc mcmc;

public:
  friend class MctsGtp;
  FastRandom random;

};
