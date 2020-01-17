#ifndef AIOS_AIOS_TIMER_H
#define AIOS_AIOS_TIMER_H

#include <chrono>
#include <functional>

namespace aios::system {
class timer {
public:
  timer(timer const &) = delete;

  timer(std::function<void()> &&func,
        std::chrono::seconds dur) noexcept(noexcept(std::move(func)));

  timer(std::function<void()> &&func,
        std::chrono::high_resolution_clock::time_point
            tp) noexcept(noexcept(std::move(func)));

  [[nodiscard]] bool
  try_handle(std::chrono::high_resolution_clock::time_point tp =
                 std::chrono::high_resolution_clock::now()) const noexcept;

  friend bool operator<(timer const &lhs, timer const &rhs) noexcept {
    return lhs.response_point < rhs.response_point;
  }

private:
  std::chrono::high_resolution_clock::time_point response_point;
  std::function<void()> handle_function;
};
} // namespace aios::system

#endif // AIOS_TIMER_H
