#ifndef ENDIAN_H
#define ENDIAN_H

#include <cstdint>

namespace aios {
namespace system {
#ifdef BIG_ENDIAN
enum class endian { little = 0, big = 1, native = 1 };
#else
enum class endian { little = 0, big = 1, native = 0 };
#endif

[[nodiscard]] constexpr uint8_t host_to_network(uint8_t number) noexcept {
  return static_cast<uint8_t>(number);
}
[[nodiscard]] constexpr uint8_t network_to_host(uint8_t number) noexcept {
  return host_to_network(number);
}

[[nodiscard]] constexpr uint16_t host_to_network(uint16_t number) noexcept {
#ifdef BIG_ENDIAN
  return number;
#else
  return static_cast<uint16_t>(number >> UINT16_C(0) << UINT16_C(8) |
                               number >> UINT16_C(8) << UINT16_C(0));
#endif
}
[[nodiscard]] constexpr uint16_t network_to_host(uint16_t number) noexcept {
  return host_to_network(number);
}

[[nodiscard]] constexpr uint32_t host_to_network(uint32_t number) noexcept {
#ifdef BIG_ENDIAN
  return number;
#else
  return static_cast<uint32_t>(number >> UINT32_C(0) << UINT32_C(24) |
                               number >> UINT32_C(8) << UINT32_C(16) |
                               number >> UINT32_C(16) << UINT32_C(8) |
                               number >> UINT32_C(24) << UINT32_C(0));
#endif
}
[[nodiscard]] constexpr uint32_t network_to_host(uint32_t number) noexcept {
  return host_to_network(number);
}

#ifdef UINT64_MAX
[[nodiscard]] constexpr uint64_t host_to_network(uint64_t number) noexcept {
#ifdef BIG_ENDIAN
  return number;
#else
  return static_cast<uint64_t>(number >> UINT64_C(0) << UINT64_C(56) |
                               number >> UINT64_C(8) << UINT64_C(48) |
                               number >> UINT64_C(16) << UINT64_C(40) |
                               number >> UINT64_C(24) << UINT64_C(32) |
                               number >> UINT64_C(32) << UINT64_C(24) |
                               number >> UINT64_C(40) << UINT64_C(16) |
                               number >> UINT64_C(48) << UINT64_C(8) |
                               number >> UINT64_C(56) << UINT64_C(0));
#endif
}
[[nodiscard]] constexpr uint64_t network_to_host(uint64_t number) noexcept {
  return host_to_network(number);
}
#endif

} // namespace system
} // namespace aios

#endif // ENDIAN_H
