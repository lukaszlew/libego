#ifndef _GTP_H_
#define _GTP_H_

#include <sstream>
#include <map>
#include <string>
#include <boost/bind.hpp>
#include <boost/function.hpp>

namespace Gtp {

using namespace std;

// -----------------------------------------------------------------------------
// Command communication interface.

class Io {
public:

  istream& In  ();
  ostream& Out ();

  // Reads and returns type T, throws syntax_error otherwise.
  template <typename T> T Read ();

  // Returns true is all that's left in In() is whitespace.
  bool IsEmpty ();

  // Throws syntax_error if a non-whitespace is still in In()
  void CheckEmpty ();

  // Exception that can be throwed by a command and will be catched by Repl.
  struct Error {
    Error (const string& msg_) : msg(msg_) {}
    string msg;
  };

  const static Error syntax_error;

private:
  Io (istringstream& arg_line);
  friend class Repl;
private:
  istringstream& in;
  ostringstream out;
};

// -----------------------------------------------------------------------------
// GTP::Callback and some constructors.

// A callback for a GTP command.
typedef boost::function< void(Io&) > Callback;

// Creates GTP callback out of object pointer and a method.
// Example: OfMethod<MyClass> (this, &MyClass::MyMethod)
template <class T>
Callback OfMethod (T* object, void(T::*member)(Io&));

// Creates a GTP callback that always returns the same string.
Callback StaticCommand (const string& ret);

// Creates a GTP callback that without arguments in its io prints value of var,
// with one argument sets value of var using Io::Read<T>.
template <typename T>
Callback GetSetCommand (T* var);

// -----------------------------------------------------------------------------
// GTP read-eval-print-loop (repl)

class Repl {
public:
  Repl (istream&, ostream&);
  void RegisterCommand (const string& name, Callback command);
  void Run ();
  bool IsCommand (const string& name);

  // exception that can be raised by a command
  class Quit {};

private:
  // commands built-in into interpreter (registered during interpreter construction)
  void CListCommands (Io&);
  void CKnownCommand (Io&);
  void CQuit (Io&);
  
  void Report (bool success, const string& name);

private:
  map <string, Callback> callbacks;
  istream& in;
  ostream& out;
};

// -----------------------------------------------------------------------------
// internal 

template <typename T>
T Io::Read () {
  assert(in.good());
  T t;
  in >> t;
  if (!in) {
    in.clear();
    throw syntax_error;
  }
  return t;
}

template <class T>
Callback OfMethod(T* object, void (T::*member) (Io&)) {
  return boost::bind(member, object, _1);
}

namespace {
  template <typename T>
  void GetSetCallback (T* var, Io& io) {
    if (io.IsEmpty()) {
      io.Out () << *var;
      return;
    }
    // TODO handle quoted strings
    T tmp = io.Read<T>();
    io.CheckEmpty ();
    *var = tmp;
  }
}

template <typename T>
Callback GetSetCommand (T* var) {
  return bind(GetSetCallback<T>, var, _1);
}

// -----------------------------------------------------------------------------

} // namespace Gtp

#endif // _GTP_H_
