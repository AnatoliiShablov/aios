#include "../include/tcp_socket.h"

aios::tcp_socket::tcp_socket(io_processor &processor, int fd) noexcept
    : fd{-1}, processor{std::ref(processor)} {
  if (fd > 0) {
    processor.add_fd(fd);
  }
}

aios::tcp_socket::tcp_socket(aios::tcp_socket &&other) noexcept
    : fd{other.fd}, processor{other.processor} {
  other.fd = -1;
}

aios::tcp_socket &
aios::tcp_socket::operator=(aios::tcp_socket &&other) noexcept {
  if (fd > 0) {
    processor.get().remove_fd(fd);
    close(fd);
  }
  fd = other.fd;
  other.fd = -1;
  processor = other.processor;
  return *this;
}

void aios::tcp_socket::change_fd(int new_fd) noexcept {
  if (fd > 0) {
    processor.get().remove_fd(fd);
    close(fd);
  }
  if (new_fd > 0) {
    processor.get().add_fd(new_fd);
  }
  fd = new_fd;
}

void aios::tcp_socket::async_write_some(
    char const *data, size_t length,
    std::function<void(size_t, aios::system::error_code)> &&handler) const {
  processor.get().register_writer(fd, data, length, std::move(handler));
}

void aios::tcp_socket::async_read_some(
    char *data, size_t length,
    std::function<void(size_t, aios::system::error_code)> &&handler) const {
  processor.get().register_reader(fd, data, length, std::move(handler));
}

void aios::tcp_socket::async_write_some(
    char const *data, size_t length,
    std::function<void(size_t, aios::system::error_code)> &&handler,
    std::chrono::seconds dur) const {
  processor.get().register_writer(fd, data, length, std::move(handler), dur);
}

void aios::tcp_socket::async_read_some(
    char *data, size_t length,
    std::function<void(size_t, aios::system::error_code)> &&handler,
    std::chrono::seconds dur) const {
  processor.get().register_reader(fd, data, length, std::move(handler), dur);
}

aios::tcp_socket::~tcp_socket() {
  if (fd > 0) {
    processor.get().remove_fd(fd);
    close(fd);
  }
}
