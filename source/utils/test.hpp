#ifndef TEST_H_
#define TEST_H_

#include <cassert>

// TODO include backtrace + core dump
// http://stackoverflow.com/questions/18265/
#define CHECK(expr)                                                     \
  ((expr) ?                                                             \
   (static_cast<void>(0)) :                                             \
   __assert_fail (#expr, __FILE__, __LINE__, __PRETTY_FUNCTION__))

#define TEST(exp) (kDoTests ? CHECK(exp) : (static_cast<void>(0)))

#endif
