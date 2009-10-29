#ifndef GTP_H_
#define GTP_H_

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <list>
#include <map>
#include <sstream>
#include <string>

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
  string Report () const;

private:
  const string& params;
  bool success;
  bool quit_gtp;
};


// -----------------------------------------------------------------------------
// GTP read-eval-print-loop (repl)

class Repl {
public:
  typedef boost::function< void(Io&) > Callback;

  Repl ();

  void Register (const string& name, Callback command);

  template <class T>
  void Register (const string& name, T* object, void(T::*member)(Io&));

  void RegisterStatic (const string& name, const string& response);

  enum Status {
    Success,
    Failure,
    Quit,
    NoOp
  };

  Status RunOneCommand (const string& line, string* report);

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
  void CGtpFile (Io&);
  
private:
  map <string, list<Callback> > callbacks;
};

// Creates a callback that:
//  - without arguments prints value of given variable
//  - with one argument sets the given variable to this value using Io::Read<T>.
template <typename T>
Repl::Callback GetSetCallback (T* var);


// -----------------------------------------------------------------------------
// implementation
namespace detail {
  template <class T> T IoReadOfStream (istream& in) { return T::OfGtpStream (in); }
#define Specialization(T) template<> T IoReadOfStream< T > (istream& in)
  Specialization(bool);
  Specialization(char);
  Specialization(int);
  Specialization(unsigned int);
  Specialization(float);
  Specialization(double);
  Specialization(string);
#undef Specialization
}


template <typename T>
T Io::Read () {
  in.clear();
  T t = detail::IoReadOfStream<T> (in);
  if (in.fail()) {
    in.clear();
    SetError ("syntax error");
    throw Repl::Return();
  }
  return t;
}

template <typename T>
T Io::Read (const T& default_value) {
  in.clear();
  T t = detail::IoReadOfStream<T> (in);
  if (in.fail()) {
    in.clear();
    return default_value;
  }
  return t;
}

template <class T>
void Repl::Register (const string& name, T* object, void(T::*member)(Io&)) {
  Register (name, boost::bind(member, object, _1));
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
Repl::Callback GetSetCallback (T* var) {
  return bind(GetSetCallback<T>, var, _1);
}

// -----------------------------------------------------------------------------

} // namespace Gtp

#endif // _GTP_H_
