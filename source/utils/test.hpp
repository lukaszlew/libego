#ifndef TEST_H_
#define TEST_H_

#include <cassert>

// TODO replace __assert_fail with a backtrace and core dump
// http://stackoverflow.com/questions/18265/

// debugging  macros

#define STRING__(x) #x
#define STRING(x) STRING__(x)

// Debug print.
#define QQ(x) cerr << x << flush;

// Debug print with expresion equation.
#define QQEE(x) cerr << #x << " = " << x << endl << flush;

// Automatic progam failure with report.
#define FAIL(msg) (__assert_fail (msg, __FILE__, __LINE__, __PRETTY_FUNCTION__))

// Always evaluate condition and always assert true.
#define CHECK(expr) ((expr) ? void(0) : FAIL (#expr))

// Evalueate and assert only if kCheckAsserts == true
#define ASSERT(expr) (kCheckAsserts ? CHECK (expr) : void(0))

#endif
