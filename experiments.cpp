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

class stat_t {
public:
  float update_count;
  float sample_sum;
  float square_sample_sum;

  stat_t () {
    reset ();
  }
  
  void reset () {
    update_count       = 1.0; // TODO 
    sample_sum         = 0.0; // TODO
    square_sample_sum  = 0.0; // TODO
  }

  void update (float sample) {
    update_count       += 1.0;
    sample_sum         += sample;
    square_sample_sum  += sample * sample;
  }

  float mean () { return sample_sum / update_count; }

  float variance () {
    // VX = E(X^2) - EX ^ 2
    float m = mean ();
    return square_sample_sum / update_count - m * m;
  }

  float std_dev () { return sqrt (variance ()); }

  float get_update_count () { return update_count; }

};



class all_as_first_t : public gtp_engine_t {
public:

  stat_t stat_unconditional;
  move_map_t < stat_t > stat_given_move;
  stack_board_t* stack_board;

  uint playout_no;
  float aaf_fraction;
  float influence_scale;
  

public:

  all_as_first_t (stack_board_t& stack_board_, gtp_static_commands_t& sc) : stack_board (&stack_board_) 
  { 
    sc.extend ("gogui_analyze_commands", "dboard/AAF.move_value black/AAF.move_value black\n");
    sc.extend ("gogui_analyze_commands", "dboard/AAF.move_value white/AAF.move_value white\n");
    sc.extend ("gogui_analyze_commands", "none/AAF.set_influence_scale/AAF.set_influence_scale %s\n");
    sc.extend ("gogui_analyze_commands", "none/AAF.set_playout_number/AAF.set_playout_number %s\n");
    sc.extend ("gogui_analyze_commands", "none/AAF.set_aaf_fraction/AAF.set_aaf_fraction %s\n");
    playout_no = 50000;
    aaf_fraction = 0.5;
    influence_scale = 6.0;
  }

  void reset () {
    stat_unconditional.reset ();
    move_for_each_all (m)
      stat_given_move [m].reset ();
  }

  void do_playout (const board_t* base_board) {
    board_t mc_board [1];
    mc_board->load (base_board);
    simple_policy_t policy (mc_board);
    playout_t<simple_policy_t> playout (mc_board, policy);
    playout.run ();

    float score = mc_board->score ();
    stat_unconditional.update (score);

    uint aaf_move_count = uint (float(mc_board->move_no)*aaf_fraction);
    rep (m_cnt, aaf_move_count)
      stat_given_move [playout.history [m_cnt]].update (score);
  }

  virtual vector <string> get_command_names () const {
    vector <string> commands;
    commands.push_back ("AAF.move_value");
    commands.push_back ("AAF.set_influence_scale");
    commands.push_back ("AAF.set_playout_number");
    commands.push_back ("AAF.set_aaf_fraction");
    return commands;
  };


  virtual gtp_status_t exec_command (string command, istream& params, ostream& response) {
    
    if (command == "AAF.move_value") {
      player_t  player;
      if (!(params >> player)) return gtp_syntax_error;

      reset ();
      rep (ii, playout_no) 
        do_playout (stack_board->act_board ());

      float base_mean = stat_unconditional.mean ();
      vertex_map_t<float> means;

      vertex_for_each_all (v) {
        means [v] = stat_given_move [move_t (player, v)].mean () - base_mean;
        means [v] /= influence_scale;;
        if (stack_board->act_board()->color_at [v] != color::empty) 
          means [v] = 0.0;
      }
      response << means.to_string ();
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

