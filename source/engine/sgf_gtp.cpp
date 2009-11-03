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


#include "sgf_gtp.hpp"

#include "testing.hpp"

// class gtp_sgf

SgfGtp::SgfGtp (Gtp::Repl& _gtp, SgfTree& _sgf_tree, FullBoard& _base_board) : 
  sgf_tree (_sgf_tree), gtp (_gtp), base_board (_base_board)
{
  gtp.Register ("sgf.load",     this, &SgfGtp::CLoad);
  gtp.Register ("sgf.save",     this, &SgfGtp::CSave);
  gtp.Register ("sgf.gtp.exec", this, &SgfGtp::CGtpExec);
}


void SgfGtp::CLoad (Gtp::Io& io) {
  string file_name = io.Read<string> ();
  io.CheckEmpty();

  if (!sgf_tree.load_from_file(file_name)) {
    io.SetError ("file not found or invalid SGF: " + file_name);
    return;
  }
}

void SgfGtp::CSave (Gtp::Io& io) {
  string file_name = io.Read<string> ();
  io.CheckEmpty();
  if (!sgf_tree.save_to_file(file_name)) {
    io.SetError ("file cound not be created: " + file_name);
    return;
  }
}

void SgfGtp::CGtpExec (Gtp::Io& io) {
  io.CheckEmpty();
  if (!sgf_tree.is_loaded ()) {
    io.SetError ("SGF file not loaded");
    return;
  }

  if (sgf_tree.properties ().get_board_size () != board_size) {
    io.SetError ("invalid board size");
    return;
  }

  FullBoard save_board;
  save_board.Load (&base_board);
      
  base_board.clear ();
  base_board.set_komi (sgf_tree.properties ().get_komi ());
  ostringstream response;
  exec_embedded_gtp_rec (sgf_tree.game_node (), response);

  base_board.Load (&save_board);

  io.out << response.str ();
}

// It goes through a whole sgf and executes GTP commends embedded in SGF comments.
// if you put "@gtp" in comment, everything below is an embedded gtp command.

void SgfGtp::exec_embedded_gtp_rec (SgfNode* current_node, ostream& response) {
  // update position
  list <Vertex> vertex_list;
  vertex_list = current_node->properties.get_vertices_to_play (Color::Empty ());
  if (vertex_list.empty () == false) {
    response << "sgf.gtp.exec: AE property in SGF not implemented" << endl;
    return;
  }

  player_for_each (pl) {
    vertex_list = current_node->properties.get_vertices_to_play (pl);
    for (list<Vertex>::iterator vi = vertex_list.begin();
         vi != vertex_list.end();
         vi++) {
      if (base_board.try_play (pl, *vi) == false) {
        response << "sgf.gtp.exec: bad move in SGF: " << pl << " " << *vi << endl;
        return;
      }
    }
  }

  
  // process gtp
  istringstream embedded_gtp (current_node->properties.get_embedded_gtp ());
  ostringstream embedded_response;
  gtp.Run (embedded_gtp, embedded_response); // TODO add command logging
  // TODO make gtp filter out double newlines
  current_node->properties.set_comment (embedded_response.str ());

  // save board
  FullBoard node_board;
  node_board.Load (&base_board);

  // recursove call
  for (list<SgfNode>::iterator child = current_node->children.begin();
       child != current_node->children.end();
       child++) {
    base_board.Load (&node_board);
    exec_embedded_gtp_rec (&(*child), response);
  }
    
}
