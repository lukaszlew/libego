
struct All2051Hash3x3 {
  All2051Hash3x3 () : random(123), min_hash (Hash3x3::Any ()) {
    gtp.Register ("gen_all_pat", this, &All2051Hash3x3::GtpGenerate);
  }

  void GtpGenerate (Gtp::Io& io) {
    io.CheckEmpty ();
    Generate (5000);
  }

  void Generate (uint n) {
    NewHashAt (Vertex::OfGtpString("A1"));
    NewHashAt (Vertex::OfGtpString("A2"));
    NewHashAt (Vertex::OfGtpString("B2"));

    rep (ii, n) {
      board.Load (empty);
      while (!board.BothPlayerPass ()) {
        Player pl = board.ActPlayer ();
        Vertex v = board.RandomLightMove (pl, random);
        board.PlayLegal (pl, v);
        rep (ii, board.Hash3x3ChangedCount ()) {
          NewHashAt (board.Hash3x3Changed (ii));
        }
      }
    }
  }


  void NewHashAt (Vertex v) {
    Hash3x3 hash = board.Hash3x3At (v);
    if (!hash.IsLegal (Player::Black()))  return;
    if (min_hash[hash] != Hash3x3::Any()) return;

    FastStack <Hash3x3, 8> all;

    Hash3x3 act = hash;
    rep (kk, 2) {
      Hash3x3 tmp = act;
      rep (ii, 4) {
        all.Push (act);
        act = act.Rotate90();
      }

      CHECK (act == tmp);
      act = act.Mirror();
    }

    CHECK (act == hash);
    CHECK (all.Size() == 8);

    uint min_ii = 0;
    rep (ii, 8) {
      if (all[ii].GetRaw() < all[min_ii].GetRaw()) min_ii = ii;
    }

    rep (ii, 8) {
      CHECK (min_hash [all [ii]] == Hash3x3::Any ());
    }

    rep (ii, 8) {
      min_hash [all [ii]] = all [min_ii];
    }
    unique.push_back (all [min_ii]);
    cerr << "New: " << unique.size() << " " << all [min_ii].ToString() << endl;
  }

  const Board empty;
  Board board;
  FastRandom random;
    
  vector <Hash3x3> unique;
  NatMap <Hash3x3, Hash3x3> min_hash;
};
