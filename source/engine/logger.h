#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>

namespace bdate = boost::gregorian;
namespace btime = boost::posix_time;
namespace bfile = boost::filesystem;

class Logger {
public:

  Logger () {
    is_active = false;
    log_directory = "log";
  }

  ~Logger () {
    if (out.is_open()) out.close();
  }

  void NewLog () {
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
    string filename = path + time + ".log.gtp";

    try {
      bfile::create_directories (path);
      if (bfile::exists (filename)) {
        uint i = 1;
        while (!bfile::exists (filename + ToString(i))) i += 1;
        filename = filename + ToString(i);
      }
    } catch (std::exception e) { return; }

    out.open (filename.c_str(), ios_base::app);
  }

  template <class T>
  void LogLine (const T& val) {
    if (!is_active && out.is_open()) return;
    out << val << endl << flush;
  }

  bool   is_active;
  string log_directory;

private:
  ofstream out;
};
