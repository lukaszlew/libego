#include "boost/foreach.hpp"

#include "gtp_gogui.hpp"
#include <cmath>

namespace Gtp {

GoguiGfx::GoguiGfx () {
}


void GoguiGfx::SetInfluence (const string& vertex, float influence) {
  if (influence != 0.0 && ! std::isnan (influence)) {
    ostringstream s;
    s << influence;
    gfx_output ["INFLUENCE"] += vertex + " " + s.str() + " ";
  }
}

void GoguiGfx::SetLabel (const string& vertex, const string& label) {
  if (label != "") {
    gfx_output ["LABEL"] += vertex + " \"" + label + "\" ";
  }
}

void GoguiGfx::SetSymbol (const string& vertex, GoguiGfx::Symbol s) {
  string key;
  switch (s) {
  case circle:   key = "CIRCLE"; break;
  case triangle: key = "TRIANGLE"; break;
  case square:   key = "SQUARE"; break;
  default: assert (false);
  }
  gfx_output [key] += vertex + " ";
}

void GoguiGfx::AddVariationMove (const string& player_and_vertex) {
  gfx_output ["VAR"] += player_and_vertex + " ";
}

void GoguiGfx::SetStatusBar (const string& status) {
  gfx_output ["STATUS"] += status + " ";
}

void GoguiGfx::Report (Io& io)  {
  typedef pair <const string, string> P;
  BOOST_FOREACH (P& p, gfx_output) {
    io.out << p.first << " " << p.second << endl;
  }
}

// -----------------------------------------------------------------------------

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
    BOOST_FOREACH(v, vars) {
      io.out << "[string] " << v.first << " ";
      v.second (io);
      io.out << endl;
    }
  } else {
    string var_name = io.Read<string> ();
    if (vars.find (var_name) == vars.end ()) {
      io.SetError ("unknown variable: \"" + var_name + "\"");
      return;
    }
    vars[var_name] (io);
  }
}

void ReplWithGogui::CAnalyze (Io& io) {
  io.out << analyze_list.str ();
}

} // namespace Gtp
