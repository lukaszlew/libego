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


enum gtp_status_t {
  gtp_success,
  gtp_failure,
  gtp_syntax_error,
  gtp_panic,
  gtp_quit
};

class gtp_engine_t {
public:
  gtp_engine_t () {}
  virtual ~gtp_engine_t () {}
  virtual gtp_status_t exec_command (string command_name, istream& params, ostream& response) = 0;
};


class gtp_t : public gtp_engine_t {
public:
  gtp_t () { 
    add_gtp_command (this, "help");
    add_gtp_command (this, "list_commands");
    add_gtp_command (this, "known_command");
    add_gtp_command (this, "quit");
    add_gtp_command (this, "echo");
    add_gtp_command (this, "run_gtp_file");

    add_static_command ("protocol_version", "2");
    add_static_command ("name", "libego");
    add_static_command ("gogui_analyze_commands", ""); // to be extended
  }

  bool is_command (string name) {
    return engine_of_cmd_name.find (name) != engine_of_cmd_name.end ();
  }

  bool is_static_command (string name) {
    return command_to_response.find (name) != command_to_response.end ();
  }

  
  void add_gtp_command (gtp_engine_t* engine, string name) {
    assert(!is_command(name));
    engine_of_cmd_name [name] = engine;
  }

  void add_static_command (string name, string response) {
    command_to_response [name] = response;
    add_gtp_command (this, name);
  }

  void extend_static_command (string name, string response_ext) {
    if (!is_command(name)) add_gtp_command (this, name);
    command_to_response [name] = command_to_response [name] + response_ext;
    //cout << "P " << name << " -> " << endl <<      command_to_response [name] << endl;
  }
  
  void add_gogui_command (gtp_engine_t* engine, string type, string name, string params) {
    if (!is_command(name)) add_gtp_command (engine, name);
    string ext = type + "/" + name + " " + params + "/" + name + " " + params + "\n";
    extend_static_command ("gogui_analyze_commands", ext);
  }

  bool run_file (string file_name, ostream& out = cout) {
    ifstream in (file_name.data ());
    if (in) {
      run_loop (in, out);
      in.close ();
      return true;
    } else {
      return false;
    }
  }


  void run_loop (istream& in = cin, ostream& out = cout, bool echo_commands = false) {
    string line;
    int cmd_num;
    string cmd_name;
    gtp_status_t status;

    while (true) {
      if (!getline (in, line)) break;
      if (echo_commands) out << line << endl;

      line = preprocess (line);

      istringstream line_stream (line);
      if (!(line_stream >> cmd_num )) { cmd_num = -1; line_stream.clear (); }
      if (!(line_stream >> cmd_name)) continue; // empty line - continue

      if (engine_of_cmd_name.find (cmd_name) == engine_of_cmd_name.end ()) {
        out << "? unknown command: \"" << cmd_name << "\"" << endl << endl;
        continue;
      }

      gtp_engine_t* engine = (*(engine_of_cmd_name.find (cmd_name))).second;

      ostringstream response;
      status = engine->exec_command (cmd_name, line_stream, response);
      string response_str = response.str ();

      response_str = remove_empty_lines (response_str);
      
      // make sure there is no \n or`whitespace on the end of string
      while (isspace ( *(response_str.end ()-1) )) {
        response_str.resize (response_str.size () - 1);
      }

      out << status_marker (status);

      if (cmd_num >= 0) 
        out << cmd_num;

      out << " ";

      if (status == gtp_syntax_error) 
        out << "syntax error" << endl;

      out << response_str << endl << endl;

      if (status == gtp_panic || 
          status == gtp_quit) 
        break;
      
    }
  }

public: // basic GTP commands

  virtual gtp_status_t exec_command (string command, istream& params, ostream& response) {

    if (is_static_command (command)) {
      response << command_to_response.find (command)->second;
      return gtp_success;
    }

    if (command == "help" || command == "list_commands") {
      for_each (cmd_it, engine_of_cmd_name) { // TODO iterate over map instead of separate vector
        response << (*cmd_it).first << endl;
      }
      return gtp_success;
    }

    if (command == "known_command") {
      string known_cmd;
      if (!(params >> known_cmd)) return gtp_syntax_error;

      if (engine_of_cmd_name.find (known_cmd) == engine_of_cmd_name.end ()) response << "false";
      else response << "true";

      return gtp_success; 
    }

    if (command == "quit") 
      return gtp_quit;

    if (command == "echo") {
      string buf;
      while (params.peek () == ' ')  // eat all spaces in front
        unused (params.get ());

      if (getline (params, buf))
        response << buf; // TODO this should by in STL
      return gtp_success;
    }

    if (command == "run_gtp_file") {
      // TODO error checking
      string file_name;
      params >> file_name;
      ifstream filein (file_name.data ());
      run_loop (filein, response);
      return gtp_success;
    }

    fatal_error ("wrong command in gtp_t::exec_command");
    return gtp_panic; // formality 
  }

private:
  string preprocess (string s) {
    ostringstream ret;
    for_each (cp, s) {
      if (*cp == 9) ret << '\32'; 
      else if (*cp > 0 && *cp <= 9) continue; 
      else if (*cp >= 11 && *cp <= 31) continue; 
      else if (*cp == 127) continue; 
      else if (*cp == '#') break;  // remove comments
      else ret << *cp;
    }
    return ret.str ();
  }

  string status_marker (gtp_status_t status) {
    switch (status) {
    case gtp_success: return "=";
    case gtp_failure: return "?";
    case gtp_syntax_error: return "?";
    case gtp_panic:   return "!";
    case gtp_quit:    return "=";
    default: assert (false); exit(1);
    }
  }

private:
  map <string, gtp_engine_t*> engine_of_cmd_name;
  map <string, string>        command_to_response;
};
