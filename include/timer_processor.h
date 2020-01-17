#ifndef AIOS_TIMER_PROCESSOR_H
#define AIOS_TIMER_PROCESSOR_H

#include "timer.h"
#include <chrono>
#include <functional>
#include <set>

namespace aios::system {
class timer_processor {
public:
  using iterator = std::multiset<timer>::iterator;

  timer_processor() = default;

  timer_processor(timer_processor const &) = delete;

  iterator add_timer(std::function<void()> &&func, std::chrono::seconds dur);

  void remove_timer(iterator pos);

  void poll_this_moment() noexcept;

  void poll() noexcept;

private:
  std::multiset<aios::system::timer> timers;
};
} // namespace aios::system

#endif // AIOS_TIMER_PROCESSOR_H
