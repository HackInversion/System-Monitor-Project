#include "format.h"

#include <string>

using std::string;

// Returns elasped time
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
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