namespace Param {
  bool mcmc_update = true;
  bool mcmc_update_fraction = 0.5;
  float mcmc_explore_coeff = 1000.0;
}

class Mcmc {
public:
  Mcmc () : move_stats (Stat()), ucb(0.0) {
  }

  void Reset () {
    ForEachNat (Move, m) {
      // TODO prior
      // stat.reset      (Param::prior_update_count,
      //                  player.SubjectiveScore (Param::prior_mean));
      move_stats [m].reset (1.0, 0.0);
      RecalcUcb (m);
      // TODO handle passes
    }
  }

  void RecalcUcb (Move m) {
    ucb [m] = m.GetPlayer().SubjectiveScore (move_stats [m].mean());
    ucb [m] += Param::mcmc_explore_coeff / sqrt (move_stats [m].update_count());
  }

  void Update (Move m, float score) {
    move_stats [m] . update (score);
    RecalcUcb (m);
  }

  Move ChooseMove (Player pl, const Board& board) {
    Move best_m = Move (pl, Vertex::Pass ());
    float best_value = ucb [best_m];

    empty_v_for_each (&board, v, {
      if (board.IsEyelike (pl, v) || !board.IsLegal (pl, v)) continue;
      Move m = Move (pl, v);
      float value = ucb [m];
      if (best_value < value) {
        best_value = value;
        best_m = m;
      }
    });

    return best_m;
  }

  NatMap <Move, Stat> move_stats;
  NatMap <Move, float> ucb;
};

// -----------------------------------------------------------------------------

class AllMcmc {
public:
  AllMcmc () : mcmc (Mcmc()) {
    Reset ();
  }

  void Reset () {
    ForEachNat (Move, m) mcmc [m].Reset ();
  }

  void Update (const Move* history, uint n, float score) {
    n *= Param::mcmc_update_fraction;
    NatMap <Move, bool> move_seen (false);
    rep (ii, n-2) {
      Move m = history[ii];
      Move m2 = history [ii+1];
      Move m3 = history [ii+2];
      ASSERT (m.IsValid());
      ASSERT (m2.IsValid());
      ASSERT (m3.IsValid());
      if (!move_seen [m]) {
        move_seen [m] = true;
        if (!move_seen [m2]) mcmc [m] . Update (m2, score);
        if (!move_seen [m3]) mcmc [m] . Update (m3, score);
      }
    }
  }

  Move ChooseMove (const Board& board) {
    return mcmc [board.LastMove()] . ChooseMove (board.ActPlayer (), board);
  }

  void MoveProbGfx (Move pre_move,
                    Player player,
                    const Board& board,
                    Gtp::GoguiGfx* gfx)
  {
    const uint n = 10000;
    NatMap <Move, float> prob (0.0);
    float max_prob = 0.0;
    rep (ii, n) {
      Move m = mcmc [pre_move] . ChooseMove (player, board); // TODO Pass ?
      cerr << m.ToGtpString() << "; ";
      prob [m] += 1.0 / n;
    }
    cerr << endl;

    ForEachNat (Vertex, v) {
      if (board.ColorAt (v) == Color::Empty () &&
          v != pre_move.GetVertex())
      {
        Move m = Move (player, v);
        max_prob = max (max_prob, prob[m]);
      }
    }

    ForEachNat (Vertex, v) {
      if (board.ColorAt (v) == Color::Empty () &&
          v != pre_move.GetVertex())
      {
        Move m = Move (player, v);
        gfx->SetInfluence (v.ToGtpString(), prob [m] / max_prob);
      }
    }
    
  }

  void MoveValueGfx (Move pre_move,
                     Player player,
                     const Board& board,
                     Gtp::GoguiGfx* gfx)
  {
    Stat stat(0.0, 0.0);

    ForEachNat (Vertex, v) {
      if (board.ColorAt (v) == Color::Empty () && v != pre_move.GetVertex()) {
        Move move = Move (player, v);
        float mean = mcmc [pre_move].ucb [move];
        stat.update (mean);
      }
    }

    ForEachNat (Vertex, v) {
      if (board.ColorAt (v) == Color::Empty () &&
          v != pre_move.GetVertex()) {
        Move move = Move (player, v);
        float mean = mcmc [pre_move].ucb [move];
        float val = (mean - stat.mean()) / stat.std_dev () / 4;
        gfx->SetInfluence (v.ToGtpString (), val);
        cerr << v.ToGtpString () << " : "
             << mcmc [pre_move].move_stats [move].to_string () << endl;
      }
    }
  }

  NatMap <Move, Mcmc> mcmc;
};
// -----------------------------------------------------------------------------

