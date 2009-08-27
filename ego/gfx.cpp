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
