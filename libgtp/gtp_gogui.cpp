#include "boost/foreach.hpp"
#include "gtp_gogui.h"

#define FOREACH BOOST_FOREACH

namespace Gtp {

ReplWithGogui::ReplWithGogui () : Repl () {
  Register ("gogui_analyze_commands", this, &ReplWithGogui::CAnalyze);
}

void ReplWithGogui::RegisterGfx (const string& name,
                           const string& params,
                           Callback command)
{
  string full_name = params == "" ? name : name + " " + params;
  analyze_list
    << "gfx/" << full_name << "/" << full_name << endl;
  if (!IsCommand (name)) {
    Register (name, command);
  }
}

void ReplWithGogui::CParam (const string& cmd_name, Io& io) {
  map<string, Callback>& vars = params[cmd_name];
  if (io.IsEmpty ()) {
    // print all vars and their values
    pair<string, Callback> v;
    FOREACH(v, vars) {
      io.Out() << "[string] " << v.first << " ";
      v.second (io);
      io.Out() << endl;
    }
    return;
  }
  string var_name = io.Read<string> ();
  if (vars.find (var_name) == vars.end ()) {
    throw Error ("unknown variable: \"" + var_name + "\"");
  }
  vars[var_name] (io);
}

void ReplWithGogui::CAnalyze (Io& io) {
  io.Out () << analyze_list.str ();
}

} // namespace Gtp
