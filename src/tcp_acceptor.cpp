#include "../include/tcp_acceptor.h"
#include <iostream>
aios::tcp_acceptor::tcp_acceptor(io_processor &processor) noexcept
    : fd{-1}, processor{std::ref(processor)} {}

aios::tcp_acceptor::tcp_acceptor(io_processor &processor,
                                 aios::ip::endpoint const &ep,
                                 aios::system::error_code &ec) noexcept
    : fd{socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)}, processor{std::ref(
                                                               processor)} {
  if (fd < 0) {
    ec = aios::system::error_code::cant_create_socket;
    return;
  }
  sockaddr_in acceptor_addr{};
  acceptor_addr.sin_family = AF_INET;
  acceptor_addr.sin_addr.s_addr = ep.address_v4().to_network_uint();
  acceptor_addr.sin_port = ep.network_port();
  if (bind(fd, reinterpret_cast<sockaddr *>(&acceptor_addr),
           sizeof(acceptor_addr)) < 0) {
    ec = aios::system::error_code::cant_bind_socket;
    return;
  }
  if (listen(fd, 16) < 0) {
    ec = aios::system::error_code::cant_bind_socket;
    return;
  }
  processor.add_fd(fd, ec);
}

aios::tcp_acceptor::tcp_acceptor(aios::tcp_acceptor &&other) noexcept
    : fd{other.fd}, processor{other.processor} {
  other.fd = -1;
}

aios::tcp_acceptor &
aios::tcp_acceptor::operator=(aios::tcp_acceptor &&other) noexcept {
  if (fd > 0) {
    processor.get().remove_fd(fd);
    close(fd);
  }
  fd = other.fd;
  other.fd = -1;
  processor = other.processor;
  return *this;
}

aios::tcp_acceptor::~tcp_acceptor() {
  if (fd > 0) {
    processor.get().remove_fd(fd);
    close(fd);
  }
}

void aios::tcp_acceptor::async_accept(
    aios::tcp_socket &socket,
    std::function<void(aios::system::error_code)> &&handler) {
  processor.get().register_acceptor(
      fd, [&socket, handler = std::move(handler)](int new_fd,
                                                  aios::system::error_code ec) {
        if (ec == aios::system::error_code::no_errors) {
          socket.change_fd(new_fd);
        }
        handler(ec);
      });
}

void aios::tcp_acceptor::async_accept(
    aios::tcp_socket &socket,
    std::function<void(aios::system::error_code)> &&handler,
    std::chrono::seconds dur) {
  processor.get().register_acceptor(
      fd,
      [&socket, handler = std::move(handler)](int new_fd,
                                              aios::system::error_code ec) {
        if (ec == aios::system::error_code::no_errors) {
          socket.change_fd(new_fd);
        }
        handler(ec);
      },
      dur);
}
