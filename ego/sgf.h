#ifndef _SGF_H_
#define _SGF_H_

#include <map>
#include <list>
#include <string>

#include "vertex.h"
#include "color.h"

using namespace std;

extern const string embedded_gtp_tag;;

class SgfNodeProperties {
public:
  map <string, list <string> > property_map;

  void add (string name, string value);
  string to_sgf_string ();
  string get_comment ();
  void set_comment (string comment);
  string get_embedded_gtp ();
  list <Vertex> get_vertices_to_play (Color color);
  list <Vertex> get_vertices_to_clear ();

  list <Vertex> get_vertices_to_play (Player pl);
  string get_single_property (string prop_name, string default_value = "");
  uint  get_board_size ();
  float get_komi ();
};


class SgfNode {
public:
  list <SgfNode>      children;
  SgfNodeProperties  properties;

  SgfNode* add_child ();
  string to_sgf_string (bool print_braces = true);
};


class SgfTree {
  SgfNode* root;

public:

  SgfTree ();
  ~SgfTree ();
  bool parse_property_value (istream& in, string& prop_value);

  // constructor
  bool parse_sgf (istream& in);
  string to_sgf_string ();
  bool save_to_file   (const string& file_name);
  bool load_from_file (const string& file_name);
  bool is_loaded ();
  SgfNode* game_node ();
  SgfNodeProperties& properties ();
};

#endif
