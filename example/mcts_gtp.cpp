class MctsGtp {
public:
  MctsGtp (Gtp::ReplWithGogui& gtp, FullBoard& full_board_, Mcts& mcts_)
    : mcts(mcts_), full_board(full_board_)
  {
    playout_count = 10000;
    gtp.Register ("genmove", this, &MctsGtp::CGenmove);

    gtp.RegisterParam ("MCTS.params", "Playouts_before_genmove",
                       &playout_count);
    gtp.RegisterParam ("MCTS.params", "UCT_explore_coeff",
                       &mcts.uct_explore_coeff);
    gtp.RegisterParam ("MCTS.params", "Min_updates_to_have_children",
                       &mcts.mature_update_count);
    gtp.RegisterParam ("MCTS.params", "Min_updates_to_print",
                       &mcts.print_update_count);
    gtp.RegisterParam ("MCTS.params", "E(score)_to_resign",
                       &mcts.resign_mean);
  }

private:
  void CGenmove (Gtp::Io& io) {
    Player player = io.Read<Player> ();
    io.CheckEmpty ();

    full_board.set_act_player(player); // TODO move player parameter to DoPlayouts

    mcts.DoNPlayouts (playout_count);
    cerr << mcts.ToString () << endl;

    Vertex v = mcts.BestMove (player);

    if (v != Vertex::resign ()) {
      bool ok = full_board.try_play (player, v);
      assert(ok);
      io.Out () << v.to_string();
    } else {
      io.Out () << "resign";
    }
  }
private:
  Mcts& mcts;
  FullBoard& full_board;
  float playout_count;
};
