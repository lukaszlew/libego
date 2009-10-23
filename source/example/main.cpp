#include <iostream>
#include <string>

#include "gtp_gogui.h"

using namespace std;

void CGreet (Gtp::Io& io) {
  string name = io.Read<string> ("stranger");
  io.CheckEmpty ();
  io.out << "Hello " << name;
}

int main(int argc, char** argv) {
  Gtp::ReplWithGogui gtp;

  gtp.RegisterStatic("name", "GTP Example");
  gtp.RegisterStatic("protocol_version", "2");
  gtp.Register ("greet", CGreet);

  for (int ii = 1; ii < argc; ii+=1) {
    string response;
    switch (gtp.RunOneCommand (argv[ii], &response)) {
    case Gtp::Repl::Success:
      cerr << response << endl;
      break;
    case Gtp::Repl::Failure:
      cerr << "Command: \"" << argv[ii] << "\" failed.";
      return 1;
    case Gtp::Repl::NoOp:
      break;
    case Gtp::Repl::Quit:
      cerr << response << endl;
      return 0;
    }
  }

  gtp.Run (cin, cout);

  return 0;
}
