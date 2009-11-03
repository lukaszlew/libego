/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                           *
 *  This file is part of Library of Effective GO routines - EGO library      *
 *                                                                           *
 *  Copyright 2006 and onwards, Lukasz Lew                                   *
 *                                                                           *
 *  EGO library isfree software; you can redistribute it and/or modify       *
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

#include "sgf.hpp"

#include <stack>
#include <fstream>
#include <cstdlib>

using namespace std;

#include "testing.hpp"


const string embedded_gtp_tag = "@gtp";


void SgfNodeProperties::add (string name, string value) {
  // TODO check existance
  property_map [name] . push_back (value);
}
  

string SgfNodeProperties::to_sgf_string () {
  ostringstream out;
  for (map <string, list<string> >::iterator prop = property_map.begin(); 
       prop != property_map.end(); 
       prop++) {
    if (prop->second.size () > 0) {
      out << prop->first; // prop_name
      for (list<string>::iterator prop_value = prop->second.begin();
           prop_value != prop->second.end();
           prop_value++) {
        out << "[" << *prop_value << "]";
      }
    }
  }
  return out.str ();
}


string SgfNodeProperties::get_comment () { 
  ostringstream out;
  list <string> comments = property_map ["C"];
  for (list<string>::iterator comment = comments.begin();
       comment != comments.end();
       comment++) {
    out << *comment << endl;
  }
  return out.str ();
}

void SgfNodeProperties::set_comment (string comment) { 
  property_map ["C"].clear ();
  add ("C", comment);
}

string SgfNodeProperties::get_embedded_gtp () { 
  string comment = get_comment (); // TODO tolower 
  int gtppos = comment.find (embedded_gtp_tag);
  if (gtppos == -1) return "";
  gtppos += embedded_gtp_tag.size ();
  return comment.substr (gtppos); 
}

// color to allow add empty
list <Vertex> SgfNodeProperties::get_vertices_to_play (Color color) { 
  list <Vertex> ret;

#define add_moves(prop_name)                                          \
  {                                                                   \
    list <string> prop_values = property_map [prop_name];             \
    for (list<string>::iterator sgf_coord = prop_values.begin();      \
         sgf_coord != prop_values.end();                              \
         sgf_coord++) {                                               \
      ret.push_back (Vertex::of_sgf_coords (*sgf_coord));             \
    }                                                                 \
  }
      
  if (color == Color::Empty ()) {
    add_moves ("AE");
  } else if (color == Color::Black ()) {
    add_moves ("B");
    add_moves ("AB");
  } else if (color == Color::White ()) {
    add_moves ("W");
    add_moves ("AW");
  } else {
    assert(false);
  }

  return ret;

#undef add_moves
}

list <Vertex> SgfNodeProperties::get_vertices_to_clear () {
  return get_vertices_to_play (Color::Empty ());
}

list <Vertex> SgfNodeProperties::get_vertices_to_play (Player pl) { 
  return get_vertices_to_play (Color (pl));
}

string SgfNodeProperties::get_single_property (string prop_name,
                                               string default_value) {
  if (property_map [prop_name].size () == 0) return default_value;
  if (property_map [prop_name].size () > 1) {
    //cerr << "warning: multiple property value for: " << prop_name <<
    //endl;
    assert(false);
  } 
  return property_map [prop_name].front ();
}

uint  SgfNodeProperties::get_board_size () {
  return atoi (get_single_property ("SZ", "19").data ());
}

float SgfNodeProperties::get_komi () {
  return atof (get_single_property ("KM", "0.0").data ());
}



// class SgfNode

SgfNode* SgfNode::add_child () {
  children.push_back (SgfNode ());
  return &(children.back ());
}


string SgfNode::to_sgf_string (bool print_braces) {
  ostringstream out;
  if (print_braces) out << "(";

  out << endl << ";" << properties.to_sgf_string ();

  bool print_child_braces = children.size () > 1;
  for (list<SgfNode>::iterator child = children.begin();
       child != children.end();
       child++)
    out << child->to_sgf_string (print_child_braces);

  if (print_braces) out << ")";

  return out.str ();
}  


// class SgfTree


SgfTree::SgfTree () : root (new SgfNode) { }

  
SgfTree::~SgfTree () {
  delete root;
}


bool SgfTree::parse_property_value (istream& in, string& prop_value) {
  char c;
  while (in.get (c)) {
    switch (c) {
    case '\\':
      in >> c;
      if (c == ']')
        prop_value += ']'; // escaped ']'
      else {
        prop_value += '\\'; 
        prop_value += c;
      }
      break;
    case ']':
      return true;
    default:
      prop_value += c;
      break;
    }
  }
  return false;
}
  
// constructor
bool SgfTree::parse_sgf (istream& in) {
  SgfNode* new_root = new SgfNode;
#define return_fail { delete new_root; return false; }

  SgfNode*         current = new_root;
  stack <SgfNode*> cb_stack; // come-back-here-later-nodes

  string              buffer;
  string              prop_name;
  string              prop_value;
  bool                prop_name_already_used = true;

  char c;
  while (in >> c) {
    switch (c) {

    case '(': 
      cb_stack.push (current);
      break;

    case ')': // go back to open_bracket_node
      current = cb_stack.top ();
      if (cb_stack.empty ()) return_fail;
      cb_stack.pop ();
      break;

    case ';': // create new node and attach it to the current one
      current = current->add_child ();
      break;

      // parsing property
    case '[':
      // parsing the property value
      prop_value = "";
      if (parse_property_value (in, prop_value) == false) return_fail;
      current->properties.add (prop_name, prop_value); // we reuse last property, this may be nonlocal in nonvalid sgf files
      prop_name_already_used = true;
      break;

    case ' ':
    case '\n':
    case '\t':
    case '\r':
      break;

    default:
      // parsing property name
      if (prop_name_already_used) {
        prop_name = "";
        prop_name_already_used = false;
      }
      prop_name += c;
      break;
    }
  }
    
  if (cb_stack.empty () == false)
    return_fail;

  delete root;
  root = new_root;
  return true;
}


string SgfTree::to_sgf_string () {
  ostringstream out;
  for (list<SgfNode>::iterator child = root->children.begin();
       child != root->children.end();
       child++)
    out << child->to_sgf_string () << endl; 
  return out.str ();
}
  

bool SgfTree::is_loaded () { return root->children.size () > 0; }


SgfNode* SgfTree::game_node () {
  assert (is_loaded ());
  return & (root->children.front ());
}

SgfNodeProperties& SgfTree::properties () {
  return game_node ()->properties;
}

bool SgfTree::save_to_file (const string& file_name) {
  ofstream sgf_stream (file_name.data ());
  if (!sgf_stream) return false;
  sgf_stream << to_sgf_string () << endl;
  sgf_stream.close ();
  return true;
}

bool SgfTree::load_from_file (const string& file_name) {
  ifstream sgf_stream (file_name.data ());
  if (!sgf_stream)
    return false;
  return parse_sgf (sgf_stream);
}
