#include <sstream>
#include <iostream>
#include <boost/algorithm/string/trim.hpp>
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK // IMPROVE: why do I need this?
#include <boost/test/unit_test.hpp>

#include "gtp.h"

using std::endl;
using std::stringstream;
using std::string;

// -----------------------------------------------------------------------------

// A simple command.
void CAdd (Gtp::Io& io) {
  io.Out() << io.Read<int>() + io.Read<int>();
  io.CheckEmpty ();
}

// A simple command implemented as a method. 
class DummyGtpUser {
public:
  DummyGtpUser (Gtp::Repl& gtp) {
    gtp.RegisterCommand ("echo", Gtp::OfMethod(this, &DummyGtpUser::CEcho));
    gtp.RegisterCommand ("echo2", this, &DummyGtpUser::CEcho); // same as above
  }
private:
  void CEcho (Gtp::Io& io) {
    std::string s;
    std::getline(io.In(), s);
    boost::trim(s);
    io.Out () << s;
  }
};

// Environement of all unit tests.
struct Fixture {
  stringstream in;
  stringstream out;
  stringstream expected_out;

  Gtp::Repl gtp;
  DummyGtpUser gtp_user;

  float f;
  int i;
  string s;

  Fixture()
  : gtp_user (gtp), f (1.5), i(-1), s("GTP rulez")
  {
    gtp.RegisterCommand("+", CAdd);
    gtp.RegisterCommand("whoami", Gtp::StaticCommand("Santa !"));
    gtp.RegisterCommand("var_f", Gtp::GetSetCommand (&f));
    gtp.RegisterCommand("var_i", Gtp::GetSetCommand (&i));
    gtp.RegisterCommand("var_s", Gtp::GetSetCommand (&s));
  }
};

// -----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_SUITE( f, Fixture )

BOOST_AUTO_TEST_CASE (BuiltInCommands) {
  in 
    << "help" << endl
    << "list_commands # same as help" << endl
    << "list_commands badArgument" << endl
    << "known_command help" << endl
    << "known_command hlp" << endl
    << "quit # bye bye" << endl
    ;

  stringstream expected_help_msg;
  expected_help_msg
    << "= "
    << "+" << endl
    << "echo" << endl
    << "echo2" << endl
    << "help" << endl
    << "known_command" << endl
    << "list_commands" << endl
    << "quit" << endl
    << "var_f" << endl
    << "var_i" << endl
    << "var_s" << endl
    << "whoami" << endl
    << endl
    ;

  expected_out
    << expected_help_msg.str()
    << expected_help_msg.str()
    << "? syntax error" << endl << endl
    << "= true" << endl << endl
    << "= false" << endl << endl
    << "= bye" << endl << endl
    ;

  gtp.Run(in, out);
  BOOST_CHECK_EQUAL (out.str(), expected_out.str());
}

BOOST_AUTO_TEST_CASE (RegisteredCommands) {
  in
    << "echo  GTP was never so simple  " << endl
    << "echo2 command registered with a helper works too" << endl
    << "+  1  2  " << endl
    << "  +  1  2 3 " << endl
    << "  whoami " << endl
    << "  whoami today  " << endl
    << "   " << endl
    << "  who are you " << endl
    ;
  expected_out
    << "= GTP was never so simple" << endl << endl
    << "= command registered with a helper works too" << endl << endl
    << "= 3" << endl << endl
    << "? syntax error" << endl << endl
    << "= Santa !" << endl << endl
    << "? syntax error" << endl << endl
    << "? unknown command: \"who\"" << endl << endl
    ;

  gtp.Run(in, out);
  BOOST_CHECK_EQUAL (out.str(), expected_out.str());
}

BOOST_AUTO_TEST_CASE (GetSetCommands) {
  in
    << "var_f" << endl
    << "var_i" << endl
    << "var_s" << endl
    << "var_f 123.25" << endl
    << "var_i 321" << endl
    << "var_s no_spaces" << endl
    << "var_f 123.a" << endl
    << "var_i 321.5" << endl
    ;
  expected_out
    << "= 1.5" << endl << endl
    << "= -1" << endl << endl
    << "= GTP rulez" << endl << endl
    << "= " << endl << endl
    << "= " << endl << endl
    << "= " << endl << endl
    << "? syntax error" << endl << endl
    << "? syntax error" << endl << endl
    ;
  gtp.Run(in, out);
  BOOST_CHECK_EQUAL (out.str(), expected_out.str());
  BOOST_CHECK_EQUAL (f, 123.25);
  BOOST_CHECK_EQUAL (i, 321);
  BOOST_CHECK_EQUAL (s, "no_spaces");
}

BOOST_AUTO_TEST_SUITE_END()
