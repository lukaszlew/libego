#ifndef _UTILS_H_
#define _UTILS_H_

float get_seconds ();
void fatal_error (const char* s);

// string/stream opereations

class SS {
public:
  template <typename T> SS& operator<< (const T& elt);
  operator std::string () const;
private:
  std::ostringstream buffer_;
};


char getc_non_space (istream& is);
bool is_all_whitespace (string s);
void remove_empty_lines (string* s);
void remove_trailing_whitespace (string* str);

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
