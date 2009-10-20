class MctsGtp {
public:
  MctsGtp (Gtp::ReplWithGogui& gtp, MctsEngine& mcts_engine)
  : mcts_engine (mcts_engine),
    mcts (mcts_engine.mcts),
    playout_gfx (gtp, mcts_engine.mcts, "MCTS.ShowLastPlayout")
  {
    show_tree_min_updates = 1000;
    show_tree_max_children = 4;

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

    gtp.RegisterGfx ("MCTS.DoPlayouts",      "1", this, &MctsGtp::CDoPlayouts);
    gtp.RegisterGfx ("MCTS.DoPlayouts",     "10", this, &MctsGtp::CDoPlayouts);
    gtp.RegisterGfx ("MCTS.DoPlayouts",    "100", this, &MctsGtp::CDoPlayouts);
    gtp.RegisterGfx ("MCTS.DoPlayouts",   "1000", this, &MctsGtp::CDoPlayouts);
    gtp.RegisterGfx ("MCTS.DoPlayouts",  "10000", this, &MctsGtp::CDoPlayouts);
    gtp.RegisterGfx ("MCTS.DoPlayouts", "100000", this, &MctsGtp::CDoPlayouts);

    gtp.RegisterGfx ("MCTS.ShowTree",    "0", this, &MctsGtp::CShowTree);
    gtp.RegisterGfx ("MCTS.ShowTree",   "10", this, &MctsGtp::CShowTree);
    gtp.RegisterGfx ("MCTS.ShowTree",  "100", this, &MctsGtp::CShowTree);
    gtp.RegisterGfx ("MCTS.ShowTree", "1000", this, &MctsGtp::CShowTree);
  }

  void RegisterParams (Gtp::ReplWithGogui& gtp) {

    gtp.RegisterParam ("MCTS.params", "uct_explore_coeff",
                       &mcts.playout.best_child_finder.uct_explore_coeff);

    gtp.RegisterParam ("MCTS.params", "bias_stat",
                       &mcts.playout.best_child_finder.bias_stat);

    gtp.RegisterParam ("MCTS.params", "bias_rave",
                       &mcts.playout.best_child_finder.bias_rave);

    gtp.RegisterParam ("MCTS.params", "use_rave",
                       &mcts.playout.best_child_finder.use_rave);


    gtp.RegisterParam ("MCTS.params", "update_rave",
                       &mcts.playout.update_rave);

    gtp.RegisterParam ("MCTS.params", "E(score)_to_resign",
                       &mcts.resign_mean);


    gtp.RegisterParam ("MCTS.params", "playouts_before_genmove",
                       &mcts_engine.playout_count);

    gtp.RegisterParam ("MCTS.params", "Min_updates_to_have_children",
                       &mcts.playout.mature_update_count);

    gtp.RegisterParam ("MCTS.params", "show_tree_min_updates",
                       &show_tree_min_updates);
    gtp.RegisterParam ("MCTS.params", "show_tree_max_children",
                       &show_tree_max_children);
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
    uint n = io.Read <uint> (mcts_engine.playout_count);
    io.CheckEmpty();
    mcts.DoNPlayouts (n);
  }

  void CShowTree (Gtp::Io& io) {
    uint min_updates  = io.Read <uint> (show_tree_min_updates);
    uint max_children = io.Read <uint> (show_tree_max_children);
    io.CheckEmpty();
    io.out << mcts.ToString (min_updates, max_children);
    // TODO show PV
  }

private:
  MctsEngine& mcts_engine;
  Mcts& mcts;

  float show_tree_min_updates;
  float show_tree_max_children;

  PlayoutGfx<Mcts> playout_gfx;
};
