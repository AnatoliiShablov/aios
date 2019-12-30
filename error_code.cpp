#include "error_code.h"

std::string_view aios::system::error_code::message() const {
  switch (data_) {
  case NO_ERRORS:
    return "No errors";
  case BAD_ADDRESS_CONVERTATION:
    return "Bad address convertation";
  default:
    return "No such error";
  }
}
