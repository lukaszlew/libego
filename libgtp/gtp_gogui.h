#ifndef _GTP_GOGUI_H_
#define _GTP_GOGUI_H_

#include <string>
#include <map>
#include <boost/bind.hpp>

#include "gtp.h"

namespace Gtp {

using namespace std;

// -----------------------------------------------------------------------------

class GoguiGfx {
public:
  GoguiGfx();

  enum Symbol { circle, triangle, square };

  void SetInfluence (const string& vertex, float influence);
  void SetLabel     (const string& vertex, const string& label);
  void SetSymbol    (const string& vertex, Symbol s);

  void AddVariationMove (const string& player_and_vertex);

  void SetStatusBar (const string& status);

  void Report (Io& io);

private:
  map <string, string> gfx_output;
};

// -----------------------------------------------------------------------------

class ReplWithGogui : public Repl {
public:
  ReplWithGogui ();

  void RegisterGfx (const string& name, const string& params, Callback command);

  template <class T>
  void RegisterGfx (const string& name,
                    const string& params,
                    T* object,
                    void(T::*member)(Io&));

  template <typename T>
  void RegisterParam (const string& cmd_name, const string& param_name, T* param);

private:
  void CAnalyze (Io&);
  void CParam (const string& cmd_name, Io& io);

private:
  stringstream analyze_list;
  map <string, map <string, Callback> > params; // params [cmd_name] [param_name]
};

// -----------------------------------------------------------------------------
// internal

template <class T>
void ReplWithGogui::RegisterGfx (const string& name,
                                 const string& params,
                                 T* object,
                                 void(T::*member)(Io&))
{
  RegisterGfx (name, params, boost::bind(member, object, _1));
}

template <typename T>
void ReplWithGogui::RegisterParam (const string& cmd_name,
                                   const string& param_name,
                                   T* param)
{
  params [cmd_name] [param_name] = GetSetCallback (param);
  if (IsCommand (cmd_name)) return;
  analyze_list << "param/" << cmd_name << "/" << cmd_name << endl; // NOTE: factor out
  Register (cmd_name, boost::bind (&ReplWithGogui::CParam, this, cmd_name, _1));
}

} // namespace Gtp

#endif
