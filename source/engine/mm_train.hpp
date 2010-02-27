struct MmTrain {
  MmTrain () {
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
    cerr << "Reading file..." << endl << flush;
    Read (file);
    cerr << "Constructing data ..." << endl << flush;
    Do();
    cerr << "Done." << endl << flush;
    file.close ();
  }

  void Do () {
    rep (game_no, games.size()) {
      const vector<Move> & moves = games[game_no];
      board.Clear ();
      
      rep (move_no, moves.size()) {
        Move m = moves [move_no];
        IFNCHECK (board.IsLegal (m), {
          rep (kk, moves.size()) {
            cerr << kk << " " << moves[kk].ToGtpString() << endl;
          }
          board.DebugPrint (m.GetVertex());
          WW(move_no);
          WW(files [game_no]);
        });
        board.PlayLegal (m);
      }
    }
  }

  vector <vector <Move> > games;
  vector <string> files;
  Board board;
};

MmTrain mm_train;
