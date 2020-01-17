#ifndef AIOS_TCP_ACCEPTOR_H
#define AIOS_TCP_ACCEPTOR_H

#include "endpoint.h"
#include "io_processor.h"
#include "tcp_socket.h"
#include <functional>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace aios {
class tcp_acceptor {
public:
  tcp_acceptor(tcp_acceptor const &other) = delete;

  tcp_acceptor(io_processor &processor) noexcept;

  tcp_acceptor(io_processor &processor, aios::ip::endpoint const &ep,
               aios::system::error_code &ec) noexcept;

  tcp_acceptor(tcp_acceptor &&other) noexcept;

  tcp_acceptor &operator=(tcp_acceptor &&other) noexcept;

  ~tcp_acceptor();

  void async_accept(tcp_socket &socket,
                    std::function<void(aios::system::error_code)> &&handler);

  void async_accept(tcp_socket &socket,
                    std::function<void(aios::system::error_code)> &&handler,
                    std::chrono::seconds dur);

private:
  int fd;
  std::reference_wrapper<io_processor> processor;
};
} // namespace aios

#endif // AIOS_ACCEPTOR_H
