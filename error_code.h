#ifndef ERROR_CODE_H
#define ERROR_CODE_H

#include <cstdint>
#include <string_view>

namespace aios {
namespace system {
class error_code {
public:
  enum enum_error_codes : uint32_t {
    NO_ERRORS = 0,
    BAD_ADDRESS_CONVERTATION = 1
  };
  constexpr error_code() noexcept : data_{0} {}
  constexpr error_code(uint32_t ec) noexcept : data_{ec} {}
  constexpr error_code(error_code const &other) noexcept : data_{other.data_} {}

  [[nodiscard]] std::string_view message() const;

private:
  uint32_t data_;
};
} // namespace system
} // namespace aios

#endif // ERROR_CODE_H
