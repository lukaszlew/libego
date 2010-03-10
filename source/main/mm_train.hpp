#include "all_hash3x3.hpp"
#include "mm.hpp"

struct MmTrain {
  MmTrain () :
    random(123),
    pattern_level (uint(-1))
  {
    gtp.Register ("mm_train", this, &MmTrain::GtpMmTrain);
  }

  void GtpMmTrain (Gtp::Io& io) {
    string file_name = io.Read <string> ();
    string out_file_name = io.Read <string> ();
    uint   needed_moves = io.Read <uint> ();
    uint   epochs = io.Read <uint> ();
    io.CheckEmpty();

    ifstream file;
    ofstream out_file;

    file.open (file_name.c_str(), ifstream::in);
    if (!file.good()) {
      io.SetError ("Can't open in-file: " + file_name);
      return;
    }

    out_file.open (out_file_name.c_str(), ofstream::out);
    if (!out_file.good()) {
      io.SetError ("Can't open out-file: " + out_file_name);
      return;
    }
    
    cerr << "Initializing..." << endl << flush;
    Init ();
    cerr << "Reading game file..." << endl << flush;
    Read (file, needed_moves);
    file.close ();
    cerr << "Harvesting pattern data..." << endl << flush;
    Harvest();
    cerr << "Learning..." << endl << flush;
    Learn (epochs);
    cerr << "Dumping..." << endl << flush;
    Dump (out_file);
    cerr << "Done." << endl << flush;
    out_file.close ();
  }

  void Read (istream& in, double needed_move_count) {
    uint game_no = 0;
    uint total_move_count = 0;

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
      total_move_count += move_count;

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

    accept_prob = needed_move_count / total_move_count;
    WW (accept_prob);
  }

  void Init () {
    All2051Hash3x3 all2051;
    level_to_pattern.resize (2051);
    all2051.Generate (5000);
    CHECK (all2051.unique.size() == 2051);
    rep (level, 2051) {
      Hash3x3 all[8];
      all2051.unique [level].GetAll8Symmetries (all);
      rep (ii, 8) {
        pattern_level [all [ii]] = level;
      }
      level_to_pattern [level] = all [0];
    }
  }

  void Harvest () {
    rep (game_no, games.size()) {
      if (game_no % (games.size() / 50) == 0) {
        cerr << "." << flush;
      }
      const vector<Move> & moves = games[game_no];
      board.Clear ();
      
      rep (move_no, moves.size()) {
        Move m = moves [move_no];
        HarvestNewMatch (m);
        CHECK2 (board.IsLegal (m), {
          cerr
            << "Illegal move " << m.ToGtpString()
            << " nr " << move_no
            << " in file " << files[game_no]
            << endl;
        });
        board.PlayLegal (m);
      }
    }
    cerr << endl;
  }

  void HarvestNewMatch (Move m) {
    if (random.NextDouble () > accept_prob) return;

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

  void Learn (uint epochs) {
    WW(model.matches.size());

    model.PreprocessData ();
    model.Train (epochs);
  }

  void Dump (ostream& out) {
    vector <pair <double, uint> > sort_tab (2051);
    rep (level, 2051) {
      sort_tab [level].first  = model.gammas.Get (Mm::kPatternFeature, level);
      sort_tab [level].second = level;
    }
    std::sort (sort_tab.begin(), sort_tab.end());
    rep (level, 2051) {
      out 
        << setw(7) << level_to_pattern [sort_tab [level].second].GetRaw()  << ", "
        << sort_tab [level].first
        << endl;
    }
  }

  vector <vector <Move> > games;
  vector <string> files;
  double accept_prob;

  Board board;
  FastRandom random;
  Mm::BtModel model;

  NatMap <Hash3x3, uint> pattern_level;
  vector <Hash3x3> level_to_pattern;
};

MmTrain mm_train;
