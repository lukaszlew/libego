#include <boost/algorithm/string/trim.hpp>
#include <boost/foreach.hpp>
#include <boost/lambda/lambda.hpp>
#include <iostream>
#include <fstream>
#include "gtp.hpp"

namespace Gtp {

Io::Io (const string& params_) : params (params_), success(true), quit_gtp (false) { 
}

void Io::SetError (const string& message) {
  out.str (message);
  success = false;
}

void Io::CheckEmpty() {
  string s;
  in >> s;
  if (in) {
    SetError ("too many parameters");
    throw Repl::Return();
  }
  in.clear();
}

bool Io::IsEmpty() {
  string s;
  std::streampos pos = in.tellg();
  in >> s;
  bool ok = in;
  in.seekg(pos);
  in.clear();
  return !ok;
}

void Io::PrepareIn () {
  in.clear();
  in.str (params);
}

string Io::Report () const {
  return boost::trim_right_copy (out.str()); // remove bad endl in msg
}

// -----------------------------------------------------------------------------

// IMPROVE: can't Static Aux be anonymous function

void StaticAux(const string& ret, Io& io) {
  io.CheckEmpty ();
  io.out << ret;
}

Repl::Callback StaticCommand (const string& ret) {
  return boost::bind(StaticAux, ret, _1);
}

// -----------------------------------------------------------------------------

Repl::Repl () {
  Register ("list_commands", this, &Repl::CListCommands);
  Register ("help",          this, &Repl::CListCommands);
  Register ("known_command", this, &Repl::CKnownCommand);
  Register ("quit",          this, &Repl::CQuit);
  Register ("gtpfile",       this, &Repl::CGtpFile);
}

void Repl::Register (const string& name, Callback callback) {
  callbacks[name].push_back (callback);
}

void Repl::RegisterStatic (const string& name, const string& response) {
  Register (name, StaticCommand(response));
}

void ParseLine (const string& line, int* id, string* command, string* rest) {
  stringstream ss;
  BOOST_FOREACH (char c, line) {
    if (false) {}
    else if (c == '\t') ss << ' ';
    else if (c == '\n') assert (false);
    else if (c <= 31 || c == 127) continue;
    else if (c == '#') break;  // remove comments
    else ss << c;
  }
  *id = -1;
  *command = "";
  *rest = "";
  ss >> *id;
  if (!ss) {
    ss.clear ();
    ss.seekg (ios_base::beg);
  }
  ss >> *command;
  getline (ss, *rest);
}

Repl::Status Repl::RunOneCommand (const string& line, string* report) {
  int id;
  string command, params;

  ParseLine (line, &id, &command, &params);

  Io io(params);

  *report = "";
  if (command == "") return NoOp;

  if (IsCommand (command)) {
    // Callback call with optional fast return.
    BOOST_FOREACH (Callback& cmd, callbacks [command]) {
      io.PrepareIn();
      try { cmd (io); } catch (Return) { }
    }
  } else {
    io.SetError ("unknown command: \"" + command + "\"");
  }

  *report = io.Report();
  if (io.quit_gtp) return Quit;
  if (io.success)  return Success;
  return Failure;
}

void Repl::Run (istream& in, ostream& out) {
  in.clear();
  while (true) {
    string line, report;
    if (!getline (in, line)) break;

    Status status = RunOneCommand (line, &report);
    if (status == NoOp) continue;

    out << (status == Failure ? "?" : "=") << " "
        << report
        << endl << endl;

    if (status == Quit) break;
  }
}

void Repl::CListCommands (Io& io) {
  io.CheckEmpty();
  pair<string, list<Callback> > p;
  BOOST_FOREACH(p, callbacks) {
    io.out << p.first << endl;
  }
}

void Repl::CKnownCommand (Io& io) {
  io.out << boolalpha << IsCommand(io.Read<string> ());
  io.CheckEmpty();
}

void Repl::CQuit (Io& io) {
  io.CheckEmpty();
  io.out << "bye";
  io.quit_gtp = true;
}

void Repl::CGtpFile (Io& io) {
  string file = io.Read<string> ();
  io.CheckEmpty();
  ifstream in (file.c_str());
  if (!in) {
    io.SetError ("No such file: \"" + file + "\"");
    return;
  }
  Run (in, cerr);
}

bool Repl::IsCommand (const string& name) {
  return callbacks.find(name) != callbacks.end();
}

namespace detail {

#define Specialization(T)                               \
  template<> T IoReadOfStream< T > (istream& in) {      \
    T t;                                                \
    in >> t;                                            \
    return t;                                           \
  }

  Specialization(bool);
  Specialization(char);
  Specialization(int);
  Specialization(unsigned int);
  Specialization(float);
  Specialization(double);
  Specialization(string);
#undef Specialization

} // namespace detail

} // namespace Gtp
