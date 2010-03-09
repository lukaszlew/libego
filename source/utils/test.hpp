#ifndef TEST_H_
#define TEST_H_

#include <cstdlib>

void TestFail (const char* msg, const char* file, int line, const char* pf);

// TODO ifndef DEBUG

// On default there should be one global kCheckAsserts == false
// During debugging it might be convinient to define local variable
// with the same name to change behaviour of subset of ASSERTS.

// ASSERT's arument should have no side effects.
// CHECK might be used safely with side effects.
// ASSERT2 and CHECK2 take as a second argument block of code that
// will be executed in case of failure.
// It might be convinient to use WW in this blocks.

#if 1
const bool kCheckAsserts = false;
#else
const bool kCheckAsserts = true;
#endif

// debugging  macros

#define STRING__(x) #x
#define STRING(x) STRING__(x)

// Debug print to track code execution.
// Funny to use: Just write QQ(a) to have letter a printed when it is executed.
#define QQ(x) cerr << #x << flush;

// Convinient macro for printing value of expression.
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
