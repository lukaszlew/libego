template <class Source>
class PlayoutGfx {
public:
  PlayoutGfx (Gtp::ReplWithGogui& gtp, Source& source_, const string& prefix)
    : source (source_)
  {
    const string cmd_name = prefix + "show";
    gtp.RegisterGfx (cmd_name, "new",  this, &PlayoutGfx::CShow);
    gtp.RegisterGfx (cmd_name, "more", this, &PlayoutGfx::CShow);
    gtp.RegisterGfx (cmd_name, "less", this, &PlayoutGfx::CShow);
    show_move_count = 6;
  }

  void CShow (Gtp::Io& io) {
    int n = io.Read<int> (-1);
    if (n > 0) {
      io.CheckEmpty ();
      show_move_count = n;
    } else {
      string sub = io.Read<string> ();
      io.CheckEmpty ();
      if (sub == "new") {
        show_move_count = 6;
        playout = source.NewPlayout ();
      } else if (sub == "more") {
        show_move_count += 1;
      } else if (sub == "less") {
        show_move_count -= 1;
      } else {
        io.SetError("syntax error");
        return;
      }
    }

    show_move_count = max(show_move_count, 0);
    show_move_count = min(show_move_count, int(playout.size()));

    Gfx gfx;

    rep(ii, show_move_count) {
      gfx.add_var_move(playout[ii]);
    }

    if (show_move_count > 0) {
      gfx.add_symbol(playout[show_move_count-1].get_vertex(), Gfx::circle);
    }

    io.out << gfx.to_string ();
  }

private:
  vector<Move> playout;
  int show_move_count;
  Source& source;
};
