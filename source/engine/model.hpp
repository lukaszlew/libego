const double NaN = std::numeric_limits<double>::quiet_NaN();

namespace M {

namespace Param {
  bool   update = true;

  double expand_at_n = 100.0;
  uint   expand_max_stars = 2;

  double max_rave_n = 1000.0;
  double explore_coeff = 0.0;
  double boltzmann_constant = 0.0;
}

// -----------------------------------------------------------------------------

struct Stat {
  Stat (bool maximize_) {
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
    sprintf (buf, "%+3.3f(%5.0f)", Mean(), N());
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

  Node (Node* parent_, Move last_move_)
    : stat (last_move_.GetPlayer() == Player::Black())
  {
    parent = parent_;
    last_move = last_move_;
    children.SetToZero();
    null_child = NULL;
    if (parent != NULL) {
      depth = parent->depth + 1;
      stars_on_path = parent->stars_on_path + (last_move == Move::Null());
    } else {
      depth = 0;
      stars_on_path = 0;
    }
    expanded = false;
    activate_count = 0;
  }


  ~Node () {
    ForEachNat (Move, m) {
      if (children [m] != NULL) {
        delete children [m];
        children[m] = NULL;
      }
    }
    if (null_child != NULL) {
      delete null_child;
      null_child = NULL;
    }
  }

  void Activated () {
    if (!expanded &&
        activate_count >= Param::expand_at_n &&
        stars_on_path < Param::expand_max_stars)
    {
      ForEachNat (Move, m) {
        CHECK (children [m] == NULL);
        if (m != Move::Null () && m.GetVertex().IsOnBoard()) {
          children [m] = new Node (this, m);
        }
      }
      null_child =  new Node (this, Move::Null());
      null_child->activate_count += Param::expand_at_n;
      expanded = true;
    }
  }

  // PRINTING

  string ToString (bool full = false) {
    ostringstream out;
    if (full) {
      vector <Move> path = Path();
      rep(ii, path.size()) {
        out << path[ii].ToGtpString();
        if (ii < path.size() - 1)
          out << ", ";
      }
    } else {
      out << last_move.ToGtpString();
    }

    out << " (" << activate_count << ")"
        << "   S: " << stat.ToString();

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
      Node* c = children [m];
      if (c == NULL) continue;
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

  bool expanded;
  Node* parent;
  uint depth;
  uint stars_on_path;
  Move last_move;
  uint activate_count;

  NatMap <Move, Node*> children;
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
    root = new Node (NULL, board.GetBoard().LastMove());
    root->activate_count += Param::expand_at_n;
    sync_board_move_no = board.MoveHistory().size();
    // TODO add children to root by default including childrens to star
  }


  bool SyncWithBoard () {
    active.clear();
    AddNonStarToActive (root);

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
    old_active.swap (active); // old_active = active;
    active.clear();
    
    rep (ii, old_active.size()) {
      Node* old = old_active[ii];
      ASSERT (old != NULL);
      if (old->last_move == Move::Null ())
        Activate (old, 0);
      AddNonStarToActive (old->children [m]);
    }
  }


  void AddNonStarToActive (Node* n) {
    Activate (n);
    if (n != NULL) {
      CHECK (n->last_move != Move::Null ()); // no double stars should happen
      Activate (n->null_child);
    }
  }

  void Activate (Node* n, int inc = 1) {
    if (n == NULL) return;
    active.push_back (n);
    n->activate_count += inc;
    n->Activated ();
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
    gtp.RegisterParam (model, "expand_max_stars",   &Param::expand_max_stars);

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
    rep (ii, active.size()) {
      io.out << active[ii]->ToString (true) << endl;
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
