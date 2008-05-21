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
  sgf_tree_t& sgf_tree;

  gtp_sgf_t (sgf_tree_t& _sgf_tree) : sgf_tree (_sgf_tree) { }

  virtual vector <string> get_command_names () const {
    vector <string> commands;
    commands.push_back ("sgf.load");
    commands.push_back ("sgf.save");
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

    fatal_error ("this should not happen!: error number = 0x994827");
    return gtp_panic;
  }
};
