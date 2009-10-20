class MctsGtp {
public:
  MctsGtp (Gtp::ReplWithGogui& gtp, MctsEngine& mcts_engine)
  : mcts_engine (mcts_engine),
    mcts (mcts_engine.mcts),
    full_board(mcts_engine.full_board),
    playout_gfx (gtp, mcts_engine.mcts, "MCTS.ShowLastPlayout")
  {
    show_tree_min_updates = 1000;
    show_tree_max_children = 4;

    gtp.Register ("clear_board", this, &MctsGtp::CClear);
    gtp.Register ("genmove",     this, &MctsGtp::CGenmove);

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

private:

  void CClear (Gtp::Io& io) {
    io.CheckEmpty ();
    mcts.Reset ();
  }

  void CGenmove (Gtp::Io& io) {
    Player player = io.Read<Player> ();
    io.CheckEmpty ();

    Vertex v = mcts_engine.Genmove (player);

    io.out << v.to_string();
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
  FullBoard& full_board;

  float show_tree_min_updates;
  float show_tree_max_children;

  PlayoutGfx<Mcts> playout_gfx;
};
