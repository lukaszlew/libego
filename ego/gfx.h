#ifndef _GFX_H_
#define _GFX_H_

#include <vector>
#include <map>
#include <string>

#include "utils.h"
#include "vertex.h"
#include "move.h"

using namespace std;

// ----------------------------------------------------------------------

class Gfx {
public:
  Gfx();
  // gfx functions
  void set_influence(Vertex v, float val);
  void set_label(Vertex v, const string& label);

  enum GfxSymbol { circle, triangle, square };
  void add_symbol(Vertex v, GfxSymbol s);

  void add_var_move(Move m);

  void set_status_text(const string& status);

  string to_string ();

private:
  // gfx
  FastMap <Vertex, float>  influence_;
  FastMap <Vertex, string> label_;
  vector <Move> var_;
  vector <Vertex> circle_;
  vector <Vertex> triangle_;
  vector <Vertex> square_;
  string status_text_;
};

#endif
