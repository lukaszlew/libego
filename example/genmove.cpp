class Genmove {
public:
  Genmove (Gtp::ReplWithGogui& gtp, FullBoard& full_board_, Mcts& mcts_)
    : mcts(mcts_), full_board(full_board_)
  {
    playout_count = 10000;
    gtp.Register ("genmove", this, &Genmove::CGenmove);
    gtp.RegisterParam ("MCTS.params", "# playouts_before_genmove", &playout_count);
  }

private:
  void CGenmove (Gtp::Io& io) {
    Player player = io.Read<Player> ();
    io.CheckEmpty ();

    full_board.set_act_player(player);

    mcts.Reset ();
    mcts.DoNPlayouts (playout_count);
    cerr << mcts.ToString () << endl;

    Vertex v = mcts.BestMove ();

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
