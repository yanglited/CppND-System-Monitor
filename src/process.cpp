#include "process.h"

#include <unistd.h>

#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

int Process::Pid() const { return pid_; }

float Process::CpuUtilization() {
  std::vector<std::string> strVec = LinuxParser::CpuUtilization(Pid());
  float utime_sec =
      std::stof(strVec[0]) / static_cast<float>(sysconf(_SC_CLK_TCK));
  float stime_sec =
      std::stof(strVec[1]) / static_cast<float>(sysconf(_SC_CLK_TCK));
  float cutime_sec =
      std::stof(strVec[2]) / static_cast<float>(sysconf(_SC_CLK_TCK));
  float cstime_sec =
      std::stof(strVec[3]) / static_cast<float>(sysconf(_SC_CLK_TCK));
  float starttime_sec =
      std::stof(strVec[4]) / static_cast<float>(sysconf(_SC_CLK_TCK));

  // Reference:
  // https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
  float total_time_sec = utime_sec + stime_sec + cutime_sec + cstime_sec;
  float elapsed_time_sec =
      static_cast<float>(LinuxParser::UpTime()) - starttime_sec;

  cpuUtilization_ = 100.0f * total_time_sec / elapsed_time_sec;
  return cpuUtilization_;
}

std::string Process::Command() const { return LinuxParser::Command(Pid()); }

std::string Process::Ram() const { return LinuxParser::Ram(Pid()); }

std::string Process::User() const { return LinuxParser::User(Pid()); }

long int Process::UpTime() const { return LinuxParser::UpTime(Pid()); }

bool Process::operator>(Process const& a) const {
  return cpuUtilization_ < a.cpuUtilization_;
}