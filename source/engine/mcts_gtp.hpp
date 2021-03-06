#ifndef MCTS_GTP_H_
#define MCTS_GTP_H_

#include <fstream>

extern Gtp::ReplWithGogui gtp;

class MctsGtp {
public:
  MctsGtp (Engine& engine)
  : engine (engine)
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
    gtp.Register ("gui",          this, &MctsGtp::Cgui);

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

    gtp.RegisterGfx ("MCTS.show",    "0 4", this, &MctsGtp::CShowTree);
    gtp.RegisterGfx ("MCTS.show",   "10 4", this, &MctsGtp::CShowTree);
    gtp.RegisterGfx ("MCTS.show",  "100 4", this, &MctsGtp::CShowTree);
    gtp.RegisterGfx ("MCTS.show", "1000 4", this, &MctsGtp::CShowTree);
  }

  void RegisterParams () {
    string tree  = "param.tree";
    string other = "param.other";
    string set = "set";

    gtp.RegisterParam (other, "genmove_playouts",     &Param::genmove_playouts);
    gtp.RegisterParam (other, "local_use",            &Param::use_local);
    gtp.RegisterParam (other, "seed",                 &engine.random.seed);

    gtp.RegisterParam (tree, "use",             &Param::tree_use);
    gtp.RegisterParam (tree, "max_moves",       &Param::tree_max_moves);
    gtp.RegisterParam (tree, "explore_coeff",   &Param::tree_explore_coeff);
    gtp.RegisterParam (tree, "rave_update",     &Param::tree_rave_update);
    gtp.RegisterParam (tree, "rave_use",        &Param::tree_rave_use);
    gtp.RegisterParam (tree, "stat_bias",       &Param::tree_stat_bias);
    gtp.RegisterParam (tree, "rave_bias",       &Param::tree_rave_bias);
    gtp.RegisterParam (tree, "rave_update_fraction", &Param::tree_rave_update_fraction);
    gtp.RegisterParam (tree, "progressive_bias",&Param::tree_progressive_bias);

    gtp.RegisterParam (set, "progressive_bias",       &Param::tree_progressive_bias);
    gtp.RegisterParam (set, "progressive_bias_prior", &Param::tree_progressive_bias_prior);
    gtp.RegisterParam (set, "explore_coeff",          &Param::tree_explore_coeff);

    gtp.RegisterParam (set, "proxy_1_bonus", &engine.sampler.proximity_bonus[0]);
    gtp.RegisterParam (set, "proxy_2_bonus", &engine.sampler.proximity_bonus[1]);
  }

  void Cclear_board (Gtp::Io& io) {
    io.CheckEmpty ();
    CHECK (engine.Reset (board_size));
  }

  void Cgenmove (Gtp::Io& io) {
    Player player = io.Read<Player> ();
    io.CheckEmpty ();
    Move m = engine.Genmove (player);
    io.out << (m.IsValid() ? m.GetVertex().ToGtpString() : "resign");
  }

  void Cboardsize (Gtp::Io& io) {
    int new_board_size = io.Read<int> ();
    io.CheckEmpty ();
    if (!engine.Reset (new_board_size)) {
      io.SetError ("unacceptable size");
      return;
    }
  }

  void Ckomi (Gtp::Io& io) {
    float new_komi = io.Read<float> ();
    io.CheckEmpty();
    engine.SetKomi (new_komi);
  }

  void Cplay (Gtp::Io& io) {
    Move move = io.Read<Move> ();
    io.CheckEmpty ();

    if (!engine.Play (move)) {
      io.SetError ("illegal move");
      return;
    }
  }

  void Cundo (Gtp::Io& io) {
    io.CheckEmpty ();
    if (!engine.Undo ()) {
      io.SetError ("too many undo");
      return;
    }
  }

  void Cshowboard (Gtp::Io& io) {
    io.CheckEmpty ();
    io.out << engine.GetBoard().ToAsciiArt ();
  }

  void CDoPlayouts (Gtp::Io& io) {
    uint n = io.Read <uint> (Param::genmove_playouts);
    io.CheckEmpty();
    engine.DoNPlayouts (n);
  }

  void CShowLastPlayout (Gtp::Io& io) {
    uint move_count = io.Read<uint> ();
    io.CheckEmpty ();

    vector<Move> last_playout = engine.LastPlayout ();

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

    //     rep (ii, mcmc.moves.size()) {
    //       gfx.SetSymbol (mcmc.moves[ii].GetVertex().ToGtpString(),
    //                      Gtp::GoguiGfx::triangle);
    //     }

    gfx.Report (io);
  }

  void CShowGammas (Gtp::Io& io) {
    io.CheckEmpty ();
    Gtp::GoguiGfx gfx;
    Player pl = engine.base_board.ActPlayer ();
    engine.PrepareToPlayout ();
    NatMap <Vertex, double> p (0.0);
    ForEachNat (Vertex, v) {
      if (engine.base_board.ColorAt (v) == Color::Empty()) {
        p [v] = engine.sampler.act_gamma [v] [pl];
      }
    }
    p.Scale (0.0, 1.0);
    ForEachNat (Vertex, v) {
      if (engine.base_board.ColorAt (v) == Color::Empty()) {
        gfx.SetInfluence (v.ToGtpString(), p[v]);
      }
    }
    gfx.Report (io);
  }


  void CShowTree (Gtp::Io& io) {
    uint min_updates  = io.Read <uint> ();
    uint max_children = io.Read <uint> ();
    io.CheckEmpty();
    io.out << endl << engine.base_node->RecToString (min_updates, max_children);
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
    if (!engine.gammas.Read (in)) {
      io.SetError ("File in a bad format.");
      return;
    }
    in.close();
  }

  void Cgui (Gtp::Io& io) {
    io.CheckEmpty ();
    //RunGui (engine);
  }

private:
  Engine& engine;
};

#endif /* MCTS_GTP_H_ */
