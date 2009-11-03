#ifndef TEST_H_
#define TEST_H_

#include <cassert>

// TODO replace __assert_fail with a backtrace and core dump
// http://stackoverflow.com/questions/18265/

#define NO_OP (static_cast<void>(0))

#define FAIL(msg) (__assert_fail (msg, __FILE__, __LINE__, __PRETTY_FUNCTION__))

#define CHECK(expr) ((expr) ? NO_OP : FAIL (#expr))

#define CHECK_IF(cond, expr) ((cond) ? CHECK (expr) : NO_OP)

#define ASSERT(expr) CHECK_IF (kCheckAsserts, (expr))

#endif
