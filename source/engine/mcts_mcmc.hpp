
class Mcmc {
public:
  Mcmc () {
    Reset ();
  }

  void Reset () {
    // TODO prior randomization here!, optimizm/exploration
    ForEachNat (Move, m2) {
      ForEachNat (Move, m1) {
        ForEachNat (Move, m0) {
          float optimism = m0.GetPlayer().SubjectiveScore (1.0);
          ms3 [m2][m1][m0].stat.reset (10.0, optimism);
          ms3 [m2][m1][m0].rave.reset (10.0, optimism);
        }
      }
    }

    ForEachNat (Move, m1) {
      ForEachNat (Move, m0) {
        float optimism = m0.GetPlayer().SubjectiveScore (1.0);
        ms2 [m1][m0].stat.reset (10.0, optimism);
        ms2 [m1][m0].rave.reset (10.0, optimism);
      }
    }
  }

  void NewPlayout () {
  }

  void Update (float score, Move pre, const vector<Move>& history) {
    if (history.size() < 1) return;
    Move m2 = pre;
    Move m1 = history[0];

    NatMap<Vertex, uint> play_count; // TODO unify RAVE; set 1 on nonempty
    play_count.SetToZero ();
    play_count [m2.GetVertex()] += 1;
    play_count [m1.GetVertex()] += 1;
    play_count [Vertex::Any()] = 1; // beginning of the game is Any*2

    uint last_ii = history.size() * Param::mcmc_update_fraction;
    reps (ii, 1, last_ii) {
      Move m0 = history[ii];
      if (play_count [m2.GetVertex()] == 1 &&
          play_count [m1.GetVertex()] == 1 &&
          play_count [m0.GetVertex()] == 0)
      {
        Stat2& s2 = ms3 [m2] [m1] [m0];
        s2.stat.update (score);
        s2.stat.UpdateUcb (m0.GetPlayer (), Param::mcmc_explore_coeff);

        Stat2& s2b = ms2 [m1] [m0];
        s2b.stat.update (score);
        s2b.stat.UpdateUcb (m0.GetPlayer (), Param::mcmc_explore_coeff);

        MS1& ms1 = ms3 [m2] [m1];
        MS1& ms1b = ms2 [m1];
        reps (jj, ii, last_ii) {
          Move mr = history[jj];
          if (play_count [mr.GetVertex()] == 0) {
            ms1  [mr] . rave . update (score);
            ms1b [mr] . rave . update (score);
          }
        }

      }

      play_count [m0.GetVertex()] += 1;
      m2 = m1;
      m1 = m0;
    }
  }

  Vertex Choose8Move (const Board& board, const NatMap<Vertex, uint>& play_count) {
    Move m2 = board.LastMove2 ();
    Move m1 = board.LastMove ();

    Vertex last_v = board.LastVertex();

    if (play_count [m2.GetVertex ()] > 1) return Vertex::Any ();
    if (play_count [m1.GetVertex ()] > 1) return Vertex::Any ();

    Vertex best_v = Vertex::Any(); // any == light move
    float best_value = - 1E20;
    //MS1& my_ms1 = ms3 [m2] [m1];
    MS1& my_ms1 = ms2 [m1];
    Player pl = board.ActPlayer();
        
    ForEachNat (Vertex, nbr) {
      if (play_count[nbr] == 0 &&
          board.IsLegal (pl, nbr) &&
          !board.IsEyelike (pl, nbr))
      {
        Move m =  Move(pl, nbr);
        float value = my_ms1 [m].Mix();
        if (pl == Player::White()) value = -value;

        if (best_value < value) {
          best_value = value;
          best_v = nbr;
        }
      }
    }

    return best_v;
  }

  void MoveValueGfx (const Board& board, Gtp::GoguiGfx* gfx, int level)
  {

    float max_v = -1E20;
    float min_v = 1E20;

    //Param::rave_bias *= 20;
    

    CHECK (level == 1 || level == 2);
    MS1& my_ms1 =
      level == 1 ? 
      ms2 [board.LastMove ()] :
      ms3 [board.LastMove2 ()] [board.LastMove ()];
      ;
    Player pl = board.ActPlayer ();

    ForEachNat (Vertex, v) {
      if (board.ColorAt (v) == Color::Empty () && board.IsLegal (pl, v)) {
        Move m = Move (pl, v);
        float mean = my_ms1 [m].Mix();
        max_v = max (max_v, mean);
        min_v = min (min_v, mean);
      }
    }

    ostringstream out;
    out << "\"range: " << min_v << " ... " << max_v << "\"";
    gfx->SetStatusBar (out.str());

    ForEachNat (Vertex, v) {
      if (board.ColorAt (v) == Color::Empty ()) {
        Move m = Move (pl, v);
        float mean = my_ms1 [m].Mix();
        float val = (mean - min_v) / (max_v - min_v) * 2 - 1;
        gfx->SetInfluence (v.ToGtpString (), val);
        cerr << v.ToGtpString () << " : "
             << my_ms1 [m].ToString()
             << endl;
      }
    }

    //Param::rave_bias /= 20;
  }


  typedef NatMap <Move, Stat2> MS1;
  typedef NatMap <Move, MS1>  MS2;
  typedef NatMap <Move, MS2>  MS3;

  MS3 ms3;
  MS2 ms2;
};
