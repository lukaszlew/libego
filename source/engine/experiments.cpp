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

class AafStats {
public:
  Stat unconditional;
  NatMap<Move, Stat> given_move;

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

class AllAsFirst {
public:
  FastRandom  random;
  FullBoard*  board;
  AafStats    aaf_stats;
  uint        playout_no;
  float       aaf_fraction;
  float       influence_scale;
  float       prior;
  bool        progress_dots;

public:
  AllAsFirst (Gtp::ReplWithGogui& gtp, FullBoard& board_)
  : random (123), board (&board_)
  { 
    playout_no       = 50000;
    aaf_fraction     = 0.5;
    influence_scale  = 6.0;
    prior            = 1.0;
    progress_dots    = false;

    gtp.RegisterGfx ("AAF.move_value", "black", this, &AllAsFirst::CMoveValue);
    gtp.RegisterGfx ("AAF.move_value", "white", this, &AllAsFirst::CMoveValue);

    gtp.RegisterParam ("AAF.params", "prior",            &prior);
    gtp.RegisterParam ("AAF.params", "aaf_fraction",     &aaf_fraction);
    gtp.RegisterParam ("AAF.params", "influence_scale",  &influence_scale);
    gtp.RegisterParam ("AAF.params", "playout_number",   &playout_no);
    gtp.RegisterParam ("AAF.params", "20_progress_dots", &progress_dots);
  }
    
  void do_playout (const FullBoard* base_board) {
    Board mc_board [1];
    mc_board->load (&base_board->board());

    LightPlayout playout(mc_board, random);
    LightPlayout::MoveHistory history;
    playout.Run (history);

    uint aaf_move_count = uint (float(history.Size())*aaf_fraction);
    float score = mc_board->playout_score ();

    aaf_stats.update (history.Data(), aaf_move_count, score);
  }

  void CMoveValue (Gtp::Io& io) {
    Player player = io.Read<Player> ();
    io.CheckEmpty ();

    aaf_stats.reset (prior);
    rep (ii, playout_no) {
      if (progress_dots && (ii * 20) % playout_no == 0) cerr << "." << flush;
      do_playout (board);
    }
    if (progress_dots) cerr << endl;

    Gtp::GoguiGfx gfx;

    for (Vertex v; v.Next(); ) {
      if (board->board().color_at [v] == Color::Empty ()) {
        gfx.SetInfluence(v.ToGtpString (),
                         aaf_stats.norm_mean_given_move (Move(player, v)) /
                         influence_scale
                         );
      }
    }

    gfx.Report (io);
  }
};
