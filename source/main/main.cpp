//
// Copyright 2006 and onwards, Lukasz Lew
//

#include "gtp_gogui.hpp"

Gtp::ReplWithGogui gtp;

#include "engine.hpp"
//#include "gui.h"
#include "mcts_gtp.hpp"
#include "mm_train.hpp"



// TODO automatize through CMake (and add git SHA1)
#ifndef VERSION
#define VERSION unknown
#endif

void GtpBenchmark (Gtp::Io& io) {
  uint n = io.Read<uint> (100000);
  io.CheckEmpty ();
  io.out << Benchmark::Run (n);
}

void GtpBoardTest (Gtp::Io& io) {
  bool print_moves = io.Read<bool> (false);
  io.CheckEmpty ();
  PlayoutTest (print_moves);
}

void GtpSamplerTest (Gtp::Io& io) {
  bool print_moves = io.Read<bool> (false);
  io.CheckEmpty ();
  SamplerPlayoutTest (print_moves);
}

void GtpMmTest (Gtp::Io& io) {
  io.CheckEmpty ();
  Mm::Test ();
}

int main(int argc, char** argv) {
  // no buffering to work well with gogui
  setbuf (stdout, NULL);
  setbuf (stderr, NULL);
  srand48 (123);

  gtp.RegisterStatic("name", "Libego");
  gtp.RegisterStatic("version", STRING(VERSION));
  gtp.RegisterStatic("protocol_version", "2");
  gtp.Register ("benchmark", GtpBenchmark);
  gtp.Register ("board_test", GtpBoardTest);
  gtp.Register ("sampler_test", GtpSamplerTest);
  gtp.Register ("mm_test", GtpMmTest);

  Engine& engine = *(new Engine());
  MctsGtp mcts_gtp (engine);

  reps (ii, 1, argc) {
    if (ii == argc-1 && string (argv[ii]) == "gtp") continue;
    string response;
    switch (gtp.RunOneCommand (argv[ii], &response)) {
    case Gtp::Repl::Success:
      cerr << response << endl;
      break;
    case Gtp::Repl::Failure:
      cerr << "Command: \"" << argv[ii] << "\" failed." << endl;
      return 1;
    case Gtp::Repl::NoOp:
      break;
    case Gtp::Repl::Quit:
      cerr << response << endl;
      return 0;
    }
  }

  if (argc == 1 || string(argv[argc-1]) == "gtp") {
    gtp.Run (cin, cout);
  }

  delete &engine;

  return 0;
}
