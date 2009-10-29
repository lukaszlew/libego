#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK // IMPROVE: why do I need this?
#include <boost/algorithm/string/trim.hpp>
#include <boost/test/unit_test.hpp>
#include <iostream>
#include <sstream>

#include "gtp.h"

using std::endl;
using std::stringstream;
using std::string;
using std::istream;

// -----------------------------------------------------------------------------

// A simple command.
void CAdd (Gtp::Io& io) {
  io.out << io.Read<int>() + io.Read<unsigned int>(11);
  io.CheckEmpty ();
}

// A simple command implemented as a method. 
class DummyGtpUser {
public:
  DummyGtpUser (Gtp::Repl& gtp) {
    gtp.Register ("echo",  this, &DummyGtpUser::CEcho);
    gtp.Register ("echo2", this, &DummyGtpUser::CEcho);
    gtp.Register ("echo2", this, &DummyGtpUser::CEcho);
  }

private:
  void CEcho (Gtp::Io& io) {
    std::string s;
    std::getline(io.in, s);
    boost::trim(s);
    io.out << s << endl;
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
    gtp.Register ("+", CAdd);
    gtp.RegisterStatic ("whoami",    "Santa ");
    gtp.RegisterStatic ("whoami",    "Claus ! ");
    gtp.RegisterStatic ("whoareyou", "Merry");
    gtp.Register ("var_f", Gtp::GetSetCallback (&f));
    gtp.Register ("var_i", Gtp::GetSetCallback (&i));
    gtp.Register ("var_s", Gtp::GetSetCallback (&s));
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
    << "this won't be executed" << endl
    ;

  stringstream expected_help_msg;
  expected_help_msg
    << "= "
    << "+" << endl
    << "echo" << endl
    << "echo2" << endl
    << "gtpfile" << endl
    << "help" << endl
    << "known_command" << endl
    << "list_commands" << endl
    << "quit" << endl
    << "var_f" << endl
    << "var_i" << endl
    << "var_s" << endl
    << "whoami" << endl
    << "whoareyou" << endl
    << endl
    ;

  expected_out
    << expected_help_msg.str()
    << expected_help_msg.str()
    << "? too many parameters" << endl << endl
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
    << "echo2 print twice " << endl
    << "+  1  2  " << endl
    << "  +  1  2 3 " << endl
    << "  +  11" << endl
    << "  whoami " << endl
    << "  whoami today  " << endl
    << "  whoareyou  " << endl
    << "   " << endl
    << "  who are you " << endl
    ;
  expected_out
    << "= GTP was never so simple" << endl << endl
    << "= print twice" << endl << "print twice" << endl << endl
    << "= 3" << endl << endl
    << "? too many parameters" << endl << endl
    << "= 22" << endl << endl
    << "= Santa Claus !" << endl << endl
    << "? too many parameters" << endl << endl
    << "= Merry" << endl << endl
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
    << "? too many parameters" << endl << endl
    << "? too many parameters" << endl << endl
    ;
  gtp.Run(in, out);
  BOOST_CHECK_EQUAL (out.str(), expected_out.str());
  BOOST_CHECK_EQUAL (f, 123.25);
  BOOST_CHECK_EQUAL (i, 321);
  BOOST_CHECK_EQUAL (s, "no_spaces");
}

BOOST_AUTO_TEST_CASE (RunSingleCommand) {
  string response;
  BOOST_CHECK_EQUAL (gtp.RunOneCommand ("echo \t out there ", &response),
                     Gtp::Repl::Success);
  BOOST_CHECK_EQUAL (response, "out there");
  BOOST_CHECK_EQUAL (gtp.RunOneCommand ("please", &response),
                     Gtp::Repl::Failure);
  BOOST_CHECK_EQUAL (response, "unknown command: \"please\"");
  BOOST_CHECK_EQUAL (gtp.RunOneCommand ("quit", &response), Gtp::Repl::Quit);
  BOOST_CHECK_EQUAL (response, "bye");
  BOOST_CHECK_EQUAL (gtp.RunOneCommand ("123 \t ", &response), Gtp::Repl::NoOp);
  BOOST_CHECK_EQUAL (response, "");
}

// Private Default Constructor class
class Pdc {
public:
  int i;
  string s;

  static Pdc OfGtpStream (istream& in) {
    Pdc pdc;
    in >> pdc.i >> pdc.s;
    return pdc;
  }

private:
  Pdc () {};
};

void CReadPdc (Gtp::Io& io) {
  Pdc pdc = io.Read <Pdc> ();
  io.out << pdc.s << " - " << pdc.i;
}

BOOST_AUTO_TEST_CASE (NoDefaultConstructorGtp) {
  gtp.Register ("read_pdc", CReadPdc);

  string response;

  BOOST_CHECK_EQUAL (gtp.RunOneCommand ("read_pdc 42 istheanswer", &response),
                     Gtp::Repl::Success);
  BOOST_CHECK_EQUAL (response, "istheanswer - 42");

  BOOST_CHECK_EQUAL (gtp.RunOneCommand ("read_pdc istheanswer 42", &response),
                     Gtp::Repl::Failure);
  BOOST_CHECK_EQUAL (response, "syntax error");
}

BOOST_AUTO_TEST_SUITE_END()
