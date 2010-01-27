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

    gtp.RegisterGfx ("DoPlayouts",      "1", this, &MctsGtp::CDoPlayouts);
    gtp.RegisterGfx ("DoPlayouts",     "10", this, &MctsGtp::CDoPlayouts);
    gtp.RegisterGfx ("DoPlayouts",    "100", this, &MctsGtp::CDoPlayouts);
    gtp.RegisterGfx ("DoPlayouts",   "1000", this, &MctsGtp::CDoPlayouts);
    gtp.RegisterGfx ("DoPlayouts",  "10000", this, &MctsGtp::CDoPlayouts);
    gtp.RegisterGfx ("DoPlayouts", "100000", this, &MctsGtp::CDoPlayouts);

    gtp.RegisterGfx ("Mcts.Show",    "0 4", this, &MctsGtp::CShowTree);
    gtp.RegisterGfx ("Mcts.Show",   "10 4", this, &MctsGtp::CShowTree);
    gtp.RegisterGfx ("Mcts.Show",  "100 4", this, &MctsGtp::CShowTree);
    gtp.RegisterGfx ("Mcts.Show", "1000 4", this, &MctsGtp::CShowTree);

    gtp.RegisterGfx ("ShowLastPlayout",  "4", this, &MctsGtp::CShowLastPlayout);
    gtp.RegisterGfx ("ShowLastPlayout",  "8", this, &MctsGtp::CShowLastPlayout);
    gtp.RegisterGfx ("ShowLastPlayout", "12", this, &MctsGtp::CShowLastPlayout);
    gtp.RegisterGfx ("ShowLastPlayout", "16", this, &MctsGtp::CShowLastPlayout);
    gtp.RegisterGfx ("ShowLastPlayout", "20", this, &MctsGtp::CShowLastPlayout);


    gtp.RegisterGfx ("McMc.Ownage", "", this, &MctsGtp::CMcmcOwnage);
    gtp.RegisterGfx ("McMc.Reset", "", this, &MctsGtp::CMcmcReset);
    gtp.RegisterGfx ("McMc.Show", "1", this, &MctsGtp::CShowMcmc);
    gtp.RegisterGfx ("McMc.Show", "2", this, &MctsGtp::CShowMcmc);

  }

  void RegisterParams (Gtp::ReplWithGogui& gtp) {
    string tree  = "param.tree";
    string mcmc  = "param.mcmc";
    string other = "param.other";

    gtp.RegisterParam (other, "genmove_playouts",     &Param::genmove_playouts);
    gtp.RegisterParam (other, "local_use",            &Param::use_local);
    gtp.RegisterParam (other, "seed",                 &mcts_engine.random.seed);
    gtp.RegisterParam (other, "logger_is_active",     &mcts_engine.logger.is_active);
    gtp.RegisterParam (other, "logger_directory",     &mcts_engine.logger.log_directory);

    gtp.RegisterParam (tree, "use",             &Param::tree_use);
    gtp.RegisterParam (tree, "max_moves",       &Param::tree_max_moves);
    gtp.RegisterParam (tree, "explore_coeff",   &Param::tree_explore_coeff);
    gtp.RegisterParam (tree, "rave_update",     &Param::tree_rave_update);
    gtp.RegisterParam (tree, "rave_use",        &Param::tree_rave_use);
    gtp.RegisterParam (tree, "stat_bias",       &Param::tree_stat_bias);
    gtp.RegisterParam (tree, "rave_bias",       &Param::tree_rave_bias);


    gtp.RegisterParam (mcmc, "update",          &Param::mcmc_update);
    gtp.RegisterParam (mcmc, "update_fraction", &Param::mcmc_update_fraction);
    gtp.RegisterParam (mcmc, "use",             &Param::mcmc_use);
    gtp.RegisterParam (mcmc, "max_moves",       &Param::mcmc_max_moves);
    gtp.RegisterParam (mcmc, "stat_bias",       &Param::mcmc_stat_bias);
    gtp.RegisterParam (mcmc, "rave_bias",       &Param::mcmc_rave_bias);
    gtp.RegisterParam (mcmc, "explore_coeff",   &Param::mcmc_explore_coeff);



//     gtp.RegisterParam (cmd2, "resign_mean",        &Param::resign_mean);

//     string d2gtp = "d2gtp-params";
//     gtp.RegisterParam (d2gtp, "ucb_coeff", &Param::uct_explore_coeff);
//     gtp.RegisterParam (d2gtp, "mcts_bias", &Param::mcts_bias);
//     gtp.RegisterParam (d2gtp, "rave_bias", &Param::rave_bias);
//     gtp.RegisterParam (d2gtp, "mature_at", &Param::mature_update_count);
//     gtp.RegisterParam (d2gtp, "playouts_per_genmove", &Param::genmove_playouts);
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
    int level = io.Read<int> ();
    io.CheckEmpty ();
    if (level != 1 && level != 2) {
      io.SetError ("bad level");
      return;
    }

    Gtp::GoguiGfx gfx;
    mcts_engine.playout.mcmc.MoveValueGfx (mcts_engine.full_board.GetBoard(), &gfx, level);
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
//     mcts_engine.playout.mcmc.MoveProbGfx (pre_move,
//                                           player,
//                                           mcts_engine.full_board.GetBoard(),
//                                           &gfx);
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
