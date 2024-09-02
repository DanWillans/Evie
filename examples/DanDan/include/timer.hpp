#ifndef EXAMPLES_DANDAN_INCLUDE_TIMER_HPP_
#define EXAMPLES_DANDAN_INCLUDE_TIMER_HPP_

#include <chrono>
#include <evie/logging.h>

class Timer
{
public:
  Timer(const std::string& timer_name) : name_(timer_name) { start_ = std::chrono::high_resolution_clock::now(); }

  void PrintTime()
  {
    const auto now = std::chrono::high_resolution_clock::now();
    const auto time_us = std::chrono::duration_cast<std::chrono::microseconds>(now - start_);
    const auto time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_us);
    APP_TRACE("{} timer: {} us -  {} ms", name_, time_us.count(), time_ms.count());
  }

private:
  std::string name_;
  std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};

#endif// !EXAMPLES_DANDAN_INCLUDE_TIMER_HPP_