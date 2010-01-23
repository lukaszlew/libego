
const float kSureWinUpdate = 1.0; // TODO increase this

// -----------------------------------------------------------------------------

namespace Param {
  bool use_mcts_in_playout = true; // TODO true
}

class MctsPlayout {
  static const bool kCheckAsserts = false;
public:
  MctsPlayout (FastRandom& random_) : random (random_) {
  }

  void DoOnePlayout (MctsNode& playout_root, const Board& board, Player first_player) {
    // Prepare simulation board and tree iterator.
    play_board.Load (board);
    play_board.SetActPlayer (first_player);
    trace.clear();
    trace.push_back (&playout_root);
    move_history.clear ();
    move_history.push_back (playout_root.GetMove());
    play_count.SetToZero();
    mcmc.NewPlayout ();

    tree_phase = Param::use_mcts_in_playout;

    // do the playout
    while (true) {
      if (play_board.BothPlayerPass()) break;
      if (move_history.size() >= 3*Board::kArea) return;
      Player pl = play_board.ActPlayer ();
      Vertex v  = Vertex::Any ();


      if (tree_phase && v == Vertex::Any ())
        v = ChooseTreeMove (pl);

      // if (Param::mcmc_use && v == Vertex::Any ()) + randomization
      //   v = mcmc.Choose8Move (play_board, play_count, random);

      // if (v = Vertex::Any () && random.GetNextUint (1024) < 128)
      if (v == Vertex::Any ())
        v = ChooseLocalMove ();

      if (v == Vertex::Any ())
        v = play_board.RandomLightMove (pl, random);

      Move prev_move = play_board.LastMove ();

      ASSERT (play_board.IsLegal (pl, v));
      play_board.PlayLegal (pl, v);
      move_history.push_back (Move(pl, v));
      play_count [v] += 1;

      Move last_move = play_board.LastMove ();
      mcmc.MovePlayed (prev_move, last_move, play_count);
    }

    float score;
    if (tree_phase) {
      score = play_board.TrompTaylorWinner().ToScore() * kSureWinUpdate;
    } else {
      int sc = play_board.PlayoutScore();
      score = Player::WinnerOfBoardScore (sc).ToScore (); // +- 1
      score += float(sc) / 10000.0; // small bonus for bigger win.
    }
    UpdateTrace (score);
  }

  vector<Move> LastPlayout () {
    return move_history;
  }

private:

  Vertex ChooseTreeMove (Player pl) {
    if (!ActNode().has_all_legal_children [pl]) {
      if (!ActNode().ReadyToExpand ()) {
        tree_phase = false;
        return Vertex::Any ();
      }
      ASSERT (pl == ActNode().player.Other());
      ActNode().EnsureAllLegalChildren (pl, play_board);
    }

    MctsNode& uct_child = ActNode().BestRaveChild (pl);
    trace.push_back (&uct_child);
    ASSERT (uct_child.v != Vertex::Any());
    return uct_child.v;
  }
  
  Vertex ChooseLocalMove () {
    Vertex last_v = play_board.LastVertex ();
    Player pl = play_board.ActPlayer ();

    if (last_v == Vertex::Any () || last_v == Vertex::Pass ())
      return Vertex::Any ();

    FastStack <Vertex, 8> tab;
    for_each_8_nbr (last_v, v, {
      if (play_board.IsLegal(pl, v)) {
        tab.Push (v);
      } 
    });
    
    if (tab.Size() <= 0) return Vertex::Any ();

    uint i = random.GetNextUint (tab.Size());
    return tab[i];
  }

  void UpdateTrace (int score) {
    UpdateTraceRegular (score);
    if (Param::update_rave) UpdateTraceRave (score);
    if (Param::mcmc_update) mcmc.Update (play_board);
  }

  void UpdateTraceRegular (float score) {
    BOOST_FOREACH (MctsNode* node, trace) {
      node->stat.update (score);
    }
  }

  void UpdateTraceRave (float score) {
    // TODO configure rave blocking through options


    uint last_ii  = move_history.size () * 7 / 8; // TODO 

    rep (act_ii, trace.size()) {
      // Mark moves that should be updated in RAVE children of: trace [act_ii]
      NatMap <Move, bool> do_update (false);
      NatMap <Move, bool> do_update_set_to (true);

      // TODO this is the slow and too-fixed part
      // TODO Change it to weighting with flexible masking.
      reps (jj, act_ii+1, last_ii) {
        Move m = move_history [jj];
        do_update [m] = do_update_set_to [m];
        do_update_set_to [m] = false;
        do_update_set_to [m.OtherPlayer()] = false;
      }

      // Do the update.
      BOOST_FOREACH (MctsNode& child, trace[act_ii]->Children()) {
        if (do_update [child.GetMove()]) {
          child.rave_stat.update (score);
        }
      }
    }
  }

  MctsNode& ActNode() {
    ASSERT (trace.size() > 0);
    return *trace.back ();
  }

private:
  friend class MctsGtp;
  
  // playout
  Board play_board;
  FastRandom& random;
  vector <MctsNode*> trace;               // nodes in the path
  vector <Move> move_history;
  NatMap <Vertex, uint> play_count;
  bool tree_phase;
public:
  Mcmc mcmc;
};
