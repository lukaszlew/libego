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




// class gtp_sgf

class gtp_sgf_t : public gtp_engine_t {
public:
  sgf_tree_t&       sgf_tree;
  gtp_t&            gtp;
  board_t&          base_board;
  

  gtp_sgf_t (sgf_tree_t& _sgf_tree, gtp_t& _gtp, board_t& _base_board) : 
    sgf_tree (_sgf_tree), gtp (_gtp), base_board (_base_board)
  { }

  virtual vector <string> get_command_names () const {
    vector <string> commands;
    commands.push_back ("sgf.load");
    commands.push_back ("sgf.save");
    commands.push_back ("sgf.gtp.exec");
    return commands;
  };

  virtual gtp_status_t exec_command (string command, istream& params, ostream& response) {
    // ---------------------------------------------------------------------
    if (command == "sgf.load") {
      string file_name;

      if (!(params >> file_name)) return gtp_syntax_error;

      ifstream sgf_stream (file_name.data ());
      if (!sgf_stream) {
        response << "file not found: " << file_name << endl;
        return gtp_failure;
      }

      if (sgf_tree.parse_sgf (sgf_stream) == false) {
        response << "invalid SGF file" << endl;
        return gtp_failure;
      }

      return gtp_success;
    }

    // ---------------------------------------------------------------------
    if (command == "sgf.save") {
      string file_name;

      if (!(params >> file_name)) return gtp_syntax_error;

      ofstream sgf_stream (file_name.data ());
      if (!sgf_stream) {
        response << "file cound not be created: " << file_name << endl;
        return gtp_failure;
      }
      
      sgf_stream << sgf_tree.to_sgf_string () << endl;
      sgf_stream.close ();

      return gtp_success;
    }

    // ---------------------------------------------------------------------
    if (command == "sgf.gtp.exec") {
      if (!sgf_tree.is_loaded ()) {
        response << "SGF file not loaded" << endl;
        return gtp_failure;
      }

      if (sgf_tree.properties ().get_board_size () != board_size) {
        response << "invalid board size";
        return gtp_failure;
      }

      board_t save_board;
      save_board.load (&base_board);
      
      base_board.clear ();
      base_board.set_komi (sgf_tree.properties ().get_komi ());

      exec_embedded_gtp_rec (sgf_tree.game_node (), response);

      base_board.load (&save_board);

      return gtp_success;
    }

    // ---------------------------------------------------------------------
    fatal_error ("this should not happen!: error number = 0x994827");
    return gtp_panic;
  }

  
  void exec_embedded_gtp_rec (sgf_node_t* current_node, ostream& response) {
    // update position
    list <vertex_t> vertex_list;
    vertex_list = current_node->properties.get_vertices_to_play (color_t::empty ());
    if (vertex_list.empty () == false) {
      response << "sgf.gtp.exec: AE property in SGF not implemented" << endl;
      return;
    }

    player_for_each (pl) {
      vertex_list = current_node->properties.get_vertices_to_play (pl);
      for_each (vi, vertex_list) {
        if (base_board.try_play (pl, *vi) == false) {
          response << "sgf.gtp.exec: bad move in SGF: " << pl << " " << *vi << endl;
          return;
        }
      }
    }

    // save board
    board_t node_board;
    node_board.load (&base_board);

    // process gtp
    istringstream embedded_gtp (current_node->properties.get_embedded_gtp ());
    ostringstream embedded_response;
    gtp.run_loop (embedded_gtp, embedded_response, true); // TODO make gtp filter out double newlines
    current_node->properties.set_comment (embedded_response.str ());

    // recursove call
    for_each (child, current_node->children) {
      base_board.load (&node_board);
      exec_embedded_gtp_rec (&(*child), response); // LOL itearator is imitationg a pointer but we need a true pointer
    }
    
  }
  
};
