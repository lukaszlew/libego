#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>
#include <fstream>
#include <iostream>

class Logger {
public:
  Logger ();
  ~Logger ();

  void SetFileNamePrefix (const std::string&);
  void SetFileNameSuffix (const std::string&);
  void EnableLogging (const std::string& log_directory);

  void NewLog ();

  template <class T>
  void LogLine (const T& val);

  // TODO make it private
  bool is_active;
  std::string log_directory;

private:
  std::ofstream out;
  std::string prefix;
  std::string suffix;
};

// -----------------------------------------------------------------------------
// implementation

template <class T>
void Logger::LogLine (const T& val) {
  if (!is_active || !out.is_open()) return;
  out << val << std::endl << std::flush;
}

#endif
