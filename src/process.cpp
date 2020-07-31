#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
// Used the suggestion from this stackOverflow post to calculate usage
// https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
// Gives the avg Cpu usage since the process started
float Process::CpuUtilization() const {
  // we divide the total work time spent by the process over the process uptime;
  return (float)LinuxParser::ActiveJiffies(pid_) /
         (float)(LinuxParser::UpTime(pid_));
}

// Return the command that generated this process
const string& Process::Command() const { return command_; }

// Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// Return the user (name) that generated this process
const string& Process::User() const { return user_; }

// Return the age of this process (in seconds)
long int Process::UpTime() const { return LinuxParser::UpTime(pid_); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& other) const {
  return this->CpuUtilization() > other.CpuUtilization();
}