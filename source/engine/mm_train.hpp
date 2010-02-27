struct MmTrain {
  MmTrain () :
    random(123),
    pattern_level (uint(-1))
  {
    gtp.Register ("mm_train", this, &MmTrain::GtpMmTrain);
  }

  void Read (istream& in) {
    uint game_no = 0;
    string s;
    while (in >> s) {
      CHECK (s == "file");
      games.resize (game_no + 1);
      files.resize (game_no + 1);

      getline (in, files[game_no]);
      CHECK (in);

      uint bs;
      CHECK (in >> bs);
      
      uint move_count;
      CHECK (in >> move_count);
      games [game_no].resize (move_count);
      rep (ii, move_count) {
        Move m = Move::OfGtpStream (in);
        CHECK (m.IsValid ());
        games [game_no] [ii] = m;
      }
      if (bs == board_size) {
        game_no += 1;
      }
    }
  }

  void GtpMmTrain (Gtp::Io& io) {
    string file_name = io.Read <string> ();
    io.CheckEmpty();
    ifstream file;
    file.open (file_name.c_str(), ifstream::in);
    if (!file.good()) {
      io.SetError ("Can't find file: " + file_name);
      return;
    }
    cerr << "Initializing..." << endl << flush;
    Init ();
    cerr << "Reading game file..." << endl << flush;
    Read (file);
    cerr << "Harvesting pattern data ..." << endl << flush;
    Harvest();
    cerr << "Done." << endl << flush;
    file.close ();
  }

  void Init () {
    All2051Hash3x3 all2051;
    all2051.Generate (5000);
    CHECK (all2051.unique.size() == 2051);
    rep (level, 2051) {
      Hash3x3 all[8];
      all2051.unique [level].GetAll8Symmetries (all);
      rep (ii, 8) {
        pattern_level [all [ii]] = level;
      }
    }
  }

  void Harvest () {
    rep (game_no, games.size()) {
      if (game_no % (games.size() / 50) == 0) {
        WW(model.matches.size());
        cerr << "." << flush;
      }
      const vector<Move> & moves = games[game_no];
      board.Clear ();
      
      rep (move_no, moves.size()) {
        Move m = moves [move_no];
        HarvestNewMatch (m);
        IFNCHECK (board.IsLegal (m), {
          cerr
            << "Illegal move " << m.ToGtpString()
            << " nr " << move_no
            << " in file " << files[game_no]
            << endl;
        });
        board.PlayLegal (m);
      }
    }
  }

  void HarvestNewMatch (Move m) {
    if (random.GetNextUint(32) >= 1) return;

    Player pl = m.GetPlayer ();

    Mm::Match& match = model.NewMatch();

    rep (ii, board.EmptyVertexCount()) {
      Vertex v = board.EmptyVertex (ii);
      if (!board.IsLegal (pl, v)) continue;

      Hash3x3 hash = board.Hash3x3At (v);

      if (pl == Player::White ()) {
        hash = hash.InvertColors();
      }

      CHECK (pattern_level [hash] != uint(-1));

      Mm::Team& team = match.NewTeam();
      team.SetFeatureLevel (Mm::kPatternFeature, pattern_level [hash]);

      if (v == m.GetVertex()) {
        match.SetWinnerLastTeam();
      }
    }
  }

  vector <vector <Move> > games;
  vector <string> files;
  Board board;
  FastRandom random;
  Mm::BtModel model;

  NatMap <Hash3x3, uint> pattern_level;
};

MmTrain mm_train;
