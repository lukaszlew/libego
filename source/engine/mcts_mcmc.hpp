namespace Param {
  bool mcmc_update = true;
  bool mcmc_update_fraction = 0.5;
  float mcmc_explore_coeff = 1000.0;
}

class Mcmc {
public:
  Mcmc () : move_stats (Stat()), mean (0.0), noise_factor(0.0) {
  }

  void Reset () {
    ForEachNat (Move, m) {
      // TODO prior
      // stat.reset      (Param::prior_update_count,
      //                  player.SubjectiveScore (Param::prior_mean));
      move_stats [m].reset (1.0, 0.0);
      // TODO handle passes
    }
  }

  void Update (const Move* tab, int n, float score,
               const NatMap <Move, bool>& move_seen)
  {
    ASSERT (n >= 1);
    move_stats [tab[0]].update (score); // order 1 stats
    if (n >= 2 && !move_seen [tab[1]]) move_stats [tab[1]].update (score); 
    if (n >= 3 && !move_seen [tab[2]]) move_stats [tab[2]].update (score);
  }

  void RecalcValues () {
    ForEachNat (Move, m) {
      noise_factor [m] =
        1.0 /
        sqrt (move_stats[m].update_count ());
      Player pl = m.GetPlayer();
      mean [m] = pl.SubjectiveScore (move_stats[m].mean ());
    }
    mean [Move (Player::Black(), Vertex::Pass ())] = - 1.0E20;
    mean [Move (Player::White(), Vertex::Pass ())] = 1.0E20;
  }

  Move ChooseMove (Player pl, const Board& board, const NatMap<Move, float>& noise) {
    Move best_m = Move (pl, Vertex::Pass ());
    float best_value = mean [best_m] + noise [best_m] * noise_factor [best_m];

    empty_v_for_each (&board, v, {
      if (board.IsEyelike (pl, v) || !board.IsLegal (pl, v)) continue;
      Move m = Move (pl, v);
      float value = mean [m] + noise [m] * noise_factor [m];
      if (best_value < value) {
        best_value = value;
        best_m = m;
      }
    });

    return best_m;
  }

  NatMap <Move, Stat> move_stats;
  NatMap <Move, float> mean;
  NatMap <Move, float> noise_factor;
};

// -----------------------------------------------------------------------------

class AllMcmc {
public:
  AllMcmc () : mcmc (Mcmc()), noise (0.0) {
    Reset ();
    RecalcValues ();
  }

  void Reset () {
    ForEachNat (Move, m) mcmc [m].Reset ();
  }

  void RecalcValues () {
    ForEachNat (Move, m) mcmc [m].RecalcValues ();
  }

  void Update (const Move* history, uint n, float score) {
    n *= Param::mcmc_update_fraction;
    NatMap <Move, bool> move_seen (false);
    rep (ii, n) {
      Move m = history[ii];
      if (!move_seen [m]) {
        move_seen [m] = true;
        ASSERT (m.IsValid());
        mcmc [m] . Update (history + ii, n - ii, score, move_seen);
      }
    }
  }

  void SetNoise () {
    ForEachNat (Move, m) {
      noise [m] = drand48 () * Param::mcmc_explore_coeff;
    }
  }

  Move ChooseMove (const Board& board) {
    return mcmc [board.LastMove()] . ChooseMove (board.ActPlayer (), board, noise);
  }

  void MoveProbGfx (Move pre_move,
                    Player player,
                    const Board& board,
                    Gtp::GoguiGfx* gfx)
  {
    RecalcValues();
    const uint n = 10000;
    NatMap <Move, float> prob (0.0);
    float max_prob = 0.0;
    rep (ii, n) {
      SetNoise ();
      Move m = mcmc [pre_move] . ChooseMove (player, board, noise);
      prob [m] += 1.0 / n;
    }

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
        float mean = mcmc [pre_move].move_stats [move].mean ();
        stat.update (mean);
      }
    }

    ForEachNat (Vertex, v) {
      if (board.ColorAt (v) == Color::Empty () &&
          v != pre_move.GetVertex()) {
        Move move = Move (player, v);
        float mean = mcmc [pre_move].move_stats [move].mean ();
        float val = (mean - stat.mean()) / stat.std_dev () / 4;
        gfx->SetInfluence (v.ToGtpString (), val);
        cerr << v.ToGtpString () << " : "
             << mcmc [pre_move].move_stats [move].to_string () << endl;
      }
    }
  }

  NatMap <Move, Mcmc> mcmc;
  NatMap <Move, float> noise;
};
// -----------------------------------------------------------------------------

