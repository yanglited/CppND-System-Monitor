#include "processor.h"

#include <string>
#include <vector>

#include "linux_parser.h"

/* Calculation strategy:

References:
 https://www.kernel.org/doc/html/latest/filesystems/proc.html?highlight=proc%20stat#miscellaneous-kernel-statistics-in-proc-stat
 https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux

PrevIdle = previdle + previowait
Idle = idle + iowait

PrevNonIdle = prevuser + prevnice + prevsystem + previrq + prevsoftirq +
prevsteal NonIdle = user + nice + system + irq + softirq + steal

PrevTotal = PrevIdle + PrevNonIdle
Total = Idle + NonIdle

# differentiate: actual value minus the previous one
totald = Total - PrevTotal
idled = Idle - PrevIdle

CPU_Percentage = (totald - idled)/totald
 */

float Processor::Utilization() {
  std::vector<std::string> strVec = LinuxParser::CpuUtilization();
  if (strVec.size() == 10) {
    float user_read = std::stof(strVec[0]);
    float nice_read = std::stof(strVec[1]);
    float system_read = std::stof(strVec[2]);
    float idle_read = std::stof(strVec[3]);
    float iowait_read = std::stof(strVec[4]);
    float irq_read = std::stof(strVec[5]);
    float softirq_read = std::stof(strVec[6]);
    float steal_read = std::stof(strVec[7]);

    float idle = idle_read + iowait_read;
    float nonIdel = user_read + nice_read + system_read + irq_read +
                    softirq_read + steal_read;
    float total = idle + nonIdel;

    float total_diff = total - prevTotal;
    float idle_diff = idle - prevIdle;

    prevTotal = total;
    prevIdle = idle;

    return (total_diff - idle_diff) / total_diff;
  }

  return 0.0f;
}