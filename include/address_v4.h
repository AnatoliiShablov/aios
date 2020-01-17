#ifndef AIOS_ADDRESS_V4_H
#define AIOS_ADDRESS_V4_H

#include <array>
#include <cstdint>

#include "endian.h"
#include "error_code.h"

namespace aios::ip {
class address_v4 {
public:
  using uint_type = uint32_t;
  using bytes_type = std::array<uint8_t, 4>;

  constexpr address_v4() noexcept : data_{0} {}

  constexpr explicit address_v4(uint_type addr) noexcept
      : data_{aios::system::host_to_network(addr)} {}

  constexpr explicit address_v4(bytes_type const &bytes) noexcept
      : data_{aios::system::host_to_network(
            static_cast<uint32_t>(bytes[0]) << UINT32_C(24) |
            static_cast<uint32_t>(bytes[1]) << UINT32_C(16) |
            static_cast<uint32_t>(bytes[2]) << UINT32_C(8) |
            static_cast<uint32_t>(bytes[3]) << UINT32_C(0))} {}

  constexpr address_v4(address_v4 const &other) noexcept = default;

  constexpr explicit address_v4(std::string_view str) noexcept : data_{0} {
    if (str == "localhost") {
      *this = loopback();
      return;
    }
    std::array<uint8_t, 4> bytes{UINT8_C(0), UINT8_C(0), UINT8_C(0),
                                 UINT8_C(0)};
    uint32_t i{0};
    uint32_t tmp{0};
    for (char c : str) {
      if (tmp > UINT32_C(255)) {
        return;
      }
      if (c == '.') {
        bytes[i] = static_cast<uint8_t>(tmp);
        if (++i > UINT32_C(3)) {
          return;
        }
      } else if (c > '0' && c < '9') {
        tmp *= UINT32_C(10);
        tmp += static_cast<uint32_t>(c - '0');
      } else {
        return;
      }
    }
    if (tmp > UINT32_C(255)) {
      return;
    } else {
      bytes[3] = static_cast<uint8_t>(tmp);
    }
    *this = address_v4{bytes};
  }

  address_v4(std::string_view str, aios::system::error_code &ec) noexcept;

  constexpr address_v4 &operator=(address_v4 const &other) noexcept = default;

  [[nodiscard]] constexpr bool is_loopback() const noexcept {
    return (aios::system::network_to_host(data_) >= UINT32_C(0x7f000001)) &&
           (aios::system::network_to_host(data_) <= UINT32_C(0x7fffffff));
  }

  [[nodiscard]] constexpr bool is_multicast() const noexcept {
    return aios::system::network_to_host(data_) & UINT32_C(0xE0000000);
  }

  [[nodiscard]] constexpr bool is_unspecified() const noexcept {
    return data_ == UINT32_C(0);
  }

  [[nodiscard]] constexpr bytes_type to_bytes() const noexcept {
    uint32_t host_variant = aios::system::network_to_host(data_);
    return bytes_type{static_cast<uint8_t>(host_variant >> UINT32_C(24)),
                      static_cast<uint8_t>(host_variant >> UINT32_C(16)),
                      static_cast<uint8_t>(host_variant >> UINT32_C(8)),
                      static_cast<uint8_t>(host_variant >> UINT32_C(0))};
  }

  [[nodiscard]] constexpr uint_type to_host_uint() const noexcept {
    return aios::system::network_to_host(data_);
  }

  [[nodiscard]] constexpr uint_type to_network_uint() const noexcept {
    return data_;
  }

  [[nodiscard]] static constexpr address_v4 any() noexcept {
    return address_v4{};
  }

  [[nodiscard]] static constexpr address_v4 loopback() noexcept {
    return address_v4{UINT32_C(0x7f000001)};
  }

  [[nodiscard]] friend constexpr bool
  operator==(address_v4 const &lhs, address_v4 const &rhs) noexcept {
    return lhs.data_ == rhs.data_;
  }

  [[nodiscard]] friend constexpr bool
  operator!=(address_v4 const &lhs, address_v4 const &rhs) noexcept {
    return lhs.data_ != rhs.data_;
  }

  [[nodiscard]] std::string to_string();

private:
  uint_type data_;
};
} // namespace aios::ip

#endif // ADDRESS_V4_H
