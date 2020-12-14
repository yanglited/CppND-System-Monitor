#include <string>

#include "format.h"

using std::string;

string Format::ElapsedTime(long seconds) {

  // Initialize hours, minutes, and seconds used for return:
  long hh = seconds / 3600;;
  long mm = (seconds - hh * 3600) / 60;
  long ss = seconds - hh * 3600 - mm * 60;

  std::string upTimeString;
  upTimeString.append(std::to_string(hh));
  upTimeString.append(":");
  upTimeString.append(std::to_string(mm));
  upTimeString.append(":");
  upTimeString.append(std::to_string(ss));

  //TODO need to pad a zero when single digit numbers appear.

  return upTimeString; }