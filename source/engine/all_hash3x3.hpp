
struct All2051Hash3x3 {
  All2051Hash3x3 () :
    empty (),
    random(123),
    min_hash (Hash3x3::Any ())
  {
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

    Hash3x3 all[8];

    hash.GetAll8Symmetries (all);

    rep (ii, 8) CHECK (min_hash [all [ii]] == Hash3x3::Any ());
    rep (ii, 8) min_hash [all [ii]] = all [0];

    unique.push_back (all [0]);
    //cerr << "New: " << unique.size() << " " << all [0].ToString() << endl;
  }

  const Board empty;
  Board board;
  FastRandom random;
    
  vector <Hash3x3> unique;
  NatMap <Hash3x3, Hash3x3> min_hash;
};
