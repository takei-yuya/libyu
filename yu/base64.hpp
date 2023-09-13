#ifndef YU_BASE64_HPP_
#define YU_BASE64_HPP_

#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <sstream>

namespace yu {
namespace base64 {

class InvalidBase64 : public std::runtime_error {
 public:
  InvalidBase64(const std::string& message) : std::runtime_error(message) {}
};

// RFC4880: for Radix-64
class Radix64CRC24 {
 public:
  Radix64CRC24(uint32_t init = 0xB704CEL, uint32_t poly = 0x1864CFBL) : crc_(init), poly_(poly) {
  }

  Radix64CRC24& addChar(char ch) {
    crc_ ^= (ch << 16);
    for (size_t i = 0; i < 8; ++i) {
      crc_ <<= 1;
      if (crc_ & 0x1000000L) {
        crc_ ^= poly_;
      }
    }
    return *this;
  }

  uint32_t crc24() const { return crc_ & 0xFFFFFFL; }

 private:
  uint32_t crc_;
  uint32_t poly_;
};

class Encoder {
 public:
  const static char kNoPadding = '\0';
  Encoder(size_t wrap_width = 76, char char62 = '+', char char63 = '/', char pad = '=', const std::string& newline = "\n")
    : wrap_width_(wrap_width), pad_(pad), newline_(newline)
  {
    initialize_alphabet_table(char62, char63);
  }

  uint32_t encode(std::istream& in, std::ostream& out) const {
    Radix64CRC24 crc24;
    size_t read_count = 0, write_count = 0;
    char buf;
    for (int ch = in.get(); ch != EOF; ch = in.get()) {
      crc24.addChar(ch);
      if ((read_count % 3) == 0) {
        output(out, alphabet_table_[((ch & 0b11111100) >> 2)], write_count);
        buf = (ch & 0b00000011) << 4;
      } else if ((read_count % 3) == 1) {
        output(out, alphabet_table_[buf | ((ch & 0b11110000) >> 4)], write_count);
        buf = (ch & 0b00001111) << 2;
      } else {
        output(out, alphabet_table_[buf | ((ch & 0b11000000) >> 6)], write_count);
        output(out, alphabet_table_[((ch & 0b00111111) >> 0)], write_count);
      }
      ++read_count;
    }

    if (read_count % 3 != 0) {
      output(out, alphabet_table_[buf], write_count);
      if (pad_ != kNoPadding) {
        for (size_t i = 0; i < 3 - (read_count % 3); ++i) output(out, pad_, write_count);
      }
    }
    return crc24.crc24();
  }

 private:
  void output(std::ostream& out, char ch, size_t& write_count) const {
    out.put(ch);
    ++write_count;
    if (wrap_width_ > 0 && (write_count % wrap_width_) == 0) {
      out << newline_;
    }
  }

  void initialize_alphabet_table(char char62, char char63) {
    alphabet_table_.reserve(64);
    for (char ch = 'A'; ch <= 'Z'; ++ch) alphabet_table_.push_back(ch);
    for (char ch = 'a'; ch <= 'z'; ++ch) alphabet_table_.push_back(ch);
    for (char ch = '0'; ch <= '9'; ++ch) alphabet_table_.push_back(ch);
    alphabet_table_.push_back(char62);
    alphabet_table_.push_back(char63);
  }

  std::vector<char> alphabet_table_;
  size_t wrap_width_;
  char pad_;
  std::string newline_;
};

inline std::string encode(const std::string& str, size_t wrap_width = 76) {
  std::istringstream iss(str);
  std::ostringstream oss;
  Encoder encoder(wrap_width);
  encoder.encode(iss, oss);
  return oss.str();
}

inline std::string encodeURL(const std::string& str, size_t wrap_width = 76) {
  std::istringstream iss(str);
  std::ostringstream oss;
  Encoder encoder(0, '-', '_', Encoder::kNoPadding);
  encoder.encode(iss, oss);
  return oss.str();
}

class Decoder {
 public:
  const static char kNoPadding = '\0';
  Decoder(char char62 = '+', char char63 = '/', char pad = '=', const std::string& allowed_white_space = " \r\n")
    : pad_(pad)
  {
    initialize_bit_table(char62, char63, pad, allowed_white_space);
  }

  uint32_t decode(std::istream& in, std::ostream& out) const {
    Radix64CRC24 crc24;
    size_t read_count = 0;
    char buf;
    char ch;
    while ((ch = in.get()) != EOF) {
      char bits = bit_table_[ch];
      if (bits == kInvalidChar) throw InvalidBase64("unexpected char: ch = '" + std::string(1, ch) + "'");
      if (bits == kSkipChar) continue;
      if (bits == kPadChar) {
        ++read_count;
        while ((ch = in.get()) != EOF) {
          char bits = bit_table_[ch];
          if (bits == kSkipChar) continue;
          if (bits == kPadChar) { ++read_count; continue; }
          throw InvalidBase64("unexpected char after padding: ch = '" + std::string(1, ch) + "'");
        }
        break;
      }

      if ((read_count % 4) == 0) {
        buf = (bits & 0b00111111) << 2;
      } else if ((read_count % 4) == 1) {
        char ch = buf | ((bits & 0b00110000) >> 4);
        crc24.addChar(ch);
        out.put(ch);
        buf = (bits & 0b00001111) << 4;
      } else if ((read_count % 4) == 2) {
        char ch = buf | ((bits & 0b00111100) >> 2);
        crc24.addChar(ch);
        out.put(ch);
        buf = (bits & 0b00000011) << 6;
      } else {
        char ch = buf | ((bits & 0b00111111) >> 0);
        crc24.addChar(ch);
        out.put(ch);
        buf = 0;
      }

      ++read_count;
    }
    if (pad_ != kNoPadding) {
      if ((read_count % 4) != 0) throw InvalidBase64("odd padding");
    }
    return crc24.crc24();
  }

 private:
  const char kInvalidChar = '\xff';
  const char kSkipChar = '\xfe';
  const char kPadChar = '\xfd';
  void initialize_bit_table(char char62, char char63, char pad, const std::string& allowed_white_space) {
    bit_table_.resize(256, kInvalidChar);
    size_t i = 0;
    for (char ch = 'A'; ch <= 'Z'; ++ch) bit_table_[ch] = i++;
    for (char ch = 'a'; ch <= 'z'; ++ch) bit_table_[ch] = i++;
    for (char ch = '0'; ch <= '9'; ++ch) bit_table_[ch] = i++;
    bit_table_[char62] = i++;
    bit_table_[char63] = i++;
    bit_table_[pad] = kPadChar;
    for (char ch : allowed_white_space) bit_table_[ch] = kSkipChar;
  }

  char pad_;
  std::vector<char> bit_table_;
};

inline std::string decode(const std::string& str) {
  std::istringstream iss(str);
  std::ostringstream oss;
  Decoder decoder;
  decoder.decode(iss, oss);
  return oss.str();
}

inline std::string decodeURL(const std::string& str) {
  std::istringstream iss(str);
  std::ostringstream oss;
  Decoder decoder('-', '_', Decoder::kNoPadding);
  decoder.decode(iss, oss);
  return oss.str();
}

}  // namespace base64
}  // namespace yu

#endif  // YU_BASE64_HPP_
