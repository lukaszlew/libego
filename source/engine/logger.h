#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>
#include <fstream>
#include <iostream>

class Logger {
public:

  Logger ();
  ~Logger ();
  void NewLog ();

  template <class T>
  void LogLine (const T& val);

  bool is_active;
  std::string log_directory;

private:
  std::ofstream out;
};

// -----------------------------------------------------------------------------
// implementation

template <class T>
void Logger::LogLine (const T& val) {
  if (!is_active && out.is_open()) return;
  out << val << std::endl << std::flush;
}

#endif
