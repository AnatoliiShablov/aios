#include "../include/timer_processor.h"

aios::system::timer_processor::iterator
aios::system::timer_processor::add_timer(std::function<void()> &&func,
                                         std::chrono::seconds dur) {
  return timers.emplace(std::move(func), dur);
}

void aios::system::timer_processor::remove_timer(
    aios::system::timer_processor::iterator pos) {
  timers.erase(pos);
}

void aios::system::timer_processor::poll_this_moment() noexcept {
  std::chrono::high_resolution_clock::time_point tp =
      std::chrono::high_resolution_clock::now();
  for (auto it = timers.begin(); it != timers.end();) {
    if (it->try_handle(tp)) {
      it = timers.erase(it);
    } else {
      break;
    }
  }
}

void aios::system::timer_processor::poll() noexcept {
  for (auto it = timers.begin(); it != timers.end();) {
    if (it->try_handle()) {
      it = timers.erase(it);
    } else {
      break;
    }
  }
}
