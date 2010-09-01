#ifdef NDEBUG
#undef NDEBUG
#endif

#include <cassert>
#include <cstdio>
#include "test.hpp"

// TODO replace __assert_fail with a backtrace and core dump
// http://stackoverflow.com/questions/18265/

void TestFail (const char* msg, const char* file, int line, const char* pf) {
  fprintf (stderr, "ASSERT FAIL: %s %s %d %s\n", msg, file, line, pf );
  fflush (stderr);
  exit(1);
}
