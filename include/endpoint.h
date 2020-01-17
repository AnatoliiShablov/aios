#ifndef AIOS_ENDPOINT_H
#define AIOS_ENDPOINT_H

#include "address_v4.h"
#include "endian.h"

namespace aios::ip {
class endpoint {
public:
  using address_t = aios::ip::address_v4;
  using port_t = uint16_t;

  constexpr endpoint(address_t const &addr, port_t port) noexcept
      : address{addr}, port{aios::system::host_to_network(port)} {}

  [[nodiscard]] constexpr port_t host_port() const noexcept {
    return aios::system::network_to_host(port);
  }

  [[nodiscard]] constexpr port_t network_port() const noexcept { return port; }

  [[nodiscard]] constexpr address_t address_v4() const noexcept {
    return address;
  }

private:
  address_t address;
  port_t port;
};
} // namespace aios::ip

#endif // AIOS_ENDPOINT_H
