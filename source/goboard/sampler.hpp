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
  }


  ~Sampler () {
  }

  void NewPlayout () {
    // Prepare act_gamma and act_gamma_sum
    ForEachNat (Player, pl) {
      act_gamma_sum [pl] = 0.0;

      rep (ii, board.EmptyVertexCount()) {
        Vertex v = board.EmptyVertex (ii);
        act_gamma [v] [pl] = gammas.Get (board.Hash3x3At (v), pl);
        act_gamma_sum [pl] += act_gamma [v] [pl];
      }

      ko_v = board.KoVertex (); // TODO this assumes correct alernating play.
      act_gamma_sum [pl] -= act_gamma [ko_v] [pl];
      act_gamma [ko_v] [pl] = 0.0;
    }
  }

  // non-incremental version.
  void MovePlayed_Slow () {
    Player pl = board.ActPlayer ();

    // Prepare act_gamma and act_gamma_sum
    double sum = 0.0;

    rep (ii, board.EmptyVertexCount()) {
      Vertex v = board.EmptyVertex (ii);
      act_gamma [v] [pl] = gammas.Get (board.Hash3x3At (v), pl);
      sum += act_gamma [v] [pl];
    }

    ko_v = board.KoVertex ();
    sum -= act_gamma [ko_v] [pl];
    act_gamma [ko_v] [pl] = 0.0;

    act_gamma_sum [pl] = sum;
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

    // Select move based on act_gamma and act_gamma_sum
    double sample = random.NextDouble (act_gamma_sum [pl]);
    ASSERT (sample < act_gamma_sum [pl] || act_gamma_sum [pl] == 0.0);
    
    double sum = 0.0;
    rep (ii, board.EmptyVertexCount()) {
      Vertex v = board.EmptyVertex (ii);
      sum += act_gamma [v] [pl];
      if (sum > sample) {
        ASSERT (act_gamma_sum [pl] > 0.0);
        ASSERT (act_gamma [v] [pl] > 0.0);
        return v;
      }
    }

    return Vertex::Pass();
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
                board.DebugPrint(v);
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

  const Board& board;
  const Gammas& gammas;

  // The invariant is that act_gamma[v] is correct for all empty 
  // vertices except KoVertex() where it is 0.0.
  // act_gamma_sum is a sum of the above.
  NatMap <Vertex, NatMap<Player, double> > act_gamma;
  NatMap <Player, double> act_gamma_sum;
  Vertex ko_v;

};

#endif
