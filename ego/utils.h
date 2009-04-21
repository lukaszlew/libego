#ifndef _UTILS_H_
#define _UTILS_H_

#include <sstream>
#include <string>

using namespace std;

typedef unsigned int uint;
typedef unsigned long long uint64;

// standard macros

#define qq(x) cerr << x << flush;
#define qqv(x) cerr << #x << " = " << x << endl << flush;

//TODO rename to ignore
#define unused(p) (void)(p)

#define rep(i,n)     for (uint i = 0; i != (uint)(n); i++)
#define seq(i,a,b)   for (let (i, a); i <= (b); i++)
#define dseq(i,b,a)  for (let (i, b); i >= (a); i--)

const float large_float = 1000000000000.0;

float process_user_time ();

// string/stream opereations

class SS {
public:
  template <typename T> SS& operator<< (const T& elt);
  operator std::string () const;
private:
  std::ostringstream buffer_;
};


char getc_non_space (istream& is);
bool is_all_whitespace (const string& s);
void remove_empty_lines (string* s);
void remove_trailing_whitespace (string* str);

// performance macros

#if 0

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#else

#define likely(x)   (x)
#define unlikely(x) (x)

#endif



#if 1

#define no_inline   __attribute__((noinline))
#define flatten     __attribute__((flatten))
#define all_inline  __attribute__((always_inline))

#else

#define no_inline
#define flatten
#define all_inline

#endif

#endif
