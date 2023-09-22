// Copyright 2023 TAKEI Yuya (https://github.com/takei-yuya/)
#ifndef YU_UTF8_HPP_
#define YU_UTF8_HPP_

#include <cstdint>
#include <sstream>

namespace yu {
namespace utf8 {

inline std::string encode(uint32_t cp) {
  std::string result;
  if (0xD800 <= cp && cp <= 0xDFFF) {
    std::ostringstream oss;
    oss << "UTF-8 not allow to encode surruogate code point: code_point = U+" << std::hex << std::uppercase << cp;
    throw std::runtime_error(oss.str());
  }
  if (cp <= 0x7f) {
    result += static_cast<char>(cp);
  } else if (cp <= 0x7ff) {
    result += static_cast<char>(0b11000000 | ((cp & 0b11111000000) >> 6));
    result += static_cast<char>(0b10000000 | ((cp & 0b00000111111) >> 0));
  } else if (cp <= 0xffff) {
    result += static_cast<char>(0b11100000 | ((cp & 0b1111000000000000) >> 12));
    result += static_cast<char>(0b10000000 | ((cp & 0b0000111111000000) >> 6));
    result += static_cast<char>(0b10000000 | ((cp & 0b0000000000111111) >> 0));
  } else if (cp <= 0x10ffff) {
    result += static_cast<char>(0b11110000 | ((cp & 0b111000000000000000000) >> 18));
    result += static_cast<char>(0b10000000 | ((cp & 0b000111111000000000000) >> 12));
    result += static_cast<char>(0b10000000 | ((cp & 0b000000000111111000000) >> 6));
    result += static_cast<char>(0b10000000 | ((cp & 0b000000000000000111111) >> 0));
  } else {
    std::ostringstream oss;
    oss << "Invalid code point: code_point = U+" << std::hex << cp;
    throw std::runtime_error(oss.str());
  }
  return result;
}

// TODO: decode

}  // namespace utf8
}  // namespace yu

#endif  // YU_UTF8_HPP_
