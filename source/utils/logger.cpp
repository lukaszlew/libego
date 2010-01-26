#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>

#include "to_string.hpp"

#include "logger.hpp"

namespace bdate = boost::gregorian;
namespace btime = boost::posix_time;
namespace bfile = boost::filesystem;

using namespace std;

Logger::Logger () {
  is_active = false;
  log_directory = "log";
  prefix = "";
  suffix = ".log";
}

Logger::~Logger () {
  if (out.is_open()) out.close();
}

void Logger::SetFileNamePrefix (const string& s) {
  prefix = s;
}

void Logger::SetFileNameSuffix (const string& s) {
  suffix = s;
}

void Logger::EnableLogging (const string& log_directory) {
  is_active = true;
  this->log_directory = log_directory;
}

void Logger::NewLog () {
  if (out.is_open()) out.close();
  if (!is_active) return;

  btime::ptime now = btime::second_clock::local_time ();
  string date = bdate::to_iso_extended_string (now.date());
  string time = btime::to_simple_string (now.time_of_day());

  string path;
  path += log_directory;
  if (path != "" && *(path.end()-1) != '/') path += "/";
  path += date;
  path += "/";
  string filename = path + prefix + time + suffix;

  try {
    bfile::create_directories (path);
    if (bfile::exists (filename)) {
      uint i = 1;
      while (bfile::exists (filename + ToString(i))) i += 1;
      filename = filename + ToString(i);
    }
  } catch (std::exception e) { return; }

  out.open (filename.c_str(), ios_base::app);
}
