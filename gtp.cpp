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

// ----------------------------------------------------------------------
class GtpResult {
public:
  GtpResult () : status_ (status_success) { }
  static GtpResult success (string response = "") { return GtpResult (status_success, response); }
  static GtpResult failure (string response = "") { return GtpResult (status_failure, response); }
  static GtpResult syntax_error () { return GtpResult (status_failure, "syntax error"); }
  static GtpResult quit () { return GtpResult (status_quit); }

  bool quit_loop () { return status_ == status_quit; }
  string to_string () { return status_marker () + " " + response_ + "\n\n"; }

private:
  enum Status {
    status_success,
    status_failure,
    status_quit
  };

  string status_marker () {
    switch (status_) {
    case status_success: return "=";
    case status_failure: return "?";
    case status_quit:    return "=";
    default: assert (false); exit(1);
    }
  }

  GtpResult (Status status, string response="") : status_ (status), response_ (response) {
    remove_empty_lines         (&response_);
    remove_trailing_whitespace (&response_);
  }

  Status status_;
  string response_;
};

// ----------------------------------------------------------------------
class GtpCommand {
public:
  virtual GtpResult exec_command (string command_name, istream& params) = 0;
};


// ----------------------------------------------------------------------
class GoguiParam {
public:
  static GoguiParam String (string name, string* ptr) {
    GoguiParam ret;
    ret.type = type_string;
    ret.string_param = ptr;
    ret.name = name;
    return ret;
  }

  static GoguiParam Float (string name, float* ptr) {
    GoguiParam ret;
    ret.type = type_float;
    ret.float_param = ptr;
    ret.name = name;
    return ret;
  }

  static GoguiParam Uint (string name, uint* ptr) {
    GoguiParam ret;
    ret.type = type_uint;
    ret.uint_param = ptr;
    ret.name = name;
    return ret;
  }

  static GoguiParam Bool (string name, bool* ptr) {
    GoguiParam ret;
    ret.type = type_bool;
    ret.bool_param = ptr;
    ret.name = name;
    return ret;
  }

  string gogui_string () {
    string ret = type_to_string () + " " + name + " " + value_to_string (); 
    return ret;
  }

  string type_to_string () {
    switch (type) {
    case type_string:  return "[string]";
    case type_float:   return "[string]";
    case type_uint:    return "[string]";
    case type_bool:    return "[bool]";
    }
    assert (false);
  }

  string value_to_string () {
    switch (type) {
    case type_string:  return *string_param;
    case type_float:   return to_string(*float_param);
    case type_uint:    return to_string(*uint_param);
    case type_bool:    return to_string(*bool_param);
    }
    assert (false);
  }

  bool set_value(istream& in) {
    switch (type) {
    case type_string:  return !!(in >> *string_param);
    case type_float:   return !!(in >> *float_param);
    case type_uint:    return !!(in >> *uint_param);
    case type_bool:    return !!(in >> *bool_param);
    }
    assert (false);
  }

  // ----------
  string name; 

  // emulation of sum type
  enum Type {
    type_string,
    type_float,
    type_uint,
    type_bool,
  } type;
  
  union {
    string* string_param;
    float*  float_param;
    uint*   uint_param;
    bool*   bool_param;
  };
};

// ----------------------------------------------------------------------
class Gtp : public GtpCommand {
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
    return command_of_name.find (name) != command_of_name.end ();
  }

  bool is_static_command (string name) {
    return command_to_response.find (name) != command_to_response.end ();
  }

  bool is_gogui_param_command (string name) {
    return command_to_gogui_params.find (name) != command_to_gogui_params.end ();
  }

  void add_gtp_command (GtpCommand* command, string name) {
    assert(!is_command(name));
    command_of_name [name] = command;
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
  
  void add_gogui_command (GtpCommand* command, string type, string name,
                          string params, bool extend_only_if_new = false) {
    if (extend_only_if_new && is_command (name)) return;
    if (!is_command(name)) add_gtp_command (command, name);
    string ext = type + "/" + name + " " + params + "/" + name + " " + params + "\n";
    extend_static_command ("gogui_analyze_commands", ext);
  }

  void add_gogui_param_string (string cmd_name, string param_name, string* ptr) {
    add_gogui_command (this, "param", cmd_name, "", true);
    command_to_gogui_params[cmd_name].push_back (GoguiParam::String(param_name, ptr));
  }

  void add_gogui_param_float (string cmd_name, string param_name, float* ptr) {
    add_gogui_command (this, "param", cmd_name, "", true);
    command_to_gogui_params[cmd_name].push_back (GoguiParam::Float(param_name, ptr));
  }

  void add_gogui_param_uint (string cmd_name, string param_name, uint* ptr) {
    add_gogui_command (this, "param", cmd_name, "", true);
    command_to_gogui_params[cmd_name].push_back (GoguiParam::Uint(param_name, ptr));
  }

  void add_gogui_param_bool (string cmd_name, string param_name, bool* ptr) {
    add_gogui_command (this, "param", cmd_name, "", true);
    command_to_gogui_params[cmd_name].push_back (GoguiParam::Bool(param_name, ptr));
  }

public:
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
    while (true) {
      string line;
      if (!getline (in, line)) break;
      if (echo_commands) out << line << endl;
      preprocess (&line);

      istringstream line_stream (line);
      string cmd_name;
      if (!(line_stream >> cmd_name)) continue; // empty line - continue

      if (!is_command (cmd_name)) {
        out << GtpResult::failure("unknown command: \"" + cmd_name + "\"").to_string ();
        continue;
      }

      GtpCommand* command = (*(command_of_name.find (cmd_name))).second;
      GtpResult result = command->exec_command (cmd_name, line_stream);
      out << result.to_string ();

      if (result.quit_loop ()) break;
    }
  }

public: // basic GTP commands

  virtual GtpResult exec_command (string command, istream& params) {

    if (is_static_command (command)) {
      return GtpResult::success (command_to_response.find (command)->second);
    }
    
    if (is_gogui_param_command (command)) {
      let (gogui_params, command_to_gogui_params [command]);
      string param_name;
      if (!(params >> param_name)) {
        // print values of all params
        string ret;
        for_each (param_it, gogui_params) {
          ret += param_it->gogui_string () + "\n";
        }
        return GtpResult::success (ret);
      } else {
        // print value of a param
        for_each (param_it, gogui_params) {
          if (param_it->name == param_name) {
            // param found
            return
              param_it->set_value (params) 
              ? GtpResult::success ()
              : GtpResult::failure ();
          }
        }
        return GtpResult::failure ();
      }
    }

    if (command == "help" || command == "list_commands") {
      string response;
      for_each (cmd_it, command_of_name) { // TODO iterate over map instead of separate vector
        response += (*cmd_it).first + "\n";
      }
      return GtpResult::success (response);
    }

    if (command == "known_command") {
      string known_cmd;
      if (!(params >> known_cmd)) return GtpResult::syntax_error ();
      return GtpResult::success (is_command(known_cmd) ? "true" : "false");
    }

    if (command == "quit") {
      return GtpResult::quit ();
    }

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
  void preprocess (string* s) {
    ostringstream ret;
    for_each (cp, *s) {
      if (*cp == 9) ret << '\32'; 
      else if (*cp > 0 && *cp <= 9) continue; 
      else if (*cp >= 11 && *cp <= 31) continue; 
      else if (*cp == 127) continue; 
      else if (*cp == '#') break;  // remove comments
      else ret << *cp;
    }
    *s = ret.str ();
  }

private:

  map <string, GtpCommand*>          command_of_name;
  map <string, string>               command_to_response;
  map <string, vector<GoguiParam> >  command_to_gogui_params;
};

