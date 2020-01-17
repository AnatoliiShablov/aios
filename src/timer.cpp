#include "../include/timer.h"

aios::system::timer::timer(
    std::function<void()> &&func,
    std::chrono::seconds dur) noexcept(noexcept(std::move(func)))
    : response_point{std::chrono::high_resolution_clock::now() + dur},
      handle_function{std::move(func)} {}

aios::system::timer::timer(std::function<void()> &&func,
                           std::chrono::high_resolution_clock::time_point
                               tp) noexcept(noexcept(std::move(func)))
    : response_point{tp}, handle_function{std::move(func)} {}

bool aios::system::timer::try_handle(
    std::chrono::high_resolution_clock::time_point tp) const noexcept {
  if (tp >= response_point) {
    handle_function();
    return true;
  }
  return false;
}
