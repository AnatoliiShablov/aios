#include "../include/address_v4.h"

aios::ip::address_v4::address_v4(std::string_view str,
                                 aios::system::error_code &ec) noexcept
    : data_{0} {
  if (str == "localhost") {
    ec = aios::system::error_code::no_errors;
    *this = loopback();
    return;
  }
  ec = aios::system::error_code::bad_address_convertation;
  std::array<uint8_t, 4> bytes{UINT8_C(0), UINT8_C(0), UINT8_C(0), UINT8_C(0)};
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
  ec = aios::system::error_code::no_errors;
  *this = address_v4(bytes);
}

std::string aios::ip::address_v4::to_string() {
  return std::to_string(static_cast<uint32_t>(to_bytes()[0])) + '.' +
         std::to_string(static_cast<uint32_t>(to_bytes()[1])) + '.' +
         std::to_string(static_cast<uint32_t>(to_bytes()[2])) + '.' +
         std::to_string(static_cast<uint32_t>(to_bytes()[3]));
}
