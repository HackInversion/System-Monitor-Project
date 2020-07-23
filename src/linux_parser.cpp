#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  float memTotal{0};
  float memFree{0};
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "MemTotal") {
        linestream >> memTotal;
      } else if (key == "MemFree") {
        linestream >> memFree;
        break;
      }
    }
  }
  return (memTotal - memFree) / memTotal;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  long seconds{0};
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    filestream >> seconds;
  }
  return seconds;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

// TODO: Read and return the number of active jiffies for a PID
// Sums up the times at index 14-17 in the file path /proc/[pid]/stat
// Times at index 14-17 are utime, stime, ctime and cstime
long LinuxParser::ActiveJiffies(int pid) {
  string str;
  long totalClockTicks{0};
  std::ifstream filestream(kProcDirectory + "/" + to_string(pid) + "/" +
                           kStatFilename);
  if (filestream.is_open()) {
    long num = 0;
    filestream >> num >> str >> str;
    int count = 3;
    while (filestream >> num) {
      // since count is zero indexed we sum up values at index 13-16
      if (count > 12 && count < 17) {
        totalClockTicks += num;
      }
      count++;
    }
  }
  // The times are stored in clock ticks so we convert to seconds
  return totalClockTicks / sysconf(_SC_CLK_TCK);
}

// TODO: Read and return the number of active jiffies for the system
// Sums all the values(except 4th && 5th) on the first line of file path
// /proc/stat
long LinuxParser::ActiveJiffies() {
  string cpu;
  long activeJiffies{0};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    filestream >> cpu;
    int count = 0;
    long num = 0;
    while (count < 10) {
      filestream >> num;
      if (count != 3 && count != 4) {
        activeJiffies += num;
      }
      count++;
    }
  }
  return activeJiffies;
}

// TODO: Read and return the number of idle jiffies for the system
// Sums the 4th and 5th values on first line of file path /proc/stat
long LinuxParser::IdleJiffies() {
  string cpu;
  long idleJiffies{0};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    filestream >> cpu;
    int count = 0;
    long num = 0;
    while (count < 5) {
      filestream >> num;
      if (count > 2) {
        idleJiffies += num;
      }
      count++;
    }
  }
  return idleJiffies;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  int total_processes{0};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "processes") {
        linestream >> total_processes;
        break;
      }
    }
  }
  return total_processes;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  int processes{0};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "procs_running") {
        linestream >> processes;
        break;
      }
    }
  }
  return processes;
}

// TODO: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line{""};
  std::ifstream filestream(kProcDirectory + "/" + to_string(pid) + "/" +
                           kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
  }
  return line;
}

// TODO: Read and return the memory used by a process
// Returns the memory used in megabytes
string LinuxParser::Ram(int pid) {
  string line{""};
  string key;
  int ram_KB{0};
  std::ifstream filestream(kProcDirectory + "/" + to_string(pid) + "/" +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "VmSize") {
        linestream >> ram_KB;
        break;
      }
    }
  }
  return to_string(ram_KB / 1024);
}

// TODO: Read and return the user ID associated with a process
// Reads file in path /proc/[pid]/status and returns first number associated
// with the key Uid
string LinuxParser::Uid(int pid) {
  string line{""};
  string key, uid;
  std::ifstream filestream(kProcDirectory + "/" + to_string(pid) + "/" +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "Uid") {
        linestream >> uid;
        break;
      }
    }
  }
  return uid;
}

// TODO: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string uid = Uid(pid);
  string line, user_name, user_id;

  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::replace(line.begin(), line.end(), 'x', ' ');
      std::istringstream linestream(line);
      linestream >> user_name >> user_id;
      if (user_id == uid) {
        break;
      }
    }
  }
  return user_name;
}

// TODO: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string str;
  long starttime{0};
  std::ifstream filestream(kProcDirectory + "/" + to_string(pid) + "/" +
                           kStatFilename);
  if (filestream.is_open()) {
    long num = 0;
    filestream >> num >> str >> str;
    int count = 3;
    while (filestream >> num) {
      if (count == 21) {
        starttime = num;
        break;
      }
      count++;
    }
  }

  return UpTime() - (starttime / sysconf(_SC_CLK_TCK));
}