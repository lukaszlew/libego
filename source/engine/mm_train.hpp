struct MmTrain {
  MmTrain () {
    gtp.Register ("mm_train", this, &MmTrain::GtpMmTrain);
  }

  void Read (istream& in) {
    uint game_no = 0;
    while (true) {
      uint bs;
      if (!(in >> bs)) break;
      games.resize (game_no + 1);
      
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
    rep (ii, games.size()) {
      board.Clear ();
      rep (jj, games[ii].size()) {
        Move m = games [ii] [jj];
        IFNCHECK (board.IsLegal (m), {
          rep (kk, games[ii].size()) {
            cerr << kk << " " << games[ii][kk].ToGtpString() << endl;
          }
          board.DebugPrint (m.GetVertex());
          WW(ii);
          WW(jj);
        });
        board.PlayLegal (m);
      }
    }
  }

  vector <vector <Move> > games;
  Board board;
};

MmTrain mm_train;
