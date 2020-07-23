#include "processor.h"

#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
// Used the suggestion from this StackOverflow post for calculation
// https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
// Dynamically calculates the Cpu utilization
float Processor::Utilization() {
  // get current data
  long activeJiffies = LinuxParser::ActiveJiffies();
  long idleJiffies = LinuxParser::IdleJiffies();

  long curr_total = activeJiffies + idleJiffies;
  long prev_total = prevActiveJiffies + prevIdleJiffies;

  long total_delta = curr_total - prev_total;
  long idle_delta = idleJiffies - prevIdleJiffies;

  //store the data for the next calculation
  prevActiveJiffies = activeJiffies;
  prevIdleJiffies = idleJiffies;

  return (float)(total_delta - idle_delta) / (float)total_delta;
}