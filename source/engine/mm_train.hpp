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
    Read (file);
    file.close ();
  }

  vector <vector <Move> > games;
};

MmTrain mm_train;
