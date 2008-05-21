/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                           *
 *  This file is part of Library of Effective GO routines - EGO library      *
 *                                                                           *
 *  Copyright 2006, 2007, 2008 Lukasz Lew                                    *
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


class sgf_node_properties_t {
public:
  map <string, list <string> > properties;

  void add (string name, string value) {
    // TODO check existance
    properties [name] . push_back (value);
  }
  
  string to_sgf_string () {
    ostringstream out;
    for_each (prop, properties) {
      out << prop->first; // prop_name
      for_each (prop_value, prop->second) {
        out << "[" << *prop_value << "]";
      }
    }
    return out.str ();
  }

};



// class sgf_node_t


class sgf_node_t {
public:
  list <sgf_node_t>      children;
  sgf_node_properties_t  properties;


  sgf_node_t* add_child () {
    children.push_back (sgf_node_t ());
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


// class sgf_tree_t


class sgf_tree_t {
public:
  sgf_node_t* root;

public:

  sgf_tree_t () : root (new sgf_node_t) { }

  
  ~sgf_tree_t () {
    delete root;
  }


  bool parse_property_value (istream& in, string& prop_value) {
    char c;
    while (in >> c) {
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
    sgf_node_t* new_root = new sgf_node_t;
    #define return_fail { delete new_root; return false; }

    sgf_node_t*         current = new_root;
    stack <sgf_node_t*> cb_stack; // come-back-here-later-nodes

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

};


