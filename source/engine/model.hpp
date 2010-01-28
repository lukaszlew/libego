const double NaN = std::numeric_limits<double>::quiet_NaN();

namespace M {

// -----------------------------------------------------------------------------

struct Stat {
  Stat () {
    n = 0.0;
    sum = 0.0;
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
};

// -----------------------------------------------------------------------------

// TODO Expand, backker, move selector, gfx display
// TODO priors na Expand

struct Node {

  Node (Node* parent_, Move last_move_) {
    parent = parent_;
    last_move = last_move_;
    children.SetToZero();
    if (parent != NULL) {
      depth = parent->depth + 1;
    } else {
      depth = 0;
    }
    mature = false;
  }


  ~Node () {
    ForEachNat (Move, m) {
      if (children [m] != NULL) {
        delete children [m];
        children[m] = NULL;
      }
    }
  }


  void AddChildren () {
    CHECK (mature == false);
    ForEachNat (Move, m) {
      CHECK (children [m] == NULL);
      if (!m.GetVertex().IsOnBoard()) continue;
      children [m] = new Node (this, m);
    }
    mature = true;
  }


  string ToString (bool full = false) {
    ostringstream out;
    if (full) {
      vector <Move> path = Path();
      rep(ii, path.size()) out << path[ii].ToGtpString() << ", ";
    }

    return
      out.str () + 
      ";  S: " + stat.ToString() +
      ";  R: " + rave.ToString();
  }


  static bool PrintCmp (Node* a, Node* b) {
    return a->PrintValue() > b->PrintValue();
  }

  double Value () {
    return rave.Mean (); // TODO RAVE
  }

  double PrintValue () {
    return stat.N();
//     return 
//       last_move.GetPlayer().SubjectiveScore(stat.Mean()) +
//       1.0 / sqrt (stat.N());
  }

  string RecToString (double stat_min_n) {
    ostringstream out;
    rep (ii, depth) out << "  "; // indent
    out << ToString() << endl;

    vector <Node*> nodes;
    ForEachNat (Move, m) {
      Node* c = children [m];
      if (c == NULL) continue;
      if (c->PrintValue() < stat_min_n) continue;
      nodes.push_back (c);
    }

    sort(nodes.begin(), nodes.end(), PrintCmp);

    rep (ii, nodes.size()){
      out << nodes[ii]->RecToString (stat_min_n);
    }

    return out.str();
  }

  void UpdateStat (double result) {
    stat.Update (result);
    if (stat.N() >= Param::model_mature_at && !mature) {
      AddChildren ();
    }
  }

  vector<Move> Path () {
    vector<Move> path;
    if (parent == NULL) return path;
    path = parent->Path();
    path.push_back (last_move);
    return path;
  }

  bool mature;
  Node* parent;
  uint depth;
  Move last_move;

  NatMap <Move, Node*> children;
  Stat stat; // *, m1, m2, m3, last_move, *
  Stat rave; // *, m1, m2, m3, *, last_move, *
};

// -----------------------------------------------------------------------------

// TODO filter duplicates
// IDEA moze dla kazdego ruchu ograniczyc liczbe active'ow

struct Model {

  Model (FullBoard& board) : board (board) {
    root = new Node (NULL, Move(Player::White(), Vertex::Any()));
  }

  void Reset () {
    delete root;
    root = new Node (NULL, Move(Player::White(), Vertex::Any()));
  }

  void NewPlayout () {
    active.clear();
    to_update_stat.clear();
    to_update_rave.clear();

    active.push_back (root);
    to_update_stat.push_back (root);
    to_update_rave.push_back (root);
  }


  void NewMove (Move m) {
    tmp.swap (active); // tmp = active;
    active.clear();

    // this loop has to be before next one
    rep (ii, to_update_stat.size()) {
      Node* n = to_update_stat[ii];
      Node* child = n->children [m];
      if (child == NULL) continue;
      to_update_rave.push_back (child);
    }

    rep (ii, tmp.size()) {
      Node* active_node = tmp[ii];

      Node* child = active_node->children[m];
      if (child == NULL) continue;
      active.push_back (child);
      to_update_stat.push_back (child);
    }
    active.push_back (root);
  }
    
    
  void Update (double result) {
    rep (ii, to_update_stat.size()) {
      to_update_stat[ii]->UpdateStat (result);
    }

    rep (ii, to_update_rave.size()) {
      to_update_rave[ii]->rave.Update (result);
    }
  }


  Node* ActNode (const vector <Move>& history) {
    Node* longest = NULL;
    for (uint start = history.size() - 1; start < history.size(); start -= 1) {
      Node* act = root;
      reps (suffix_ii, start, history.size()) {
        Move m = history [suffix_ii];
        act = act->children [m];
        if (act == NULL || act->stat.N() < Param::model_act_node_min_visit) {
          CHECK (longest != NULL);
          return longest;
        }
      }
      longest = act;
    }
    CHECK (longest != NULL);
    return longest;
  }


  // TODO history remove
  void FillValues (NatMap<Vertex, double>& values, const vector <Move>& history, Player pl) {
    Node* act = ActNode (history);

    cerr << "Act node: " << act->ToString(true) << endl;

    ForEachNat (Vertex, v) {
      Move m = Move (pl, v);
      Node* child = act->children [m];
      if (child == NULL) {
        values [v] = NaN;
      } else {
        values [v] = child->Value ();
      }
    }
  }


  void RegisterInGtp (Gtp::ReplWithGogui& gtp) {
    gtp.RegisterGfx ("Model.Values", "", this, &Model::GtpShowValues);
    gtp.RegisterGfx ("Model.Values", "", this, &Model::GtpShowValues);
  }

  void GtpShowValues (Gtp::Io& io) {
    io.CheckEmpty ();
    Player pl = board.GetBoard().ActPlayer ();

    NatMap <Vertex, double> values;
    FillValues (values, board.MoveHistory(), pl);
    ForEachNat (Vertex, v) {
      if (!board.GetBoard().IsLegal(pl, v)) {
        values [v] = NaN;
      }
    }

    values.Scale (-1.0, 1.0);
    
    Gtp::GoguiGfx gfx;
    ForEachNat (Vertex, v) {
      if (v.IsOnBoard()) 
        gfx.SetInfluence (v.ToGtpString(), values[v]);
    }
    gfx.Report(io);
  }

    
  Node* root;
  vector <Node*> active;         // * seq
  vector <Node*> to_update_stat; // * seq *
  vector <Node*> to_update_rave; // * seq[:-1] * seq[-1] *
  vector <Node*> tmp;

  FullBoard& board;
};

// -----------------------------------------------------------------------------
//TODO better board.
}
