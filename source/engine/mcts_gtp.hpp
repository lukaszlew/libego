extern Gtp::ReplWithGogui gtp;

class MctsGtp {
public:
  MctsGtp (Engine& mcts_engine)
  : mcts_engine (mcts_engine)
  {
    RegisterCommands ();
    RegisterParams ();
  }

private:

  void RegisterCommands () {
    gtp.Register ("boardsize",    this, &MctsGtp::Cboardsize);
    gtp.Register ("clear_board",  this, &MctsGtp::Cclear_board);
    gtp.Register ("komi",         this, &MctsGtp::Ckomi);
    gtp.Register ("play",         this, &MctsGtp::Cplay);
    gtp.Register ("undo",         this, &MctsGtp::Cundo);
    gtp.Register ("genmove",      this, &MctsGtp::Cgenmove);
    gtp.Register ("showboard",    this, &MctsGtp::Cshowboard);

    gtp.Register ("LoadGammas",   this, &MctsGtp::CLoadGammas);

    gtp.RegisterGfx ("DoPlayouts",      "1", this, &MctsGtp::CDoPlayouts);
    gtp.RegisterGfx ("DoPlayouts",     "10", this, &MctsGtp::CDoPlayouts);
    gtp.RegisterGfx ("DoPlayouts",    "100", this, &MctsGtp::CDoPlayouts);
    gtp.RegisterGfx ("DoPlayouts",   "1000", this, &MctsGtp::CDoPlayouts);
    gtp.RegisterGfx ("DoPlayouts",  "10000", this, &MctsGtp::CDoPlayouts);
    gtp.RegisterGfx ("DoPlayouts", "100000", this, &MctsGtp::CDoPlayouts);

    gtp.RegisterGfx ("ShowLastPlayout",  "4", this, &MctsGtp::CShowLastPlayout);
    gtp.RegisterGfx ("ShowLastPlayout",  "8", this, &MctsGtp::CShowLastPlayout);
    gtp.RegisterGfx ("ShowLastPlayout", "12", this, &MctsGtp::CShowLastPlayout);
    gtp.RegisterGfx ("ShowLastPlayout", "16", this, &MctsGtp::CShowLastPlayout);
    gtp.RegisterGfx ("ShowLastPlayout", "20", this, &MctsGtp::CShowLastPlayout);

    gtp.RegisterGfx ("ShowGammas", "", this, &MctsGtp::CShowGammas);

  }

  void RegisterParams () {
    string tree  = "param.tree";
    string other = "param.other";

    gtp.RegisterParam (other, "genmove_playouts",     &Param::genmove_playouts);
    gtp.RegisterParam (other, "local_use",            &Param::use_local);
    gtp.RegisterParam (other, "seed",                 &mcts_engine.playout.random.seed);
    gtp.RegisterParam (other, "logger_is_active",     &mcts_engine.logger.is_active);
    gtp.RegisterParam (other, "logger_directory",     &mcts_engine.logger.log_directory);

    gtp.RegisterParam (tree, "use",             &Param::tree_use);
    gtp.RegisterParam (tree, "max_moves",       &Param::tree_max_moves);
    gtp.RegisterParam (tree, "explore_coeff",   &Param::tree_explore_coeff);
    gtp.RegisterParam (tree, "rave_update",     &Param::tree_rave_update);
    gtp.RegisterParam (tree, "rave_use",        &Param::tree_rave_use);
    gtp.RegisterParam (tree, "stat_bias",       &Param::tree_stat_bias);
    gtp.RegisterParam (tree, "rave_bias",       &Param::tree_rave_bias);
    gtp.RegisterParam (tree, "rave_update_fraction", &Param::tree_rave_update_fraction);
    gtp.RegisterParam (tree, "progressive_bias",&Param::tree_progressive_bias);
  }

  void Cclear_board (Gtp::Io& io) {
    io.CheckEmpty ();
    mcts_engine.ClearBoard ();
  }

  void Cgenmove (Gtp::Io& io) {
    Player player = io.Read<Player> ();
    io.CheckEmpty ();
    Move m = mcts_engine.Genmove (player);
    io.out << (m.IsValid() ? m.GetVertex().ToGtpString() : "resign");
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
    mcts_engine.playout.DoNPlayouts (n);
  }

  void CShowLastPlayout (Gtp::Io& io) {
    int n = io.Read<int> ();
    io.CheckEmpty ();
    mcts_engine.LastPlayoutGfx(n).Report (io);
  }

  void CShowGammas (Gtp::Io& io) {
    io.CheckEmpty ();
    Gtp::GoguiGfx gfx;
    mcts_engine.ShowGammas (gfx);
    gfx.Report (io);
  }

  void CLoadGammas (Gtp::Io& io) {
    string file_name = io.Read<string> ();
    io.CheckEmpty ();
    ifstream in;
    in.open (file_name.c_str(), ifstream::in);
    if (!in.good()) {
      io.SetError ("Can't open a file: " + file_name);
      return;
    }
    if (!mcts_engine.playout.gammas.Read (in)) {
      io.SetError ("File in a bad format.");
      return;
    }
    in.close();
  }

private:
  Engine& mcts_engine;
};
