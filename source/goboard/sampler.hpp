#ifndef _SAMPLER_HPP
#define _SAMPLER_HPP

#include <tr1/random>


struct Sampler {
  explicit Sampler (const Board& board, const Gammas& gammas) :
    board (board),
    gammas (gammas)
  {
    ForEachNat (Player, pl) {
      ForEachNat (Vertex, v) {
        act_gamma [v] [pl] = 0.0;
      }
      act_gamma_sum [pl] = 0.0;
    }
    proximity_bonus [0] = 10.0;
    proximity_bonus [1] = 10.0;
  }


  ~Sampler () {
  }

  void NewPlayout () {
    // Prepare act_gamma and act_gamma_sum
    ForEachNat (Player, pl) {
      // TODO memcpy
      act_gamma_sum [pl] = 0.0;
      ForEachNat (Vertex, v) {
        act_gamma [v] [pl] = 0.0;
      }

      rep (ii, board.EmptyVertexCount()) {
        Vertex v = board.EmptyVertex (ii);
        act_gamma [v] [pl] = gammas.Get (board.Hash3x3At (v), pl);
        act_gamma_sum [pl] += act_gamma [v] [pl];
      }
    }

    Player act_pl = board.ActPlayer();
    ko_v = board.KoVertex (); // TODO this assumes correct alernating play.
    act_gamma_sum [act_pl] -= act_gamma [ko_v] [act_pl];
    act_gamma [ko_v] [act_pl] = 0.0;

    CheckConsistency ();
  }


  void MovePlayed () {
    Player last_pl = board.LastPlayer();
    Vertex last_v  = board.LastVertex ();
    // Restore gamma after ko_ban lifted
    ASSERT (act_gamma [ko_v] [last_pl] == 0.0);
    act_gamma [ko_v] [last_pl] = gammas.Get (board.Hash3x3At (ko_v), last_pl); 
    act_gamma_sum [last_pl] += act_gamma [ko_v] [last_pl];

    ForEachNat (Player, pl) {
      // One new occupied intersection.
      ASSERT (board.ColorAt(last_v) != Color::Empty());
      act_gamma_sum [pl] -= act_gamma [last_v] [pl];
      act_gamma [last_v] [pl] = 0.0;

      // All new gammas.
      uint n = board.Hash3x3ChangedCount ();
      rep (ii, n) {
        Vertex v = board.Hash3x3Changed (ii);
        ASSERT (board.ColorAt(v) == Color::Empty());

        act_gamma_sum [pl] -= act_gamma [v] [pl];
        act_gamma [v] [pl] = gammas.Get (board.Hash3x3At (v), pl);
        act_gamma_sum [pl] += act_gamma [v] [pl];
      }
    }

    // New illegal ko point.
    Player act_pl  = board.ActPlayer();
    ko_v = board.KoVertex();
    ASSERT (board.ColorAt(ko_v) == Color::Empty() || ko_v == Vertex::Any ());
    act_gamma_sum [act_pl] -= act_gamma [ko_v] [act_pl];
    act_gamma [ko_v] [act_pl] = 0.0;

    CheckConsistency ();
  }


  double Probability (Player pl, Vertex v) const {
    // TODO no locality here !
    CheckConsistency ();
    double g  = act_gamma [v] [pl];
    double tg = act_gamma_sum [pl];
    double p = g / (tg + Gammas::kAccurancy);
    ASSERT2 (!isnan (p), WW(g); WW(tg));
    ASSERT2 (p >= 0.0,   WW(g); WW(tg));
    ASSERT2 (p <= 1.0,   WW(g); WW(tg));
    return p;
  }

  
  Vertex SampleMove (FastRandom& random) {
    Player pl = board.ActPlayer ();

    if (act_gamma_sum [pl] < Gammas::kAccurancy) {
      // TODO assert no_more_legal_moves
      return Vertex::Pass ();
    }
    
    CalculateLocalGammas ();

    // Draw sample.
    double sample = random.NextDouble (total_non_local_gamma + total_local_gamma);

    // Local move ?
    if (sample < total_local_gamma) {
      return SampleLocalMove (sample);
    } else {
      sample -= total_local_gamma;
      return SampleNonLocalMove (sample);
    }
  }


  void CalculateLocalGammas () {
    Player pl = board.ActPlayer ();
    is_in_local.Clear ();
    local_vertices.Clear ();
    total_non_local_gamma = act_gamma_sum [pl];
    total_local_gamma = 0.0;

    Vertex last_v = board.LastVertex ();

    if (board.ColorAt (last_v) != Color::OffBoard ()) {
      // TODO optimize this
      ForEachNat (Dir, d) { // TODO unroll loop
        Vertex nbr = last_v.Nbr (d);
        EnsureLocal (nbr);
        local_gamma [nbr] *= proximity_bonus [d.Proximity()];
      }
    }

    rep (ii, local_vertices.Size ()) {
      Vertex local_v = local_vertices [ii];
      total_local_gamma += local_gamma [local_v];
    }

    CheckLocalSumCorrect ();
  }


  void EnsureLocal (Vertex v) {
    if (!is_in_local.IsMarked (v)) {
      Player pl = board.ActPlayer ();
      is_in_local.Mark (v);
      local_vertices.Push (v);
      local_gamma [v] = act_gamma [v] [pl];
      total_non_local_gamma -= act_gamma [v] [pl];
    }
  }


  Vertex SampleLocalMove (double sample) {
    double local_gamma_sum = 0.0;
    rep (ii, local_vertices.Size ()) {
      Vertex nbr = local_vertices [ii];
      local_gamma_sum += local_gamma [nbr];
      if (local_gamma_sum >= sample) {
        return nbr;
      }
    }
    CHECK (false);
  }


  Vertex SampleNonLocalMove (double sample) {
    ASSERT (sample < total_non_local_gamma || total_non_local_gamma == 0.0);
    Player pl = board.ActPlayer();
    double sum = 0.0;
    rep (ii, board.EmptyVertexCount()) {
      Vertex v = board.EmptyVertex (ii);
      if (is_in_local.IsMarked (v)) continue;
      sum += act_gamma [v] [pl];
      if (sum > sample) {
        ASSERT (total_non_local_gamma > 0.0);
        ASSERT (act_gamma [v] [pl] > 0.0);
        return v;
      }
    }
    return Vertex::Pass();
  }

  void SampleMany (uint nn, NatMap <Vertex,double>& count) {
    FastRandom fr;
    count.SetAll (0.0);
    ForEachNat (Vertex, v) {
      if (v.IsOnBoard () && board.IsLegal (board.ActPlayer(), v)) count [v] = 0.0;
      else count [v] = nan("");
    }

    rep (ii, nn) {
      Vertex v = SampleMove (fr);
      count [v] += 1.0;
    }
  }

  void GetPatternGammas (NatMap <Vertex,double>& gamma, bool use_local_features) {
    ForEachNat (Vertex, v) {
      gamma [v] = act_gamma [v] [board.ActPlayer()];
    }

    if (use_local_features) {
      CalculateLocalGammas ();
      rep (ii, local_vertices.Size ()) {
        Vertex nbr = local_vertices [ii];
        gamma [nbr] = local_gamma [nbr];
      }
    }

    ForEachNat (Vertex, v) {
      if (gamma [v] == 0.0) gamma[v] = nan ("");
    }
  }

private:

  void CheckLocalSumCorrect () const {
    // Tests
    if (!kCheckAsserts) return;
    Player pl = board.ActPlayer();
    double sum = 0.0;
    rep (ii, board.EmptyVertexCount()) {
      Vertex v = board.EmptyVertex (ii);
      if (!is_in_local.IsMarked (v)) {
        sum += act_gamma [v] [pl];
      }
    }
    CHECK (fabs (total_non_local_gamma - sum) < Gammas::kAccurancy);
  }


  void CheckSumCorrect (string id = "x") const {
    if (!kCheckAsserts) return;

    ForEachNat (Player, pl) {
      double sum = 0.0;
      ForEachNat (Vertex, v) {
        sum += act_gamma [v] [pl];
      }
      CHECK2 (fabs (act_gamma_sum [pl] - sum) < Gammas::kAccurancy,
              WW (act_gamma_sum[pl]);
              WW(sum);
              WW(id);
              board.Dump());
    }
  }


  void CheckValuesCorrect (string id = "x") const {
    if (!kCheckAsserts) return;

    ForEachNat (Player, pl) {
      ForEachNat (Vertex, v) {
        double correct;
        if (board.ColorAt(v) != Color::Empty ()) {
          correct = 0.0;
        } else if (pl == board.ActPlayer() && v == board.KoVertex ()) {
          correct = 0.0;
        } else {
          correct = gammas.Get (board.Hash3x3At (v), pl);
        }
        CHECK2 (correct == act_gamma [v] [pl],
                WW (act_gamma[v][pl]);
                WW(correct);
                WW(id);
                board.Dump1(v);
                WW(board.KoVertex().ToGtpString());
                WW (pl.ToGtpString());
                WW (board.ActPlayer().ToGtpString());
                );
      }
    }
  }
  

  void CheckConsistency (const char* id = "x") const {
    if (!kCheckAsserts) return;

    CheckSumCorrect (id);
    CheckValuesCorrect (id);
  }

public:

  // The invariant is that act_gamma[v] is correct for all empty 
  // vertices except KoVertex() where it is 0.0.
  // act_gamma_sum is a sum of the above.
  NatMap <Vertex, NatMap<Player, double> > act_gamma;
  NatMap <Player, double> act_gamma_sum;
  double proximity_bonus [2]; // TODO move this to Gammas 

private:
  const Board& board;
  const Gammas& gammas;

  NatSet <Vertex> is_in_local;
  FastStack <Vertex, Board::kArea> local_vertices;
  NatMap <Vertex, double> local_gamma;
  double total_non_local_gamma;
  double total_local_gamma;


  Vertex ko_v;
};

#endif
