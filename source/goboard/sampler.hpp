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
    
    Vertex last_v = board.LastVertex ();

    // Calculate local gammas
    is_in_local.Clear ();
    local_vertices.Clear ();

    double total_non_local_gamma = act_gamma_sum [pl];
    double total_local_gamma = 0.0;

    // TODO optimize this
    if (board.ColorAt (last_v) != Color::OffBoard ()) {
      ForEachNat (Dir, d) { // TODO unroll loop
        Vertex nbr = last_v.Nbr (d);
        if (!is_in_local.IsMarked (nbr)) {
          is_in_local.Mark (nbr);
          local_vertices.Push (nbr);
          local_gamma [nbr] = act_gamma [nbr] [pl];
          total_non_local_gamma -= act_gamma [nbr] [pl];
        }

        local_gamma [nbr] *= proximity_bonus [d.Proximity()];
      }
    }

    rep (ii, local_vertices.Size ()) {
      Vertex local_v = local_vertices [ii];
      total_local_gamma += local_gamma [local_v];
    }

    // Draw sample.
    double total_gamma = total_non_local_gamma + total_local_gamma;
    double sample = random.NextDouble (total_gamma);

    if (kCheckAsserts) {
      double sum = 0.0;
      rep (ii, board.EmptyVertexCount()) {
        Vertex v = board.EmptyVertex (ii);
        if (!is_in_local.IsMarked (v)) {
          sum += act_gamma [v] [pl];
        }
      }
      CHECK (fabs (total_non_local_gamma - sum) < Gammas::kAccurancy);
    }

    // Local move ?
    if (sample < total_local_gamma) {
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

    // Not local move.
    sample -= total_local_gamma;
    ASSERT (sample < total_non_local_gamma || total_non_local_gamma == 0.0);
    

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

    // Pass
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

  Vertex ko_v;
};

#endif
