/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                           *
 *  This file is part of Library of Effective GO routines - EGO library      *
 *                                                                           *
 *  Copyright 2006 and onwards, Lukasz Lew                                   *
 *                                                                           *
 *  EGO library is free software; you can redistribute it and/or modify      *
 *  it under the terms of the GNU General Public License as published by     *
 *  the Free Software Foundation; either version 2 of the License, or        *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  EGO library is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public License        *
 *  along with EGO library; if not, write to the Free Software               *
 *  Foundation, Inc., 51 Franklin St, Fifth Floor,                           *
 *  Boston, MA  02110-1301  USA                                              *
 *                                                                           *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


// some usefull functions


float get_seconds () { // TODO rename to process_user_time
  rusage usage [1];
  getrusage (RUSAGE_SELF, usage);
  return float(usage->ru_utime.tv_sec) + float(usage->ru_utime.tv_usec) / 1000000.0;
}

void fatal_error (const char* s) {
  cerr << "Fatal error: " << s << endl;
  assert (false);
  exit (1);
}


// string/stream opereations

template <typename T>
string to_string (T f, int precision = -1) {
  ostringstream s;
  if (precision > 0) s.precision(precision);
  s << f;
  return s.str ();
}

char getc_non_space (istream& is) {
  char c;
  is.get(c);
  if (c == ' ' || c == '\t') return getc_non_space (is);
  return c;
}

bool is_all_whitespace (string s) {
  for_each (cp, s)
    if (!isspace (*cp))
      return false;
  return true;
}

void remove_empty_lines (string* s) {
  istringstream in (*s);
  ostringstream out;
  string line;
  while (getline (in, line)) {
    if (is_all_whitespace (line)) continue;
    out << line << endl;
  }
  *s = out.str ();
}

void remove_trailing_whitespace (string* str) {
  while (isspace ( *(str->end ()-1) ))
    str->resize (str->size () - 1);
}

// g++ extensions


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
