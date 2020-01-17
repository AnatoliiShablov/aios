#ifndef AIOS_ERROR_CODE_H
#define AIOS_ERROR_CODE_H

#include <cstdint>
#include <string_view>

namespace aios::system {
class error_code {
public:
  enum enum_error_codes : uint32_t {
    no_errors = 0,
    bad_address_convertation = 1,
    cant_create_socket = 2,
    cant_bind_socket = 3,
    cant_accept_socket = 4,
    cant_create_epoll = 5,
    cant_add_to_epoll = 6,
    cant_del_from_epoll = 7,
    cant_mod_epoll = 8,
    time_exceeded = 9,
    error_on_read = 10,
    error_on_write = 11
  };

  constexpr error_code() noexcept : data_{0} {}

  constexpr explicit error_code(uint32_t ec) noexcept : data_{ec} {}

  constexpr error_code(error_code const &other) noexcept = default;

  constexpr error_code(enum_error_codes codes) : data_{codes} {}

  [[nodiscard]] constexpr std::string_view message() const {
    switch (data_) {
    case no_errors:
      return "No errors";
    case bad_address_convertation:
      return "Bad address convertation";
    case cant_create_socket:
      return "Can't create socket";
    case cant_bind_socket:
      return "Can't bind socket";
    case cant_accept_socket:
      return "Can't accept socket";
    case cant_create_epoll:
      return "Can't create epoll";
    case cant_add_to_epoll:
      return "Can't add fd to epoll";
    case cant_del_from_epoll:
      return "Can't delete fd from epoll";
    case cant_mod_epoll:
      return "Can't mod fd in epoll";
    case time_exceeded:
      return "Timed out";
    case error_on_read:
      return "Error on read";
    case error_on_write:
      return "Error on write";
    default:
      return "No such error";
    }
  }

  constexpr friend bool operator==(error_code const &lhs,
                                   error_code const &rhs) noexcept {
    return lhs.data_ == rhs.data_;
  }

  constexpr friend bool operator!=(error_code const &lhs,
                                   error_code const &rhs) noexcept {
    return lhs.data_ != rhs.data_;
  }

private:
  uint32_t data_;
};
} // namespace aios::system

#endif // ERROR_CODE_H
