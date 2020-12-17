#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  double value;
  double memTotal{0};
  double memFree{0};
  double memAvailable{0};
  double buffers{0};
  float percentageUtilized{0};
  std::ifstream fileStream(kProcDirectory + kMeminfoFilename);
  if (fileStream.is_open()) {
    while (std::getline(fileStream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream lineStream(line);
      lineStream >> key >> value;
      if (key == "MemTotal") {
        memTotal = value;
      }
      if (key == "MemFree") {
        memFree = value;
      }
      if (key == "MemAvailable") {
        memAvailable = value;
      }
      if (key == "Buffers") {
        buffers = value;
      }
      if (memTotal * memFree * memAvailable * buffers != 0) {
        // We should get all the values we need now, exit the while loop:
        break;
      }
    }
  }
  if (memTotal != 0) {
    percentageUtilized =
        static_cast<float>((memTotal - memAvailable) / memTotal);
  }

  return percentageUtilized;
}

long LinuxParser::UpTime() {
  long uptimeSeconds{0};
  string line;
  std::ifstream fileStream(kProcDirectory + kUptimeFilename);
  if (fileStream.is_open()) {
    std::getline(fileStream, line);
    std::istringstream lineStream(line);
    lineStream >> uptimeSeconds;
  }
  return uptimeSeconds;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid [[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

vector<string> LinuxParser::CpuUtilization() {
  vector<string> retVec;
  string line, value;
  std::ifstream fileStream(kProcDirectory + kStatFilename);
  if (fileStream.is_open()) {
    // Only the first line is what we need:
    std::getline(fileStream, line);
    std::istringstream lineStream(line);

    while (lineStream >> value) {
      if (value != "cpu") {
        retVec.push_back(value);
      }
    }
  }
  if (retVec.size() != 10) {
    std::cerr << "Error parsing file " << kStatFilename
              << "for CPU utilization info! Only got " << retVec.size()
              << " number of values!\n";
    for (auto const& item : retVec) {
      std::cerr << item << "\n";
    }
  }

  return retVec;
}

vector<string> LinuxParser::CpuUtilization(int pid) {
  std::vector<std::string> retVec;
  string line, value;
  std::string filePath = processBaseFilePath(pid);
  filePath.append(kStatFilename);
  std::ifstream fileStream(filePath);
  if (fileStream.is_open()) {
    std::getline(fileStream, line);
    std::istringstream lineStream(line);
    for (auto i = 0; i < 22; i++) {
      lineStream >> value;
      int index = i + 1;
      if (index == 14 || index == 15 || index == 16 || index == 17 ||
          index == 22) {
        retVec.push_back(value);
      }
    }
  } else {
    std::cerr << "Error opening file at " << filePath << "\n";
  }

  return retVec;
}

int LinuxParser::TotalProcesses() {
  string line;
  string key;
  int value;
  std::ifstream fileStream(kProcDirectory + kStatFilename);
  if (fileStream.is_open()) {
    while (std::getline(fileStream, line)) {
      std::istringstream lineStream(line);
      lineStream >> key >> value;

      if (key == "processes") {
        return value;
      }
    }
  }
  return 0;
}

int LinuxParser::RunningProcesses() {
  string line;
  string key;
  int value;
  std::ifstream fileStream(kProcDirectory + kStatFilename);
  if (fileStream.is_open()) {
    while (std::getline(fileStream, line)) {
      std::istringstream lineStream(line);
      lineStream >> key >> value;

      if (key == "procs_running") {
        return value;
      }
    }
  }
  return 0;
}

string LinuxParser::processBaseFilePath(int pid) {
  string filePath;
  filePath.append(kProcDirectory);
  //  filePath.append("/");
  filePath.append(std::to_string(pid));

  return filePath;
}

string LinuxParser::Command(int pid) {
  string line;
  string filePath = processBaseFilePath(pid);
  filePath.append(kCmdlineFilename);

  std::ifstream fileStream(filePath);
  if (fileStream.is_open()) {
    std::getline(fileStream, line);
  } else {
    std::cerr << "Could not open file at " << filePath << "\n";
  }

  return line;
}

// TODO: Read and return the memory used by a process - double check after
// sorting
string LinuxParser::Ram(int pid) {
  string line, key;
  int value{0};
  string filePath = processBaseFilePath(pid);
  filePath.append(kStatusFilename);

  std::ifstream fileStream(filePath);
  if (fileStream.is_open()) {
    while (std::getline(fileStream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream lineStream(line);
      lineStream >> key >> value;

      if (key == "VmSize") {
        return std::to_string(value / kMbToKbRatio);
      }
    }

  } else {
    std::cerr << "Error opening file at " << filePath << "\n";
  }
  return string();
}

string LinuxParser::Uid(int pid) {
  string line, key, value;

  string baseFilePath = processBaseFilePath(pid);
  string uidFilePath = baseFilePath.append(kStatusFilename);
  std::ifstream uidFileStream(uidFilePath);
  if (uidFileStream.is_open()) {
    while (std::getline(uidFileStream, line)) {
      std::istringstream lineStream(line);
      lineStream >> key >> value;
      if (key == "Uid:") {
        break;
      }
    }
  } else {
    std::cerr << "Could not open file at " << uidFilePath << "\n";
  }

  return value;
}

string LinuxParser::User(int pid) {
  string userName{"Undetermined"};
  string uid = Uid(pid);

  string line, uidInFile;
  std::ifstream fileStream(kPasswordPath);
  if (fileStream.is_open()) {
    while (std::getline(fileStream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::replace(line.begin(), line.end(), 'x', ' ');
      std::istringstream lineStream(line);
      lineStream >> userName >> uidInFile;
      if (uidInFile == uid) {
        break;
      }
    }
  } else {
    std::cerr << "Could not open file at " << kPasswordPath << "\n";
  }

  return userName;
}

// TODO: Read and return the uptime of a process - double check after sorting
long LinuxParser::UpTime(int pid) {
  long upTimeValue{0};
  string line;
  std::vector<string> valueStrs(kIndexOfProcessUpTimeStamp);
  string filePath = processBaseFilePath(pid);
  filePath.append(kStatusFilename);

  std::ifstream fileStream(filePath);
  if (fileStream.is_open()) {
    std::getline(fileStream, line);
    std::istringstream lineStream(line);

    for (auto& valueStr : valueStrs) {
      lineStream >> valueStr;
    }

  } else {
    std::cerr << "Error opening file at " << filePath << "\n";
  }
  const char* pUpTimeValueStr =
      valueStrs[kIndexOfProcessUpTimeStamp - 1].c_str();
  char* pUpTimeValueStrEnd;
  upTimeValue =
      UpTime() - std::strtol(pUpTimeValueStr, &pUpTimeValueStrEnd, 10) /
                     sysconf(_SC_CLK_TCK);

  return upTimeValue;
}