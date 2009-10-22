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
  }

  void RegisterParams (Gtp::ReplWithGogui& gtp) {
    gtp.RegisterParam ("Mcts.Search.Params", "uct_explore_coeff",
                       &mcts_engine.playout.best_child_finder.uct_explore_coeff);
    gtp.RegisterParam ("Mcts.Search.Params", "bias_stat",
                       &mcts_engine.playout.best_child_finder.bias_stat);
    gtp.RegisterParam ("Mcts.Search.Params", "bias_rave",
                       &mcts_engine.playout.best_child_finder.bias_rave);
    gtp.RegisterParam ("Mcts.Search.Params", "use_rave",
                       &mcts_engine.playout.best_child_finder.use_rave);
    gtp.RegisterParam ("Mcts.Search.Params", "update_rave",
                       &mcts_engine.playout.update_rave);
    gtp.RegisterParam ("Mcts.Search.Params", "mature_update_count",
                       &mcts_engine.playout.mature_update_count);

    gtp.RegisterParam ("Mcts.Genmove.Params", "resign_mean",
                       &mcts_engine.resign_mean);
    gtp.RegisterParam ("Mcts.Genmove.Params", "playouts",
                       &mcts_engine.genmove_playouts);
    gtp.RegisterParam ("Mcts.Genmove.Params", "reset_tree",
                       &mcts_engine.reset_tree_on_genmove);
    gtp.RegisterParam ("Mcts.Genmove.Params", "seed",
                       &mcts_engine.random.seed);

    gtp.RegisterParam ("Mcts.Logger.Params", "is_active",
                       &mcts_engine.logger.is_active);
    gtp.RegisterParam ("Mcts.Logger.Params", "log_directory",
                       &mcts_engine.logger.log_directory);
  }

  void Cclear_board (Gtp::Io& io) {
    io.CheckEmpty ();
    mcts_engine.ClearBoard ();
  }

  void Cgenmove (Gtp::Io& io) {
    Player player = io.Read<Player> ();
    io.CheckEmpty ();
    io.out << mcts_engine.Genmove (player).to_string();
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
    Player pl = io.Read<Player> ();
    Vertex v  = io.Read<Vertex> ();
    io.CheckEmpty ();

    if (!mcts_engine.Play(pl, v)) {
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
    uint n = io.Read <uint> (mcts_engine.genmove_playouts);
    io.CheckEmpty();
    mcts_engine.DoNPlayouts (n);
  }

  void CShowTree (Gtp::Io& io) {
    uint min_updates  = io.Read <uint> ();
    uint max_children = io.Read <uint> ();
    io.CheckEmpty();
    io.out << mcts_engine.TreeAsciiArt (min_updates, max_children);
  }

  void CShowLastPlayout (Gtp::Io& io) {
    int n = io.Read<int> ();
    io.CheckEmpty ();
    mcts_engine.LastPlayoutGfx(n).Report (io);
  }

private:
  MctsEngine& mcts_engine;
};
