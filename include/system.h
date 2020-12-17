#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

class System {
 public:
  Processor& Cpu();
  std::vector<Process>& Processes();
  static std::string OperatingSystem();
  static std::string Kernel();
  static float MemoryUtilization();
  static int TotalProcesses();
  static int RunningProcesses();
  static long UpTime();

 private:
  Processor cpu_ = {};
  std::vector<Process> processes_ = {};
};

#endif