#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace bdate = boost::gregorian;
namespace btime = boost::posix_time;


class Logger {
public:

  Logger () {
    is_active = false;
  }

  void NewLog (const string& prefix = "") {
    btime::ptime now = btime::microsec_clock::local_time ();
    string date = bdate::to_iso_extended_string (now.date());
    string time = btime::to_simple_string (now.time_of_day());
    filename = prefix + date + "T" + time;
  }

  template <class T>
  void LogLine (const T& val) {
    if (!is_active) return;
    ofstream out (filename.c_str(), ios_base::app);
    if (!out) return;
    out << val << endl;
    out.close ();
  }

  bool is_active;
private:
  string filename;
};
