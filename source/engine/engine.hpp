//
// Copyright 2006 and onwards, Lukasz Lew
//

// -----------------------------------------------------------------------------

class Engine {
public:
  
  Engine (Gtp::ReplWithGogui& gtp) :
    random (TimeSeed()),
    playout(random),
    time_control (gtp)
  {
  }

  bool SetBoardSize (uint board_size) {
    return board_size == ::board_size;
  }

  void SetKomi (float komi) {
    float old_komi = full_board.Komi ();
    full_board.SetKomi (komi);
    if (komi != old_komi) {
      logger.LogLine("komi "+ToString(komi));
      logger.LogLine ("");
    }
  }

  void ClearBoard () {
    full_board.Clear ();
    playout.mcts.Reset ();
    playout.mcmc.Reset ();
    logger.NewLog ();
    logger.LogLine ("clear_board");
    logger.LogLine ("komi " + ToString (full_board.Komi()));
    logger.LogLine ("");
  }

  bool Play (Move move) {
    bool ok = full_board.IsReallyLegal (move);
    if (ok) {
      full_board.PlayLegal (move);
      logger.LogLine ("play " + move.ToGtpString());
      logger.LogLine ("");
    }
    return ok;
  }

  bool Undo () {
    bool ok = full_board.Undo ();
    if (ok) {
      logger.LogLine ("undo");
      logger.LogLine ("");
    }
    return ok;
  }

  Vertex Genmove (Player player) {
    if (Param::reset_tree_on_genmove) {
      playout.mcts.Reset ();
    }
    playout.mcmc.Reset ();

    logger.LogLine ("param.other seed " + ToString (random.GetSeed ()));

    int playouts = time_control.PlayoutCount (player);

    DoNPlayouts (playouts, player);

    //cerr << mcts.ToString (show_mcts_min_updates, show_mcts_max_children) << endl;

    Vertex v = BestMove (player);

    if (v != Vertex::Invalid ()) {
      CHECK (full_board.IsReallyLegal (Move (player, v)));
      full_board.PlayLegal (Move (player, v));

      logger.LogLine ("reg_genmove " + player.ToGtpString() +
                      "   #? [" + v.ToGtpString() + "]");
      logger.LogLine ("play " + Move (player, v).ToGtpString() + 
                      " # move " +
                      ToString(full_board.MoveCount()));
      logger.LogLine ("");

    }
    return v;
  }

  string BoardAsciiArt () {
    return full_board.ToAsciiArt();
  }

  string MctsAsciiArt (float min_updates, float max_children) {
    return playout.mcts.FindRoot (full_board).RecToString (min_updates, max_children);
  }

  void DoNPlayouts (uint n) {
    DoNPlayouts (n, full_board.ActPlayer());
  }

  void DoNPlayouts (uint n, Player first_player) {
    MctsNode& act_root = playout.mcts.FindRoot (full_board);
    rep (ii, n) {
      playout.mcts.NewPlayout (act_root);
      playout.DoOnePlayout (full_board, first_player);
    }
  }

  Gtp::GoguiGfx LastPlayoutGfx (uint move_count) {
    vector<Move> last_playout = playout.LastPlayout ();

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

    rep (ii, playout.mcmc.moves.size()) {
      gfx.SetSymbol (playout.mcmc.moves[ii].GetVertex().ToGtpString(),
                     Gtp::GoguiGfx::triangle);
    }


    return gfx;
  }

private:

  Vertex BestMove (Player pl) {
    MctsNode& act_root = playout.mcts.FindRoot (full_board);
    const MctsNode& best_node = act_root.MostExploredChild (pl);

    return
      best_node.SubjectiveMean() < Param::resign_mean ?
      Vertex::Invalid() :
      best_node.v;
  }



private:
  friend class MctsGtp;

  // base board
  Board full_board;

  // logging
  Logger logger;
  
  // playout
  FastRandom random;
  MctsPlayout playout;

  TimeControl time_control;
};
