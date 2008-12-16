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


const string embedded_gtp_tag = "@gtp";


class SgfNodeProperties {
public:
  map <string, list <string> > property_map;


  void add (string name, string value) {
    // TODO check existance
    property_map [name] . push_back (value);
  }
  

  string to_sgf_string () {
    ostringstream out;
    for_each (prop, property_map) {
      if (prop->second.size () > 0) {
        out << prop->first; // prop_name
        for_each (prop_value, prop->second) {
          out << "[" << *prop_value << "]";
        }
      }
    }
    return out.str ();
  }


  string get_comment () { 
    ostringstream out;
    list <string> comments = property_map ["C"];
    for_each (comment, comments) {
      out << *comment << endl;
    }
    return out.str ();
  }

  void set_comment (string comment) { 
    property_map ["C"].clear ();
    add ("C", comment);
  }

  string get_embedded_gtp () { 
    string comment = get_comment (); // TODO tolower 
    int gtppos = comment.find (embedded_gtp_tag);
    if (gtppos == -1) return "";
    gtppos += embedded_gtp_tag.size ();
    return comment.substr (gtppos); 
  }

  // color to allow add empty
  list <Vertex> get_vertices_to_play (Color color) { 
    list <Vertex> ret;

      #define add_moves(prop_name)                              \
      {                                                         \
        list <string> prop_values = property_map [prop_name];   \
        for_each (sgf_coord, prop_values) {                     \
          ret.push_back (Vertex::of_sgf_coords (*sgf_coord)); \
        }                                                       \
      }
      
    if (color == Color::empty ()) {
      add_moves ("AE")
    } else if (color == Color::black ()) {
      add_moves ("B");
      add_moves ("AB");
    } else if (color == Color::white ()) {
      add_moves ("W");
      add_moves ("AW");
    } else {
      fatal_error ("get_moves: illegal color");
    }
    
    #undef add_moves

    return ret;
  }

  list <Vertex> get_vertices_to_clear () { return get_vertices_to_play (Color::empty ()); }

  list <Vertex> get_vertices_to_play (Player pl) { 
    return get_vertices_to_play (Color (pl));
  }

  string get_single_property (string prop_name, string default_value = "") {
    if (property_map [prop_name].size () == 0) return default_value;
    if (property_map [prop_name].size () > 1) {
      cerr << "warning: multiple property value for: " << prop_name << endl;
    } 
    return property_map [prop_name].front ();
  }

  uint  get_board_size () { return atoi (get_single_property ("SZ", "19").data ()); }
  float get_komi       () { return atof (get_single_property ("KM", "0.0").data ()); }

};



// class SgfNode

class SgfNode {
public:
  list <SgfNode>      children;
  SgfNodeProperties  properties;


  SgfNode* add_child () {
    children.push_back (SgfNode ());
    return &(children.back ());
  }


  string to_sgf_string (bool print_braces = true) {
    ostringstream out;
    if (print_braces) out << "(";

    out << endl << ";" << properties.to_sgf_string ();

    bool print_child_braces = children.size () > 1;
    for_each (child, children) 
      out << child->to_sgf_string (print_child_braces);

    if (print_braces) out << ")";

    return out.str ();
  }  
};


// class SgfTree


class SgfTree {
  SgfNode* root;

public:

  SgfTree () : root (new SgfNode) { }

  
  ~SgfTree () {
    delete root;
  }


  bool parse_property_value (istream& in, string& prop_value) {
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
  bool parse_sgf (istream& in) {
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


  string to_sgf_string () {
    ostringstream out;
    for_each (it, root->children) 
      out << it->to_sgf_string () << endl; 
    return out.str ();
  }
  

  bool is_loaded () { return root->children.size () > 0; }


  SgfNode* game_node () {
    assert (is_loaded ());
    return & (root->children.front ());
  }

  SgfNodeProperties& properties () {
    return game_node ()->properties;
  }

};


