/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                           *
 *  This file is part of Library of Effective GO routines - EGO library      *
 *                                                                           *
 *  Copyright 2006 and onwards, Lukasz Lew                                   *
 *                                                                           *
 *  EGO library is free software; you can redistribute it and/or modify      *
 *  it under the terms of the GNU General Public License as published by     *
 *  the Free Software Foundation; either version 2 of the License, or        *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  EGO library is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public License        *
 *  along with EGO library; if not, write to the Free Software               *
 *  Foundation, Inc., 51 Franklin St, Fifth Floor,                           *
 *  Boston, MA  02110-1301  USA                                              *
 *                                                                           *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// ----------------------------------------------------------------------

class Stat {
public:
  float sample_count;
  float sample_sum;
  float square_sample_sum;

  Stat () {
    reset ();
  }
  
  void reset (float prior_sample_count = 1.0) {
    sample_count       = prior_sample_count; // TODO 
    sample_sum         = 0.0; // TODO
    square_sample_sum  = 0.0; // TODO
  }

  void update (float sample) {
    sample_count       += 1.0;
    sample_sum         += sample;
    square_sample_sum  += sample * sample;
  }

  float mean () { 
    return sample_sum / sample_count; 
  }

  float variance () {
    // VX = E(X^2) - EX ^ 2
    float m = mean ();
    return square_sample_sum / sample_count - m * m;
  }

  float std_dev () { return sqrt (variance ()); }
  float std_err () { return sqrt (variance () / sample_count); } // TODO assert sample_count
                                                                 

  float get_sample_count () { return sample_count; }

  string to_string (bool dont_show_unupdated = true) {
    if (dont_show_unupdated && sample_count < 2.0) return "           ";

    ostringstream out;
    char buf [100];
    sprintf (buf, "%+3.1f(%5.0f)", mean (), sample_count);
    out << buf;
    return out.str ();
  }

};

// ----------------------------------------------------------------------

class AafStats {
public:
  Stat unconditional;
  FastMap<Move, Stat> given_move;

  void reset (float prior = 1.0) {
    unconditional.reset (prior);
    move_for_each_all (m)
      given_move [m].reset (prior);
  }

  void update (Move* move_history, uint move_count, float score) {
    unconditional.update (score);
    rep (ii, move_count)
      given_move [move_history [ii]].update (score);
  }

  float norm_mean_given_move (const Move& m) {
    return given_move[m].mean () - unconditional.mean ();   // ineffective in loop
  }
};

// ----------------------------------------------------------------------

class AllAsFirst : public GtpCommand {
public:
  Board*      board;
  AafStats    aaf_stats;
  uint        playout_no;
  float       aaf_fraction;
  float       influence_scale;
  float       prior;
  bool        progress_dots;

public:
  AllAsFirst (Gtp& gtp, Board& board_) : board (&board_) { 
    playout_no       = 50000;
    aaf_fraction     = 0.5;
    influence_scale  = 6.0;
    prior            = 1.0;
    progress_dots    = false;

    gtp.add_gogui_command (this, "dboard", "AAF.move_value", "black");
    gtp.add_gogui_command (this, "dboard", "AAF.move_value", "white");

    gtp.add_gogui_param_float ("AAF.params", "prior",            &prior);
    gtp.add_gogui_param_float ("AAF.params", "aaf_fraction",     &aaf_fraction);
    gtp.add_gogui_param_float ("AAF.params", "influence_scale",  &influence_scale);
    gtp.add_gogui_param_uint  ("AAF.params", "playout_number",   &playout_no);
    gtp.add_gogui_param_bool  ("AAF.params", "20_progress_dots", &progress_dots);
  }
    
  void do_playout (const Board* base_board) {
    Board mc_board [1];
    mc_board->load (base_board);

    SimplePolicy policy;
    Playout<SimplePolicy> playout(&policy, mc_board);
    playout.run ();

    uint aaf_move_count = uint (float(playout.move_history_length)*aaf_fraction);
    float score = mc_board->score ();

    aaf_stats.update (playout.move_history, aaf_move_count, score);
  }

  virtual GtpResult exec_command (const string& command, istream& params) {
    if (command == "AAF.move_value") {
      Player player;
      if (!(params >> player)) return GtpResult::syntax_error ();
      aaf_stats.reset (prior);
      rep (ii, playout_no) {
        if (progress_dots && (ii * 20) % playout_no == 0) cerr << "." << flush;
        do_playout (board);
      }
      if (progress_dots) cerr << endl;

      FastMap<Vertex, float> means;
      vertex_for_each_all (v) {
        means [v] = aaf_stats.norm_mean_given_move (Move(player, v)) / influence_scale;;
        if (board->color_at [v] != Color::empty ()) 
          means [v] = 0.0;
      }
      return GtpResult::success (to_string_2d (means));
    }

    assert (false);
  }
};
