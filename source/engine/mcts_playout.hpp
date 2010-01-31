
const float kSureWinUpdate = 1.0; // TODO increase this

class MctsPlayout {
  static const bool kCheckAsserts = false;
public:
  MctsPlayout (FastRandom& random, M::Model& model) : random (random), model (model) {
  }
 
  void DoOnePlayout (const Board& base_board, Player first_player) {
    // Prepare simulation board and tree iterator.
    play_board.Load (base_board);
    play_board.SetActPlayer (first_player);
    mcmc.NewPlayout ();
    playout_moves.clear();


    if (M::Param::update) {
      // TODO this is too costly if model not in sync
      if (!model.SyncWithBoard ()) return; // TODO return false
    }
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
      
      model.NewMove (m);
    }
    
    double score = Score (mcts.tree_phase);


    // update models
    mcts.UpdateTraceRegular (score);


    // TODO remove stupid LastMove2
    mcmc.Update (score,
                 base_board.LastMove2(),
                 base_board.LastMove(),
                 playout_moves);

    model.Update (score);

  }

  vector<Move> LastPlayout () {
    return playout_moves;
  }

private:

  double Score (bool accurate) {
    // TODO game replay i update wszystkich modeli
    double score;
    if (accurate) {
      score = play_board.TrompTaylorWinner().ToScore() * kSureWinUpdate;
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
  friend class MctsGtp;
  
  // playout
  Board play_board;
  FastRandom& random;
  vector<Move> playout_moves;

public:
  Mcts mcts;
  Mcmc mcmc;

  M::Model& model;
};
