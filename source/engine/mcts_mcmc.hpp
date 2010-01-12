#define for_each_8_nbr(center_v, nbr_v, block) {                \
    Vertex nbr_v = Vertex::Invalid();                           \
    nbr_v = center_v.N (); block;                               \
    nbr_v = center_v.W (); block;                               \
    nbr_v = center_v.E (); block;                               \
    nbr_v = center_v.S (); block;                               \
    nbr_v = center_v.NW (); block;                              \
    nbr_v = center_v.NE (); block;                              \
    nbr_v = center_v.SW (); block;                              \
    nbr_v = center_v.SE (); block;                              \
  }


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

  float GfxValue (Move m) const {
    return move_stats [m].mean();
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
    CHECK (n >= 2);
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

  Vertex Choose8Move (const Board& board, const NatMap<Vertex, uint>& v_seen, FastRandom& random) {
    Player pl = board.ActPlayer();
    Vertex last_v = board.LastVertex();

    if (!last_v.IsValid() ||
        last_v == Vertex::Pass () ||
        random.GetNextUint(256) < 50)
    {
      return board.RandomLightMove (pl, random);
    }

    Move last_m = board.LastMove ();
    Vertex best_v;
    float best_value = - 1E20;

    for_each_8_nbr (last_v, nbr, {
      if (v_seen[nbr] == 0 &&
          board.IsLegal (pl, nbr) &&
          !board.IsEyelike (pl, nbr))
      {
        float value = mcmc [last_m].ucb [Move(pl, nbr)];
        if (best_value < value) {
          best_value = value;
          best_v = nbr;
        }
      }
    });

    if (best_v.IsValid ()) return best_v;

    return board.RandomLightMove (pl, random);
  }

  void MoveProbGfx (Move pre_move,
                    Player player,
                    const Board& board,
                    Gtp::GoguiGfx* gfx)
  {
    unused (pre_move);
    unused (player);
    unused (board);
    unused (gfx);
  }

  void MoveValueGfx (Move pre_move,
                     Player player,
                     const Board& board,
                     Gtp::GoguiGfx* gfx)
  {
    Stat stat(0.01, 0.0);
    Vertex pre_v = pre_move.GetVertex();

    for_each_8_nbr (pre_v, v, {
      if (board.ColorAt (v) == Color::Empty () && v != pre_move.GetVertex()) {
        Move move = Move (player, v);
        float mean = mcmc [pre_move].GfxValue (move);
        stat.update (mean);
      }
    });

    for_each_8_nbr (pre_v, v, {
        //ForEachNat (Vertex, v) {
      if (board.ColorAt (v) == Color::Empty () &&
          v != pre_move.GetVertex()) {
        Move move = Move (player, v);
        float mean = mcmc [pre_move].GfxValue(move);
        float val = (mean - stat.mean()) / stat.std_dev () / 4;
        gfx->SetInfluence (v.ToGtpString (), val);
        cerr << v.ToGtpString () << " : "
             << mcmc [pre_move].move_stats [move].to_string () << endl;
      }
    });
  }

  NatMap <Move, Mcmc> mcmc;
};
// -----------------------------------------------------------------------------

