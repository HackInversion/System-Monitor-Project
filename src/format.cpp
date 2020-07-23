#include "format.h"

#include <string>

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  long hours, mins;
  hours = seconds / 3600;
  mins = seconds % 3600 / 60;
  seconds = seconds % 3600 % 60;
  string time = ((hours / 10) >= 1 ? "" : "0") + std::to_string(hours) + ":" +
                ((mins / 10) >= 1 ? "" : "0") + std::to_string(mins) + ":" +
                ((seconds / 10) >= 1 ? "" : "0") + std::to_string(seconds);
  return time;
}