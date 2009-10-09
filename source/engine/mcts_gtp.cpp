class MctsGtp {
public:
  MctsGtp (Gtp::ReplWithGogui& gtp, FullBoard& full_board_, Mcts& mcts_)
    : mcts(mcts_), full_board(full_board_), playout_gfx (gtp, mcts_, "MCTS.ShowLastPlayout")
  {
    playout_count = 10000;
    show_tree_min_updates = 1000;
    show_tree_max_children = 4;

    gtp.Register ("clear_board", this, &MctsGtp::CClear);
    gtp.Register ("genmove",     this, &MctsGtp::CGenmove);

    gtp.RegisterGfx ("MCTS.DoPlayouts",      "1", this, &MctsGtp::CDoPlayouts);
    gtp.RegisterGfx ("MCTS.DoPlayouts",   "1000", this, &MctsGtp::CDoPlayouts);
    gtp.RegisterGfx ("MCTS.DoPlayouts",  "10000", this, &MctsGtp::CDoPlayouts);
    gtp.RegisterGfx ("MCTS.DoPlayouts", "100000", this, &MctsGtp::CDoPlayouts);

    gtp.RegisterGfx ("MCTS.ShowTree",   "0", this, &MctsGtp::CShowTree);
    gtp.RegisterGfx ("MCTS.ShowTree",  "10", this, &MctsGtp::CShowTree);
    gtp.RegisterGfx ("MCTS.ShowTree", "100", this, &MctsGtp::CShowTree);
    gtp.RegisterGfx ("MCTS.ShowTree", "500", this, &MctsGtp::CShowTree);


    gtp.RegisterParam ("MCTS.params", "Playouts_before_genmove",
                       &playout_count);
    gtp.RegisterParam ("MCTS.params", "UCT_explore_coeff",
                       &mcts.playout.uct_explore_coeff);
    gtp.RegisterParam ("MCTS.params", "Min_updates_to_have_children",
                       &mcts.playout.mature_update_count);

    gtp.RegisterParam ("MCTS.params", "show_tree_min_updates",
                       &show_tree_min_updates);
    gtp.RegisterParam ("MCTS.params", "show_tree_max_children",
                       &show_tree_max_children);

    gtp.RegisterParam ("MCTS.params", "E(score)_to_resign",
                       &mcts.resign_mean);
  }

private:

  void CClear (Gtp::Io& io) {
    io.CheckEmpty ();
    mcts.Reset ();
  }

  void CGenmove (Gtp::Io& io) {
    Player player = io.Read<Player> ();
    io.CheckEmpty ();

    full_board.set_act_player(player); // TODO move player parameter to DoPlayouts

    mcts.DoNPlayouts (playout_count);
    cerr << mcts.ToString (show_tree_min_updates, show_tree_max_children) << endl;

    Vertex v = mcts.BestMove (player);

    if (v != Vertex::resign ()) {
      bool ok = full_board.try_play (player, v);
      assert(ok);
      io.out << v.to_string();
    } else {
      io.out << "resign";
    }
  }

  void CDoPlayouts (Gtp::Io& io) {
    uint n = io.Read <uint> (playout_count);
    io.CheckEmpty();
    mcts.DoNPlayouts (n);
  }

  void CShowTree (Gtp::Io& io) {
    uint min_updates  = io.Read <uint> (show_tree_min_updates);
    uint max_children = io.Read <uint> (show_tree_max_children);
    io.CheckEmpty();
    io.out << mcts.ToString (min_updates, max_children);
  }

private:
  Mcts& mcts;
  FullBoard& full_board;

  float playout_count;
  float show_tree_min_updates;
  float show_tree_max_children;

  PlayoutGfx<Mcts> playout_gfx;
};
