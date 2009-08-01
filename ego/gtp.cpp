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

#include <fstream>
#include <string>

#include "gtp.h"
#include "testing.h"

// ----------------------------------------------------------------------

GtpResult GtpResult::success (string response) {
  return GtpResult (status_success, response);
}

GtpResult GtpResult::failure (string response) {
  return GtpResult (status_failure, response);
}

GtpResult GtpResult::syntax_error () {
  return GtpResult (status_failure, "syntax error");
}

GtpResult GtpResult::quit () {
  return GtpResult (status_quit);
}

bool GtpResult::quit_loop () {
  return status_ == status_quit;
}

string GtpResult::to_string () {
  // sanitize return string
  string res = response_;
  remove_empty_lines         (&res);
  remove_trailing_whitespace (&res);
  return status_marker () + " " + res + "\n\n";
}

string GtpResult::status_marker () {
  switch (status_) {
  case status_success: return "=";
  case status_failure: return "?";
  case status_quit:    return "=";
  default: assert (false);
  }
}


GtpResult::GtpResult (Status status, string response)
  : status_ (status), response_ (response)
{
}

// -----------------------------------------------------------------------------

void Gfx::set_influence(Vertex v, float val) {
  influence_[v] = val;
}

void Gfx::set_label(Vertex v, const string& label) {
  label_[v] = label;
}

void Gfx::add_symbol(Vertex v, GfxSymbol s) {
  switch(s) {
  case circle:   circle_.push_back(v); return;
  case triangle: triangle_.push_back(v); return;
  case square:   square_.push_back(v); return;
  default: assert(false);
  }
}

void Gfx::add_var_move(Move m) {
  var_.push_back(m);
}

void Gfx::set_status_text(const string& status) {
  status_text_ = status;
}

Gfx::Gfx () {
  influence_.SetAll(0.0);
  label_.SetAll("");
}


string Gfx::to_string () {
  stringstream influence, label, var, circle, triangle, square, status;
  bool influence_b = false;
  bool label_b = false;
  bool var_b = false;
  bool circle_b = false;
  bool triangle_b = false;
  bool square_b = false;
  bool status_b = false;

  influence << "INFLUENCE";
  label     << "LABEL";
  var       << "VAR";
  circle    << "CIRCLE";
  triangle  << "TRIANGLE";
  square    << "SQUARE";
  status    << "TEXT";

  vertex_for_each_all(v) {
    if (!v.is_on_board()) continue;
    if (influence_[v] != 0.0) {
      influence << " " << v << " " << influence_[v];
      influence_b = true;
    }
    if (label_[v] != "") {
      label << " " << v << " \"" << label_[v] << "\"";
      label_b = true;
    }
  }
  for(vector<Move>::iterator ii = var_.begin();
      ii != var_.end(); ii++) {
    var << " " << ii->to_string();
    var_b = true;
  }
  for(vector<Vertex>::iterator ii = circle_.begin();
      ii != circle_.end(); ii++) {
    circle << " " << *ii;
    circle_b = true;
  }
  for(vector<Vertex>::iterator ii = triangle_.begin();
      ii != triangle_.end(); ii++) {
    triangle << " " << *ii;
    triangle_b = true;
  }
  for(vector<Vertex>::iterator ii = square_.begin();
      ii != square_.end(); ii++) {
    square << " " << *ii;
    square_b = true;
  }

  status << " \"" << status_text_<< "\"";
  status_b = status_text_ != "";

  influence << endl;
  label     << endl;
  var       << endl;
  circle    << endl;
  triangle  << endl;
  square    << endl;
  status    << endl;

  return
    (influence_b ? influence.str() : "") +
    (label_b     ? label.str()     : "") +
    (var_b       ? var.str()       : "") +
    (circle_b    ? circle.str()    : "") +
    (triangle_b  ? triangle.str()  : "") +
    (square_b    ? square.str()    : "") +
    (status_b    ? status.str()    : "");
}

// ----------------------------------------------------------------------

GoguiParam GoguiParam::String (string name, string* ptr) {
  GoguiParam ret;
  ret.type = type_string;
  ret.string_param = ptr;
  ret.name = name;
  return ret;
}

GoguiParam GoguiParam::Float (string name, float* ptr) {
  GoguiParam ret;
  ret.type = type_float;
  ret.float_param = ptr;
  ret.name = name;
  return ret;
}

GoguiParam GoguiParam::Uint (string name, uint* ptr) {
  GoguiParam ret;
  ret.type = type_uint;
  ret.uint_param = ptr;
  ret.name = name;
  return ret;
}

GoguiParam GoguiParam::Bool (string name, bool* ptr) {
  GoguiParam ret;
  ret.type = type_bool;
  ret.bool_param = ptr;
  ret.name = name;
  return ret;
}

string GoguiParam::gogui_string () {
  string ret = type_to_string () + " " + name + " " + value_to_string ();
  return ret;
}

string GoguiParam::type_to_string () {
  switch (type) {
  case type_string:  return "[string]";
  case type_float:   return "[string]";
  case type_uint:    return "[string]";
  case type_bool:    return "[bool]";
  }
  assert (false);
}

string GoguiParam::value_to_string () {
  switch (type) {
  case type_string:  return *string_param;
  case type_float:   return SS() << *float_param;
  case type_uint:    return SS() << *uint_param;
  case type_bool:    return SS() << *bool_param;
  }
  assert (false);
}

bool GoguiParam::set_value(istream& in) {
  switch (type) {
  case type_string:  return !!(in >> *string_param);
  case type_float:   return !!(in >> *float_param);
  case type_uint:    return !!(in >> *uint_param);
  case type_bool:    return !!(in >> *bool_param);
  }
  assert (false);
}


// ----------------------------------------------------------------------

Gtp::Gtp () {
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

bool Gtp::is_command (string name) {
  return command_of_name.find (name) != command_of_name.end ();
}

bool Gtp::is_static_command (string name) {
  return command_to_response.find (name) != command_to_response.end ();
}

bool Gtp::is_gogui_param_command (string name) {
  return command_to_gogui_params.find (name) != command_to_gogui_params.end ();
}

void Gtp::add_gtp_command (GtpCommand* command, string name) {
  assert(!is_command(name));
  command_of_name [name] = command;
}

void Gtp::add_static_command (string name, string response) {
  command_to_response [name] = response;
  add_gtp_command (this, name);
}

void Gtp::extend_static_command (string name, string response_ext) {
  if (!is_command(name)) add_gtp_command (this, name);
  command_to_response [name] = command_to_response [name] + response_ext;
  //cout << "P " << name << " -> " << endl <<      command_to_response [name] << endl;
}

void Gtp::add_gogui_command (GtpCommand* command, string type, string name,
                             string params, bool extend_only_if_new) {
  if (extend_only_if_new && is_command (name)) return;
  if (!is_command(name)) add_gtp_command (command, name);
  string ext = type + "/" + name + " " + params + "/" + name + " " + params + "\n";
  extend_static_command ("gogui_analyze_commands", ext);
}

void Gtp::add_gogui_param_string (string cmd_name, string param_name, string* ptr) {
  add_gogui_command (this, "param", cmd_name, "", true);
  command_to_gogui_params[cmd_name].push_back (GoguiParam::String(param_name, ptr));
}

void Gtp::add_gogui_param_float (string cmd_name, string param_name, float* ptr) {
  add_gogui_command (this, "param", cmd_name, "", true);
  command_to_gogui_params[cmd_name].push_back (GoguiParam::Float(param_name, ptr));
}

void Gtp::add_gogui_param_uint (string cmd_name, string param_name, uint* ptr) {
  add_gogui_command (this, "param", cmd_name, "", true);
  command_to_gogui_params[cmd_name].push_back (GoguiParam::Uint(param_name, ptr));
}

void Gtp::add_gogui_param_bool (string cmd_name, string param_name, bool* ptr) {
  add_gogui_command (this, "param", cmd_name, "", true);
  command_to_gogui_params[cmd_name].push_back (GoguiParam::Bool(param_name, ptr));
}

bool Gtp::run_file (string file_name, ostream& out) {
  ifstream in (file_name.data ());
  if (in) {
    run_loop (in, out);
    in.close ();
    return true;
  } else {
    return false;
  }
}

void Gtp::run_loop (istream& in, ostream& out, bool echo_commands) {
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

GtpResult Gtp::exec_command (const string& command, istream& params) {

  if (is_static_command (command)) {
    return GtpResult::success (command_to_response.find (command)->second);
  }

  if (is_gogui_param_command (command)) {
    vector<GoguiParam>& gogui_params = command_to_gogui_params [command];
    string param_name;
    if (!(params >> param_name)) {
      // print values of all params
      string ret;
      for(vector<GoguiParam>::iterator param_it = gogui_params.begin();
          param_it != gogui_params.end();
          param_it++) {
        ret += param_it->gogui_string () + "\n";
      }
      return GtpResult::success (ret);
    } else {
      // print value of a param
      for(vector<GoguiParam>::iterator param_it = gogui_params.begin();
          param_it != gogui_params.end();
          param_it++) {
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
    for (map <string, GtpCommand*>::iterator cmd_it = command_of_name.begin();
         cmd_it != command_of_name.end();
         cmd_it++) {
      response += cmd_it->first + "\n";
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

  //fatal_error ("wrong command in Gtp::exec_command");
  assert (false);
}

void Gtp::preprocess (string* s) {
  ostringstream ret;
  for(string::iterator cp = s->begin(); cp != s->end(); cp++) {
    if (*cp == 9) ret << '\32';
    else if (*cp > 0 && *cp <= 9) continue;
    else if (*cp >= 11 && *cp <= 31) continue;
    else if (*cp == 127) continue;
    else if (*cp == '#') break;  // remove comments
    else ret << *cp;
  }
  *s = ret.str ();
}
