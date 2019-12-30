#include "../include/address_v4.h"

aios::ip::address_v4
aios::ip::make_address_v4(const std::string &str) noexcept {
  if (str == "localhost") {
    return address_v4::loopback();
  }
  std::array<uint8_t, 4> bytes{UINT8_C(0), UINT8_C(0), UINT8_C(0), UINT8_C(0)};
  uint32_t i{0};
  uint32_t tmp{0};
  for (char c : str) {
    if (tmp > UINT32_C(255)) {
      return address_v4::any();
    }
    if (c == '.') {
      bytes[i] = static_cast<uint8_t>(tmp);
      if (++i > UINT32_C(3)) {
        return address_v4::any();
      }
    } else if (c > '0' && c < '9') {
      tmp *= UINT32_C(10);
      tmp += static_cast<uint32_t>(c - '0');
    } else {
      return address_v4::any();
    }
  }
  if (tmp > UINT32_C(255)) {
    return address_v4::any();
  } else {
    bytes[3] = static_cast<uint8_t>(tmp);
  }
  return address_v4{bytes};
}

aios::ip::address_v4
aios::ip::make_address_v4(const std::string &str,
                          aios::system::error_code &ec) noexcept {
  if (str == "localhost") {
    ec = aios::system::error_code::NO_ERRORS;
    return address_v4::loopback();
  }
  ec = aios::system::error_code::BAD_ADDRESS_CONVERTATION;
  std::array<uint8_t, 4> bytes{UINT8_C(0), UINT8_C(0), UINT8_C(0), UINT8_C(0)};
  uint32_t i{0};
  uint32_t tmp{0};
  for (char c : str) {
    if (tmp > UINT32_C(255)) {
      return address_v4::any();
    }
    if (c == '.') {
      bytes[i] = static_cast<uint8_t>(tmp);
      if (++i > UINT32_C(3)) {
        return address_v4::any();
      }
    } else if (c > '0' && c < '9') {
      tmp *= UINT32_C(10);
      tmp += static_cast<uint32_t>(c - '0');
    } else {
      return address_v4::any();
    }
  }
  if (tmp > UINT32_C(255)) {
    return address_v4::any();
  } else {
    bytes[3] = static_cast<uint8_t>(tmp);
  }
  ec = aios::system::error_code::NO_ERRORS;
  return address_v4{bytes};
}

aios::ip::address_v4
aios::ip::make_address_v4(std::string_view str,
                          aios::system::error_code &ec) noexcept {
  if (str == "localhost") {
    ec = aios::system::error_code::NO_ERRORS;
    return address_v4::loopback();
  }
  ec = aios::system::error_code::BAD_ADDRESS_CONVERTATION;
  std::array<uint8_t, 4> bytes{UINT8_C(0), UINT8_C(0), UINT8_C(0), UINT8_C(0)};
  uint32_t i{0};
  uint32_t tmp{0};
  for (char c : str) {
    if (tmp > UINT32_C(255)) {
      return address_v4::any();
    }
    if (c == '.') {
      bytes[i] = static_cast<uint8_t>(tmp);
      if (++i > UINT32_C(3)) {
        return address_v4::any();
      }
    } else if (c > '0' && c < '9') {
      tmp *= UINT32_C(10);
      tmp += static_cast<uint32_t>(c - '0');
    } else {
      return address_v4::any();
    }
  }
  if (tmp > UINT32_C(255)) {
    return address_v4::any();
  } else {
    bytes[3] = static_cast<uint8_t>(tmp);
  }
  ec = aios::system::error_code::NO_ERRORS;
  return address_v4{bytes};
}

aios::ip::address_v4
aios::ip::make_address_v4(const char *str,
                          aios::system::error_code &ec) noexcept {
  if (str[0] == 'l' && str[1] == 'o' && str[2] == 'c' && str[3] == 'a' &&
      str[4] == 'l' && str[5] == 'h' && str[6] == 'o' && str[7] == 's' &&
      str[8] == 't' && str[9] == '\0') {
    ec = aios::system::error_code::NO_ERRORS;
    return address_v4::loopback();
  }
  std::array<uint8_t, 4> bytes{UINT8_C(0), UINT8_C(0), UINT8_C(0), UINT8_C(0)};
  uint32_t i{0};
  uint32_t tmp{0};
  ec = aios::system::error_code::BAD_ADDRESS_CONVERTATION;
  for (size_t j = 0; str[j] != '\0'; ++j) {
    char const &c = str[j];
    if (tmp > UINT32_C(255)) {
      return address_v4::any();
    }
    if (c == '.') {
      bytes[i] = static_cast<uint8_t>(tmp);
      if (++i > UINT32_C(3)) {
        return address_v4::any();
      }
    } else if (c > '0' && c < '9') {
      tmp *= UINT32_C(10);
      tmp += static_cast<uint32_t>(c - '0');
    } else {
      return address_v4::any();
    }
  }
  if (tmp > UINT32_C(255)) {
    return address_v4::any();
  } else {
    bytes[3] = static_cast<uint8_t>(tmp);
  }
  ec = aios::system::error_code::NO_ERRORS;
  return address_v4{bytes};
}
