/*
  Performance tracking utilities
*/

#ifndef PERF_H
#define PERF_H

#include <chrono>

using Timestamp = std::chrono::high_resolution_clock::time_point;
using Duration = std::chrono::duration<double>;

class Perf {
private:
  Timestamp startTimestamp;
  Timestamp endTimestamp;

public:
  void start() { startTimestamp = std::chrono::high_resolution_clock::now(); }

  double timeSpent() {
    Timestamp now = std::chrono::high_resolution_clock::now();
    return Duration(now - startTimestamp).count();
  }
};

#endif