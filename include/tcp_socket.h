#ifndef AIOS_TCP_SOCKET_H
#define AIOS_TCP_SOCKET_H

#include "error_code.h"
#include "io_processor.h"
#include <functional>
#include <unistd.h>

namespace aios {
class tcp_socket {
public:
  tcp_socket(tcp_socket const &other) = delete;

  explicit tcp_socket(io_processor &processor, int fd = -1) noexcept;

  tcp_socket(tcp_socket &&other) noexcept;

  tcp_socket &operator=(tcp_socket &&other) noexcept;

  void change_fd(int new_fd) noexcept;

  void async_write_some(
      char const *data, size_t length,
      std::function<void(size_t, aios::system::error_code)> &&handler) const;

  void async_read_some(
      char *data, size_t length,
      std::function<void(size_t, aios::system::error_code)> &&handler) const;

  void async_write_some(
      char const *data, size_t length,
      std::function<void(size_t, aios::system::error_code)> &&handler,
      std::chrono::seconds dur) const;

  void async_read_some(
      char *data, size_t length,
      std::function<void(size_t, aios::system::error_code)> &&handler,
      std::chrono::seconds dur) const;

  ~tcp_socket();

private:
  int fd;
  std::reference_wrapper<io_processor> processor;
};
} // namespace aios

#endif // AIOS_SOCKET_H
