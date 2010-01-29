const double NaN = std::numeric_limits<double>::quiet_NaN();

namespace M {

namespace Param {
  bool   update = true;

  double expand_at_n = 100.0;
  uint   expand_max_nulls = 3;

  double max_rave_n = 1000.0;
  double explore_coeff = 0.0;
  double boltzmann_constant = 0.0;
}

// -----------------------------------------------------------------------------

struct Stat {
  Stat () {
  }

  void Reset (bool maximize_) {
    n = 0.0;
    sum = 0.0;
    maximize = maximize_;
  }

  double Mean () {
    //CHECK (n > 0.0);
    return sum / (n+1);
  }

  double N () {
    return n;
  }

  double N (double max_n) {
    // same as: return 1.0 / (1.0 / n + 1.0 / max_n)
    return n * max_n / (n + max_n);
  }

  void Update (double x) {
    n += 1.0;
    sum += x;
  }

  string ToString () {
    char buf [100];
    sprintf (buf, "%+3.3f(%6.0f)", Mean(), N());
    return string (buf);
  }

private:
  double n;
  double sum;
  bool maximize;
};

// -----------------------------------------------------------------------------

// TODO back links for performance
// TODO optimistic priors na Expand

struct Node {

  Node () {
    null_child = NULL;
    children = NULL;
  }


  void Reset (Node* parent_, Move last_move_, bool add_null_child = true)
  {
    stat.Reset (last_move_.GetPlayer() == Player::Black());
    parent = parent_;
    last_move = last_move_;
    children = NULL;
    null_child = NULL;
    if (parent != NULL) {
      depth = parent->depth + 1;
      nulls_on_path = parent->nulls_on_path;
    } else {
      depth = 0;
      nulls_on_path = 0;
    }
    activate_count = 0;
    if (add_null_child) {
      // null move is a *sequence* of any player
      Move null_move = Move (last_move.GetPlayer().Other(), Vertex::Any());
      null_child = new Node();
      null_child->Reset (this, null_move, false);
      null_child->nulls_on_path += 1;
    }
  }


  ~Node () {
    if (children != NULL) {
      delete children;
      children = NULL;
    }
    if (null_child != NULL) {
      delete null_child;
      null_child = NULL;
    }
  }

  void TryExpand () {
    activate_count += 1;
    
    if (children != NULL ||
        activate_count < Param::expand_at_n ||
        nulls_on_path >= Param::expand_max_nulls) return;

    CHECK (children == NULL);
    children = new NatMap<Move, Node>;

    ForEachNat (Move, m) {
      Child (m) -> Reset (this, m);
    }
  }

  Node* Child (Move m) {
    CHECK (children != NULL);
    return &(*children) [m];
  }

  // PRINTING

  string ToString (bool full = false) {
    ostringstream out;

    out << " A: " << setw(7) << activate_count << " "
        << " S: " << stat.ToString() << " ";

    if (full) {
      vector <Move> path = Path();
      rep(ii, path.size()) {
        out << (
                path[ii].GetVertex() == Vertex::Any() ?
                "[****] " :
                "[" + path[ii].ToGtpString() + "] "
                );
      }
    } else {
      out << last_move.ToGtpString();
    }


    return out.str ();
  }


  static bool PrintCmp (Node* a, Node* b) {
    return a->PrintValue() > b->PrintValue();
  }

  double PrintValue () {
    return stat.N();
  }

  string RecToString (uint n) {
    ostringstream out;
    rep (ii, depth) out << "  "; // indent
    out << ToString() << endl;

    vector <Node*> nodes;

    if (null_child != NULL) // TODO sure?
      nodes.push_back (null_child);

    ForEachNat (Move, m) {
      if (children == NULL) continue;
      Node* c = Child (m);
      nodes.push_back (c);
    }

    sort(nodes.begin(), nodes.end(), PrintCmp);

    rep (ii, min (n, uint(nodes.size()))) {
      uint new_n = max(0u, n-ii);
      out << nodes[ii]->RecToString (new_n);
    }

    return out.str();
  }

  vector<Move> Path () {
    vector<Move> path;
    if (parent == NULL) return path;
    path = parent->Path();
    path.push_back (last_move);
    return path;
  }

  Node* parent;
  uint depth;
  uint nulls_on_path;
  Move last_move;
  uint activate_count;

  NatMap <Move, Node>* children;
  Node* null_child;
  Stat stat;
};

// -----------------------------------------------------------------------------

// IDEA moze dla kazdego ruchu ograniczyc liczbe active'ow

struct Model {

  Model (FullBoard& board) : board (board) {
    root = NULL;
    Reset ();
  }


  void Reset () {
    if (root != NULL) delete root;
    root = new Node ();
    root->Reset (NULL, board.GetBoard().LastMove());
    root->activate_count += Param::expand_at_n;
    sync_board_move_no = board.MoveHistory().size();
    // TODO add children to root by default including childrens to null
  }


  bool SyncWithBoard () {
    active.clear();
    AddActive (root);
    AddActive (root->null_child);

    const vector <Move>& history = board.MoveHistory();
    if (sync_board_move_no > history.size()) {
      cerr << "Warning: can't sync with board, too much undo." << endl;
      return false;
    }
    reps (ii, sync_board_move_no, history.size()) {
      NewMove (history[ii]);
    }
    return true;
  }


  void NewMove (Move m) {
    CHECK (active.size () > 0);
    old_active.swap (active); // old_active = active;
    active.clear();
    
    rep (ii, old_active.size()) {
      Node* old = old_active[ii];
      ASSERT (old != NULL);

      // stars don't get deleted
      if (old->null_child == NULL) {
        active.push_back (old);
      }

      if (old->children != NULL) {
        AddActive (old->Child(m));
        AddActive (old->Child(m)->null_child);
      }
    }
  }

  void AddActive (Node* n) {
    CHECK (n != NULL);
    active.push_back (n);
    n->TryExpand();
  }

  void Update (double result) {
    rep (ii, active.size()) {
      active[ii]->stat.Update (result);
    }
  }


//   void FillValues (NatMap<Vertex, double>& values) {
//     SyncWithBoard ();
//     Player pl = board.GetBoard().ActPlayer ();

//     //cerr << "Act node: " << act->ToString(true) << endl;

//     ForEachNat (Vertex, v) {
//       Move m = Move (pl, v);
//       Node* child = act->children [m];
//       if (child == NULL) {
//         values [v] = NaN;
//       } else {
//         values [v] = child->Value ();
//       }
//     }
//   }


  void RegisterInGtp (Gtp::ReplWithGogui& gtp) {
//     gtp.RegisterGfx ("model.values", "", this, &Model::GtpShowValues);
//     gtp.RegisterGfx ("model.values", "", this, &Model::GtpShowValues);
    gtp.RegisterGfx ("model.active_nodes", "", this, &Model::GtpActiveNodes);
    gtp.RegisterGfx ("model.reset",        "", this, &Model::GtpReset);

    string model = "model.param";

    gtp.RegisterParam (model, "update",             &Param::update);

    gtp.RegisterParam (model, "expand_at_n",        &Param::expand_at_n);
    gtp.RegisterParam (model, "expand_max_nulls",   &Param::expand_max_nulls);

    gtp.RegisterParam (model, "max_rave_n",         &Param::max_rave_n);
    gtp.RegisterParam (model, "explore_coeff",      &Param::explore_coeff);
    gtp.RegisterParam (model, "boltzmann_constant", &Param::boltzmann_constant);
  }

//   void GtpShowValues (Gtp::Io& io) {
//     io.CheckEmpty ();
//     Player pl = board.GetBoard().ActPlayer ();

//     NatMap <Vertex, double> values;

//     FillValues (values);
//     ForEachNat (Vertex, v) {
//       if (!board.GetBoard().IsLegal(pl, v)) {
//         values [v] = NaN;
//       }
//     }
//     values.ScalePositive ();
    
//     Gtp::GoguiGfx gfx;
//     ForEachNat (Vertex, v) {
//       if (v.IsOnBoard()) 
//         gfx.SetInfluence (v.ToGtpString(), values[v]);
//     }
//     gfx.Report(io);
//   }

    
  void GtpActiveNodes (Gtp::Io& io) {
    io.CheckEmpty ();
    if (!SyncWithBoard ()) return;
    io.out << endl;
    vector <Node*> act = active;
    sort (act.begin(), act.end(), Node::PrintCmp); 
    rep (ii, act.size()) {
      (act[ii]->stat.N() == 0.0 ? cerr : io.out)
        << act[ii]->ToString (true) << endl;
    }
  }

  void GtpReset (Gtp::Io& io) {
    io.CheckEmpty ();
    Reset();
  }

  Node* root;
  uint sync_board_move_no;

  vector <Node*> active;
  vector <Node*> old_active;

  FullBoard& board;
};

// -----------------------------------------------------------------------------
}
