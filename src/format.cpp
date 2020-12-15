#include "format.h"

#include <iomanip>
#include <sstream>
#include <string>

using std::string;

string Format::ElapsedTime(long seconds) {
  // Initialize hours, minutes, and seconds used for return:
  long hh = seconds / 3600;
  long mm = (seconds - hh * 3600) / 60;
  long ss = seconds - hh * 3600 - mm * 60;

  std::ostringstream upTimeStringStream;
  upTimeStringStream << std::setfill('0') << std::setw(2) << std::to_string(hh);
  upTimeStringStream << ":";
  upTimeStringStream << std::setfill('0') << std::setw(2) << std::to_string(mm);
  upTimeStringStream << ":";
  upTimeStringStream << std::setfill('0') << std::setw(2) << std::to_string(ss);

  return upTimeStringStream.str();
}