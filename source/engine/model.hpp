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
    ForEachNat (Move, m) {
      CHECK (children [m] == NULL);
      if (!m.GetVertex().IsOnBoard()) continue;
      children [m] = new Node (this, m);
    }
    mature = true;
  }


  string ToString () {
    return
      last_move.ToGtpString() + ";  S: " +
      stat.ToString() + "  R: " +
      rave.ToString();
  }


  static bool PrintCmp (Node* a, Node* b) {
    return a->PrintValue() > b->PrintValue();
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

  Model () {
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
    // TODO expand
  }

  string ToString () {
    return "";
  }
    
  Node* root;
  vector <Node*> active;         // * seq
  vector <Node*> to_update_stat; // * seq *
  vector <Node*> to_update_rave; // * seq[:-1] * seq[-1] *

  vector <Node*> tmp;
};

// -----------------------------------------------------------------------------

}
