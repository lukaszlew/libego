#ifndef TEST_H_
#define TEST_H_

#include <cstdlib>

void TestFail (const char* msg, const char* file, int line, const char* pf);

// TODO ifndef DEBUG
const bool kCheckAsserts = false;

// debugging  macros

#define STRING__(x) #x
#define STRING(x) STRING__(x)

// Debug print.
#define QQ(x) cerr << #x << flush;

// Debug print with expresion equation.
#define WW(x) cerr << #x << " = " << x << endl << flush;

// Automatic progam failure with report.
#define FAIL(msg) (TestFail (msg, __FILE__, __LINE__, __PRETTY_FUNCTION__), exit(0))

// Always evaluate condition and always assert true.
#define CHECK2(expr, block) { if (!(expr)) { block; FAIL (#expr); } }

#define CHECK(expr) CHECK2(expr, {})

// Evalueate and assert only if kCheckAsserts == true
#define ASSERT2(expr, block) { if (kCheckAsserts) { CHECK2 (expr, block) } }

#define ASSERT(expr) ASSERT2(expr, {})


#endif
