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

  istringstream in;
  ostringstream out;

  // Repl Will print "? message" on the output
  void SetError (const string& message);

  // Prints "syntax error" to out and return directly to Repl through exception.
  void ThrowSyntaxError ();

  // Reads and returns type T, throws syntax_error otherwise.
  template <typename T> T Read ();

  // Reads and returns type T, returns default value in case of syntax error.
  template <typename T> T Read (const T& default_value);

  // Returns true is all that's left in in is whitespace.
  bool IsEmpty ();

  // Throws syntax_error if a non-whitespace is still in in
  void CheckEmpty ();

private:
  friend class Repl;

  Io (const string& params);
  void PrepareIn ();
  void Report (ostream& out) const;

  bool success;
  bool quit_gtp;
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
  Repl ();

  void Register (const string& name, Callback command);

  template <class T>
  void Register (const string& name, T* object, void(T::*member)(Io&));

  void RegisterStatic (const string& name, const string& response);

  void Run (istream&, ostream&);

  bool IsCommand (const string& name);

private:
  // Exception that can be throwed by a command and will be catched by Repl::Run
  struct Return {};

  friend class Io;

  // commands built-in into interpreter (registered during interpreter construction)
  void CListCommands (Io&);
  void CKnownCommand (Io&);
  void CQuit (Io&);
  
private:
  map <string, Callback> callbacks;
};

// -----------------------------------------------------------------------------
// internal implementation

template <typename T>
T Io::Read () {
  in.clear();
  T t;
  in >> t;
  if (in.fail()) {
    in.clear();
    ThrowSyntaxError ();
  }
  return t;
}

template <typename T>
T Io::Read (const T& default_value) {
  in.clear();
  T t;
  in >> t;
  if (in.fail()) {
    in.clear();
    return default_value;
  }
  return t;
}

template <class T>
Callback OfMethod(T* object, void (T::*member) (Io&)) {
  return boost::bind(member, object, _1);
}

template <class T>
void Repl::Register (const string& name, T* object, void(T::*member)(Io&)) {
  Register (name, OfMethod(object, member));
}


namespace {
  template <typename T>
  void GetSetCallback (T* var, Io& io) {
    if (io.IsEmpty()) {
      io.out << *var;
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
