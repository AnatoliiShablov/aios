#ifndef AIOS_ENDIAN_H
#define AIOS_ENDIAN_H

#include <cstdint>

namespace aios::system {
#ifdef AIOS_BIG_ENDIAN
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
#ifdef AIOS_BIG_ENDIAN
  return number;
#else
  return static_cast<uint16_t>((number & UINT16_C(0x00ff)) << UINT16_C(8) |
                               (number & UINT16_C(0xff00)) >> UINT16_C(8));
#endif
}

[[nodiscard]] constexpr uint16_t network_to_host(uint16_t number) noexcept {
  return host_to_network(number);
}

[[nodiscard]] constexpr uint32_t host_to_network(uint32_t number) noexcept {
#ifdef AIOS_BIG_ENDIAN
  return number;
#else
  return static_cast<uint32_t>((number & UINT32_C(0x000000ff)) << UINT32_C(24) |
                               (number & UINT32_C(0x0000ff00)) << UINT32_C(8) |
                               (number & UINT32_C(0x00ff0000)) >> UINT32_C(8) |
                               (number & UINT32_C(0xff000000)) >> UINT32_C(24));
#endif
}

[[nodiscard]] constexpr uint32_t network_to_host(uint32_t number) noexcept {
  return host_to_network(number);
}

#ifdef UINT64_MAX

[[nodiscard]] constexpr uint64_t host_to_network(uint64_t number) noexcept {
#ifdef AIOS_BIG_ENDIAN
  return number;
#else
  return static_cast<uint64_t>(
      (number & UINT64_C(0x00000000000000ff)) << UINT64_C(56) |
      (number & UINT64_C(0x000000000000ff00)) << UINT64_C(40) |
      (number & UINT64_C(0x0000000000ff0000)) << UINT64_C(24) |
      (number & UINT64_C(0x00000000ff000000)) << UINT64_C(8) |
      (number & UINT64_C(0x000000ff00000000)) >> UINT64_C(8) |
      (number & UINT64_C(0x0000ff0000000000)) >> UINT64_C(24) |
      (number & UINT64_C(0x00ff000000000000)) >> UINT64_C(40) |
      (number & UINT64_C(0xff00000000000000)) >> UINT64_C(56));
#endif
}

[[nodiscard]] constexpr uint64_t network_to_host(uint64_t number) noexcept {
  return host_to_network(number);
}

#endif

} // namespace aios::system

#endif // ENDIAN_H
