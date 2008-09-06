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


class GtpResult {
public:
  GtpResult () : status_ (status_success) { }

  static GtpResult success (string response = "") { return GtpResult (status_success, response); }
  static GtpResult failure (string response = "") { return GtpResult (status_failure, response); }
  static GtpResult syntax_error () { return GtpResult (status_failure, "syntax error"); }
  static GtpResult quit () { return GtpResult (status_quit); }


  string status_marker () {
    switch (status_) {
    case status_success: return "=";
    case status_failure: return "?";
    case status_quit:    return "=";
    default: assert (false); exit(1);
    }
  }

  bool quit_loop () {
    return status_ == status_quit;
  }

  string response () { return response_; }

private:

  enum GtpStatus {
    status_success,
    status_failure,
    status_quit
  };

  GtpResult (GtpStatus status, string response="") : status_ (status), response_ (response) { }

  GtpStatus status_;
  string response_;
};


class GtpEngine {
public:
  virtual GtpResult exec_command (string command_name, istream& params) = 0;
};


class Gtp : public GtpEngine {
public:
  Gtp () { 
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

  
  void add_gtp_command (GtpEngine* engine, string name) {
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
  
  void add_gogui_command (GtpEngine* engine, string type, string name, string params) {
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
    int cmd_num;

    while (true) {
      string line;
      if (!getline (in, line)) break;
      if (echo_commands) out << line << endl;

      line = preprocess (line);

      istringstream line_stream (line);
      string cmd_name;
      if (!(line_stream >> cmd_num )) { cmd_num = -1; line_stream.clear (); }
      if (!(line_stream >> cmd_name)) continue; // empty line - continue

      if (engine_of_cmd_name.find (cmd_name) == engine_of_cmd_name.end ()) {
        out << "? unknown command: \"" << cmd_name << "\"" << endl << endl;
        continue;
      }

      GtpEngine* engine = (*(engine_of_cmd_name.find (cmd_name))).second;
      GtpResult result = engine->exec_command (cmd_name, line_stream);
      string response_str = result.response ();

      response_str = remove_empty_lines (response_str);
      
      // make sure there is no \n or`whitespace on the end of string
      while (isspace ( *(response_str.end ()-1) )) {
        response_str.resize (response_str.size () - 1);
      }

      //  print respult
      out << result.status_marker ();
      if (cmd_num >= 0) out << cmd_num;
      out << " " << response_str << endl << endl;

      if (result.quit_loop ()) break;
    }
  }

public: // basic GTP commands

  virtual GtpResult exec_command (string command, istream& params) {

    if (is_static_command (command)) {
      return GtpResult::success (command_to_response.find (command)->second);
    }

    if (command == "help" || command == "list_commands") {
      string response;
      for_each (cmd_it, engine_of_cmd_name) { // TODO iterate over map instead of separate vector
        response += (*cmd_it).first + "\n";
      }
      return GtpResult::success (response);
    }

    if (command == "known_command") {
      string known_cmd;
      if (!(params >> known_cmd)) return GtpResult::syntax_error ();

      if (engine_of_cmd_name.find (known_cmd) == engine_of_cmd_name.end ()) 
        return GtpResult::success ("false"); 
      else 
        return GtpResult::success ("true"); 
    }

    if (command == "quit") 
      return GtpResult::quit ();

    if (command == "echo") {
      string response;
      while (params.peek () == ' ')  // eat all spaces in front
        unused (params.get ());

      getline (params, response);
      return GtpResult::success (response);
    }

    if (command == "run_gtp_file") {
      // TODO error checking
      string file_name;
      params >> file_name;
      ifstream filein (file_name.data ());
      ostringstream response;
      run_loop (filein, response);
      return GtpResult::success (response.str ());
    }

    fatal_error ("wrong command in Gtp::exec_command");
    assert (false);
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

private:
  map <string, GtpEngine*> engine_of_cmd_name;
  map <string, string>     command_to_response;
};
