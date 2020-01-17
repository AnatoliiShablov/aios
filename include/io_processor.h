#ifndef AIOS_IO_PROCESSOR_H
#define AIOS_IO_PROCESSOR_H

#include "address_v4.h"
#include "endpoint.h"
#include "error_code.h"
#include "timer_processor.h"
#include <functional>
#include <mutex>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>

namespace aios {
class io_processor {
public:
  io_processor() noexcept;

  io_processor(aios::system::error_code &ec) noexcept;

  void add_fd(int fd, aios::system::error_code &ec) const noexcept;

  void add_fd(int fd) const noexcept;

  void remove_fd(int fd, aios::system::error_code &ec) noexcept;

  void remove_fd(int fd) noexcept;

  void register_acceptor(
      int fd, std::function<void(int, aios::system::error_code)> &&handler);

  void register_acceptor(
      int fd, std::function<void(int, aios::system::error_code)> &&handler,
      std::chrono::seconds dur);

  void register_reader(
      int fd, char *data, size_t length,
      std::function<void(size_t, aios::system::error_code)> &&handler);

  void register_reader(
      int fd, char *data, size_t length,
      std::function<void(size_t, aios::system::error_code)> &&handler,
      std::chrono::seconds dur);

  void register_writer(
      int fd, char const *data, size_t length,
      std::function<void(size_t, aios::system::error_code)> &&handler);

  void register_writer(
      int fd, char const *data, size_t length,
      std::function<void(size_t, aios::system::error_code)> &&handler,
      std::chrono::seconds dur);

  void unregister_acceptor(int fd);

  void unregister_reader(int fd);

  void unregister_writer(int fd);

  void poll();

private:
  struct r_struct {
    char *data;
    size_t length;
    std::function<void(size_t, aios::system::error_code)> handler;
    std::optional<aios::system::timer_processor::iterator> waiter;
  };

  struct w_struct {
    char const *data;
    size_t length;
    std::function<void(size_t, aios::system::error_code)> handler;
    std::optional<aios::system::timer_processor::iterator> waiter;
  };

  struct l_struct {
    std::function<void(int, aios::system::error_code)> handler;
    std::optional<aios::system::timer_processor::iterator> waiter;
  };
  static constexpr size_t max_events = 64;
  int efd;
  aios::system::timer_processor time_proc;
  std::unordered_map<int, r_struct> readers;
  std::unordered_map<int, w_struct> writers;
  std::unordered_map<int, l_struct> acceptors;
};
} // namespace aios

#endif // AIOS_IO_PROCESSOR_H
