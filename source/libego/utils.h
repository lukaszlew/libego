//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef UTILS_H_
#define UTILS_H_

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
#define reps(i,s,n)  for (int i = (s); i < int(n); i++)

#define _STRING(x) #x
#define STRING(x) _STRING(x)

const float large_float = 1000000000000.0;

float process_user_time ();

// string/stream opereations

class SS {
public:
  template <typename T> SS& operator<< (const T& elt) {
    buffer_ << elt;
    return *this;
  }

  operator std::string () const;
private:
  std::ostringstream buffer_;
};


char getc_non_space (istream& is);
bool is_all_whitespace (const string& s);
void remove_empty_lines (string* s);
void remove_trailing_whitespace (string* str);

template <typename T>
bool string_to(const string &s, T* i) {
  istringstream myStream(s);
  return (myStream >> *i);
}

template <typename T>
string ToString(const T& val) {
  ostringstream s;
  s << val;
  return s.str();
}

// performance macros

#ifdef _MSC_VER

// put more of them in various places of source to force almost flat
// source in Visual C++ (no function calls except remove_chain)

#define no_inline   __declspec(noinline)
#define flatten
#define all_inline  __forceinline

#else

#define no_inline   __attribute__((noinline))
#define flatten     __attribute__((flatten))
#define all_inline  __attribute__((always_inline))

#endif //_MSC_VER

#endif
