//
// Copyright 2006 and onwards, Lukasz Lew
//

class Engine {
public:
  
  Engine () :
    random (TimeSeed()),
    playout (full_board, random)
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

  Move Genmove (Player player) {
    logger.LogLine ("param.other seed " + ToString (random.GetSeed ()));
    
    Move m = playout.Genmove (player);

    if (m.IsValid ()) {
      CHECK (full_board.IsReallyLegal (m));
      full_board.PlayLegal (m);

      logger.LogLine ("reg_genmove " + player.ToGtpString() +
                      "   #? [" + m.GetVertex().ToGtpString() + "]");
      logger.LogLine ("play " + m.ToGtpString() + " # move " +
                      ToString(full_board.MoveCount()));
      logger.LogLine ("");
    }

    return m;
  }

  string BoardAsciiArt () {
    return full_board.ToAsciiArt();
  }

  string MctsAsciiArt (float min_updates, float max_children) {
    return playout.mcts.FindRoot (full_board).RecToString (min_updates, max_children);
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




private:
  friend class MctsGtp;

  // base board
  Board full_board;

  // logging
  Logger logger;
  
  // playout
  FastRandom random;
  MctsPlayout playout;

};
