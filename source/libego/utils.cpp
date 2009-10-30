//
// Copyright 2006 and onwards, Lukasz Lew
//

// TODO move such stuff to porting.h (or somwhere)
#if defined(_MSC_VER) || defined(__MINGW32__)
#include <windows.h>
#else 
#include <sys/resource.h>
#endif

#include <iostream>
#include <cassert>
#include <cstdlib>

#include "utils.h"

// some usefull functions

// TODO use this to port rusage to windows/mingw
// http://octave.sourceforge.net/doxygen/html/getrusage_8cc-source.html
// http://stackoverflow.com/questions/771944/how-to-measure-user-time-used-by-process-on-windows
// http://www.ginac.de/pipermail/ginac-list/2006-July/000861.html

float process_user_time () {

// TODO ifdef POSIX ?
#ifndef WIN32

  rusage usage [1];
  getrusage (RUSAGE_SELF, usage);
  return 
    float(usage->ru_utime.tv_sec) +
    float(usage->ru_utime.tv_usec) / 1000000.0;

#elif defined(_MSC_VER) || defined(__MINGW32__)

  union { FILETIME t; __int64 i64; } start, exit, kernel, user;
  if (GetProcessTimes(GetCurrentProcess(), &start.t, &exit.t, &kernel.t, &user.t)) {
    __int64 userMicro = user.i64 / 10U; //(*((__int64*) &user.t)) / 10U;
    return (float)((double)userMicro / 1000000.0f);
  } else {
    return 0;
  }

#else

 return 0;

#endif

}

void fatal_error (const char* s) {
  cerr << "Fatal error: " << s << endl;
  assert (false);
  exit (1);
}


// string/stream opereations

char getc_non_space (istream& is) {
  char c;
  is.get(c);
  if (c == ' ' || c == '\t') return getc_non_space (is);
  return c;
}

bool is_all_whitespace (const string& s) {
  for(string::const_iterator cp = s.begin(); cp != s.end(); cp++)
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

SS::operator std::string () const {
  return buffer_.str();
}
