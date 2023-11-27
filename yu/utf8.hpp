// Copyright 2023 TAKEI Yuya (https://github.com/takei-yuya/)
#ifndef YU_UTF8_HPP_
#define YU_UTF8_HPP_

#include <cassert>
#include <cstdint>
#include <iostream>
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

class Decoder {
 public:
  class Error : public std::runtime_error {
   public:
    Error(const std::string& message) : std::runtime_error(message) {}
  };

  explicit Decoder(std::istream& in, uint32_t invalid = 0xfffd)
    : in_(in), invalid_(invalid), next_(invalid), last_error_(""), num_bytes_(0), num_chars_(0), num_errors_(0) {
    next_ = decode();
  }

  bool has_next() const { return next_ != kEOF; }

  uint32_t next() {
    assert(has_next());
    uint32_t result = next_;
    next_ = decode();
    return result;
  }

  const std::string& last_error() const { return last_error_; }
  size_t num_processed_bytes() const { return num_bytes_; }
  size_t num_processed_chars() const { return num_chars_; }
  size_t num_processed_errors() const { return num_errors_; }

 private:
  const uint32_t kEOF = static_cast<uint32_t>(-1);

  uint32_t decode() try {
    int ch = in_.get();
    if (ch == EOF) return kEOF;
    ++num_bytes_;
    int clo = count_leading_ones(ch);
    if (clo == 1) throw Error("an unexpected continuation byte");
    if (clo >= 5) throw Error("invalid byte");

    if (clo == 0) {
      ++num_chars_;
      return static_cast<uint32_t>(ch);
    }

    uint32_t cp = ch & (0b11111111 >> clo);
    for (int i = 1; i < clo; ++i) {
      cp = cp << 6 | get_continuation_byte();
    }

    // overlong encoding
    if (clo == 2 && cp <= 0x7f) throw Error("an overlong encoding");
    if (clo == 3 && cp <= 0x7ff) throw Error("an overlong encoding");
    if (clo == 4 && cp <= 0xffff) throw Error("an overlong encoding");

    // surrogate char
    if (0xD800 <= cp && cp <= 0xDFFF) throw Error("a sequence that decodes to an invalid code point");
    // 5 or more bytes char. checking by clo, but just in case
    if (cp > 0x10FFFF) throw Error("a sequence that decodes to an invalid code point");

    last_error_ = "";
    ++num_chars_;
    return cp;
  } catch (const Error& e) {
    last_error_ = e.what();
    ++num_chars_;
    ++num_errors_;
    return invalid_;
  }

  uint32_t get_continuation_byte() {
    int ch = in_.get();
    if (ch == EOF) throw Error("the string ending before the end of the character");
    ++num_bytes_;
    int clo = count_leading_ones(ch);
    if (clo != 1) throw Error("a non-continuation byte before the end of the character");
    return 0b00111111 & ch;
  }

  int count_leading_ones(int ch) {
    if ((0b10000000 & ch) == 0b00000000) return 0;
    if ((0b11000000 & ch) == 0b10000000) return 1;
    if ((0b11100000 & ch) == 0b11000000) return 2;
    if ((0b11110000 & ch) == 0b11100000) return 3;
    if ((0b11111000 & ch) == 0b11110000) return 4;
    if ((0b11111100 & ch) == 0b11111000) return 5;
    if ((0b11111110 & ch) == 0b11111100) return 6;
    if ((0b11111111 & ch) == 0b11111110) return 7;
    return 8;
  }

  std::istream& in_;
  const uint32_t invalid_;
  uint32_t next_;
  std::string last_error_;
  size_t num_bytes_;
  size_t num_chars_;  // num of code points returned
  size_t num_errors_;  // num of invalid_char returned
};

}  // namespace utf8
}  // namespace yu

#endif  // YU_UTF8_HPP_
