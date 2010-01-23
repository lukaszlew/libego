class MctsGtp {
public:
  MctsGtp (Gtp::ReplWithGogui& gtp, MctsEngine& mcts_engine)
  : mcts_engine (mcts_engine)
  {
    RegisterCommands (gtp);
    RegisterParams (gtp);
  }

private:

  void RegisterCommands (Gtp::ReplWithGogui& gtp) {
    gtp.Register ("boardsize",    this, &MctsGtp::Cboardsize);
    gtp.Register ("clear_board",  this, &MctsGtp::Cclear_board);
    gtp.Register ("komi",         this, &MctsGtp::Ckomi);
    gtp.Register ("play",         this, &MctsGtp::Cplay);
    gtp.Register ("undo",         this, &MctsGtp::Cundo);
    gtp.Register ("genmove",      this, &MctsGtp::Cgenmove);
    gtp.Register ("showboard",    this, &MctsGtp::Cshowboard);
    gtp.Register ("time_left",    this, &MctsGtp::CTimeLeft);

    gtp.RegisterGfx ("Mcts.DoPlayouts",      "1", this, &MctsGtp::CDoPlayouts);
    gtp.RegisterGfx ("Mcts.DoPlayouts",     "10", this, &MctsGtp::CDoPlayouts);
    gtp.RegisterGfx ("Mcts.DoPlayouts",    "100", this, &MctsGtp::CDoPlayouts);
    gtp.RegisterGfx ("Mcts.DoPlayouts",   "1000", this, &MctsGtp::CDoPlayouts);
    gtp.RegisterGfx ("Mcts.DoPlayouts",  "10000", this, &MctsGtp::CDoPlayouts);
    gtp.RegisterGfx ("Mcts.DoPlayouts", "100000", this, &MctsGtp::CDoPlayouts);

    gtp.RegisterGfx ("Mcts.ShowTree",    "0 4", this, &MctsGtp::CShowTree);
    gtp.RegisterGfx ("Mcts.ShowTree",   "10 4", this, &MctsGtp::CShowTree);
    gtp.RegisterGfx ("Mcts.ShowTree",  "100 4", this, &MctsGtp::CShowTree);
    gtp.RegisterGfx ("Mcts.ShowTree", "1000 4", this, &MctsGtp::CShowTree);

    gtp.RegisterGfx ("Mcts.ShowLastPlayout",  "4", this, &MctsGtp::CShowLastPlayout);
    gtp.RegisterGfx ("Mcts.ShowLastPlayout",  "8", this, &MctsGtp::CShowLastPlayout);
    gtp.RegisterGfx ("Mcts.ShowLastPlayout", "12", this, &MctsGtp::CShowLastPlayout);
    gtp.RegisterGfx ("Mcts.ShowLastPlayout", "16", this, &MctsGtp::CShowLastPlayout);
    gtp.RegisterGfx ("Mcts.ShowLastPlayout", "20", this, &MctsGtp::CShowLastPlayout);

    gtp.RegisterGfx ("Mcts.ShowMcmc", "black %p white", this, &MctsGtp::CShowMcmc);
    gtp.RegisterGfx ("Mcts.ShowMcmc", "white %p black", this, &MctsGtp::CShowMcmc);

//     gtp.RegisterGfx ("Mcts.ShowMcmc", "black %p black # not used", this, &MctsGtp::CShowMcmc);
//     gtp.RegisterGfx ("Mcts.ShowMcmc", "white %p white # not used", this, &MctsGtp::CShowMcmc);

    gtp.RegisterGfx ("McMc.Ownage", "", this, &MctsGtp::CMcmcOwnage);
    gtp.RegisterGfx ("McMc.Reset", "", this, &MctsGtp::CMcmcReset);

//     gtp.RegisterGfx ("Mcts.ShowMcmcProb", "black %p white", this,
//                      &MctsGtp::CShowMcmcProb);

//     gtp.RegisterGfx ("Mcts.ShowMcmcProb", "white %p black", this,
//                      &MctsGtp::CShowMcmcProb);

//     gtp.RegisterGfx ("Mcts.ShowMcmcProb", "black %p black", this,
//                      &MctsGtp::CShowMcmcProb);
//     gtp.RegisterGfx ("Mcts.ShowMcmcProb", "white %p white", this,
//                      &MctsGtp::CShowMcmcProb);
  }

  void RegisterParams (Gtp::ReplWithGogui& gtp) {
    string cmd_search  = "Mcts.Params.Search";
    string cmd_genmove = "Mcts.Params.Genmove";
    string cmd_logger  = "Mcts.Params.Logger";
    string cmd_mcmc    = "McMc.Params";

    gtp.RegisterParam (cmd_mcmc, "update",          &Param::mcmc_update);
    gtp.RegisterParam (cmd_mcmc, "use",             &Param::mcmc_use);
    gtp.RegisterParam (cmd_mcmc, "update_fraction", &Param::mcmc_update_fraction);
    gtp.RegisterParam (cmd_mcmc, "explore_coeff",   &Param::mcmc_explore_coeff);
    gtp.RegisterParam (cmd_mcmc, "prob_8_nbr",      &Param::mcmc_prob_8_nbr);

    gtp.RegisterParam (cmd_search, "explore_coeff", &Param::uct_explore_coeff);
    gtp.RegisterParam (cmd_search, "bias_stat",     &Param::mcts_bias);
    gtp.RegisterParam (cmd_search, "bias_rave",     &Param::mcts_bias);
    gtp.RegisterParam (cmd_search, "use_rave",      &Param::use_rave);
    gtp.RegisterParam (cmd_search, "update_rave",   &Param::update_rave);
    gtp.RegisterParam (cmd_search, "mature_at",     &Param::mature_update_count);

    gtp.RegisterParam (cmd_genmove, "resign_mean",  &Param::resign_mean);
    gtp.RegisterParam (cmd_genmove, "playouts",     &Param::genmove_playouts);
    gtp.RegisterParam (cmd_genmove, "reset_tree",   &Param::reset_tree_on_genmove);
    gtp.RegisterParam (cmd_genmove, "seed",         &mcts_engine.random.seed);

    gtp.RegisterParam (cmd_logger, "is_active",     &mcts_engine.logger.is_active);
    gtp.RegisterParam (cmd_logger, "log_directory", &mcts_engine.logger.log_directory);

    string d2gtp = "d2gtp-params";
    gtp.RegisterParam (d2gtp, "ucb_coeff", &Param::uct_explore_coeff);
    gtp.RegisterParam (d2gtp, "mcts_bias", &Param::mcts_bias);
    gtp.RegisterParam (d2gtp, "rave_bias", &Param::rave_bias);
    gtp.RegisterParam (d2gtp, "mature_at", &Param::mature_update_count);
    gtp.RegisterParam (d2gtp, "playouts_per_genmove", &Param::genmove_playouts);
  }

  void Cclear_board (Gtp::Io& io) {
    io.CheckEmpty ();
    mcts_engine.ClearBoard ();
  }

  void Cgenmove (Gtp::Io& io) {
    Player player = io.Read<Player> ();
    io.CheckEmpty ();
    Vertex v = mcts_engine.Genmove (player);
    io.out << (v.IsValid() ? v.ToGtpString() : "resign");
  }

  void Cboardsize (Gtp::Io& io) {
    int new_board_size = io.Read<int> ();
    io.CheckEmpty ();
    if (!mcts_engine.SetBoardSize (new_board_size)) {
      io.SetError ("unacceptable size");
      return;
    }
  }

  void Ckomi (Gtp::Io& io) {
    float new_komi = io.Read<float> ();
    io.CheckEmpty();
    mcts_engine.SetKomi (new_komi);
  }

  void Cplay (Gtp::Io& io) {
    Move move = io.Read<Move> ();
    io.CheckEmpty ();

    if (!mcts_engine.Play (move)) {
      io.SetError ("illegal move");
      return;
    }
  }

  void Cundo (Gtp::Io& io) {
    io.CheckEmpty ();
    if (!mcts_engine.Undo ()) {
      io.SetError ("too many undo");
      return;
    }
  }

  void Cshowboard (Gtp::Io& io) {
    io.CheckEmpty ();
    io.out << mcts_engine.BoardAsciiArt ();
  }

  void CDoPlayouts (Gtp::Io& io) {
    uint n = io.Read <uint> (Param::genmove_playouts);
    io.CheckEmpty();
    mcts_engine.DoNPlayouts (n);
  }

  void CShowTree (Gtp::Io& io) {
    uint min_updates  = io.Read <uint> ();
    uint max_children = io.Read <uint> ();
    io.CheckEmpty();
    io.out << endl << mcts_engine.TreeAsciiArt (min_updates, max_children);
  }

  void CShowLastPlayout (Gtp::Io& io) {
    int n = io.Read<int> ();
    io.CheckEmpty ();
    mcts_engine.LastPlayoutGfx(n).Report (io);
  }

  void CShowMcmc (Gtp::Io& io) {
    Move   pre_move = io.Read<Move> ();
    Player player   = io.Read<Player> ();
    io.CheckEmpty ();

    Gtp::GoguiGfx gfx;
    mcts_engine.playout.mcmc.MoveValueGfx (pre_move,
                                           player,
                                           mcts_engine.full_board.GetBoard(),
                                           &gfx);
    gfx.Report (io);
  }

  void CMcmcOwnage (Gtp::Io& io) {
    io.CheckEmpty ();

    Gtp::GoguiGfx gfx;
    //mcts_engine.playout.mcmc.OwnageGfx (&gfx);
    gfx.Report (io);
  }

  void CMcmcReset (Gtp::Io& io) {
    io.CheckEmpty ();
    mcts_engine.playout.mcmc.Reset ();
  }

  void CShowMcmcProb (Gtp::Io& io) {
    Move   pre_move = io.Read<Move> ();
    Player player   = io.Read<Player> ();
    io.CheckEmpty ();

    Gtp::GoguiGfx gfx;
    mcts_engine.playout.mcmc.MoveProbGfx (pre_move,
                                          player,
                                          mcts_engine.full_board.GetBoard(),
                                          &gfx);
    gfx.Report (io);
  }

  void CTimeLeft (Gtp::Io& io) {
    Player player = io.Read<Player> ();
    float time = io.Read<float> ();
    int stones = io.Read<int> ();
    mcts_engine.TimeLeft (player, time, stones);
  }

private:
  MctsEngine& mcts_engine;
};
