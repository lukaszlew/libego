/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                           *
 *  This file is part of Library of Effective GO routines - EGO library      *
 *                                                                           *
 *  Copyright 2006, 2007 Lukasz Lew                                          *
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


class aaf_stats_t {
public:
  stat_t unconditional;
  move_t::map_t <stat_t> given_move;

  void reset () {
    unconditional.reset ();
    move_for_each_all (m)
      given_move [m].reset ();
  }

  void update (move_t* move_history, uint move_count, float score) {
    unconditional.update (score);
    rep (ii, move_count)
      given_move [move_history [ii]].update (score);
  }

  float norm_mean_given_move (const move_t& m) {
    return given_move[m].mean () - unconditional.mean ();   // ineffective in loop
  }
};


class all_as_first_t : public gtp_engine_t {
public:
  board_t*    board;
  aaf_stats_t aaf_stats;
  uint        playout_no;
  float       aaf_fraction;
  float       influence_scale;
  
public:
  all_as_first_t (board_t& board_) : board (&board_) { 
    playout_no = 50000;
    aaf_fraction = 0.5;
    influence_scale = 6.0;
  }

  template <typename gtp_static_commands_t>
  void register_static_commands (gtp_static_commands_t& sc) {
    sc.extend ("gogui_analyze_commands", "dboard/AAF.move_value black/AAF.move_value black\n");
    sc.extend ("gogui_analyze_commands", "dboard/AAF.move_value white/AAF.move_value white\n");
    sc.extend ("gogui_analyze_commands", "none/AAF.set_influence_scale/AAF.set_influence_scale %s\n");
    sc.extend ("gogui_analyze_commands", "none/AAF.set_playout_number/AAF.set_playout_number %s\n");
    sc.extend ("gogui_analyze_commands", "none/AAF.set_aaf_fraction/AAF.set_aaf_fraction %s\n");
  }
    
  void do_playout (const board_t* base_board) {
    board_t mc_board [1];
    mc_board->load (base_board);
    playout_t<simple_policy_t> (mc_board).run ();

    float score = mc_board->score ();
    uint aaf_move_count = uint (float(mc_board->move_no)*aaf_fraction);

    aaf_stats.update (mc_board->move_history, aaf_move_count, score);
  }

  virtual vector <string> get_command_names () const {
    vector <string> commands;
    commands.push_back ("AAF.move_value");
    commands.push_back ("AAF.set_influence_scale");
    commands.push_back ("AAF.set_playout_number");
    commands.push_back ("AAF.set_aaf_fraction");
    return commands;
  }


  virtual gtp_status_t exec_command (string command, istream& params, ostream& response) {
    if (command == "AAF.move_value") {
      player_t  player;
      if (!(params >> player)) return gtp_syntax_error;
      aaf_stats.reset ();
      rep (ii, playout_no) 
        do_playout (board);
      vertex_t::map_t<float> means;
      vertex_for_each_all (v) {
        means [v] = aaf_stats.norm_mean_given_move (move_t(player, v)) / influence_scale;;
        if (board->color_at [v] != color_t::empty ()) 
          means [v] = 0.0;
      }
      response << means.to_string_2d ();
      return gtp_success;
    }

    if (command == "AAF.set_influence_scale") {
      if (!(params >> influence_scale)) {
        response << "influence_scale = " << influence_scale;
        return gtp_failure;
      }
      return gtp_success;
    }

    if (command == "AAF.set_playout_number") {
      if (!(params >> playout_no)) {
        response << "playout_number = " << playout_no;
        return gtp_failure;
      }
      return gtp_success;
    }

    if (command == "AAF.set_aaf_fraction") {
      if (!(params >> aaf_fraction)) {
        response << "aaf_fraction = " << aaf_fraction;
        return gtp_failure;
      }
      return gtp_success;
    }

    fatal_error ("wrong command in gtp_genmove_t::exec_command");
    return gtp_panic; // formality 
  }

};
