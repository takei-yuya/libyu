// Copyright 2023 TAKEI Yuya (https://github.com/takei-yuya/)
#ifndef YU_STREAM_REPEATSTREAM_HPP_
#define YU_STREAM_REPEATSTREAM_HPP_

#include <iostream>
#include <stdexcept>
#include <vector>

namespace yu {
namespace stream {

class repeatstreambuf : public std::streambuf {
 public:
  explicit repeatstreambuf(const std::string& pattern) : buffer_(1024) {
    if (pattern.empty()) throw std::runtime_error("invalid pattern for repeatstream");
    if (pattern.size() > buffer_.size()) {
      buffer_ = std::vector<char>(pattern.data(), pattern.data() + pattern.size());
    } else {
      size_t sz = (buffer_.size() / pattern.size()) * pattern.size();
      buffer_.resize(sz);
      for (size_t i = 0; i < sz; ++i) {
        buffer_[i] = pattern[i % pattern.size()];
      }
    }

    setg(buffer_.data(), buffer_.data(), buffer_.data() + buffer_.size());
  }

 private:
  int underflow() override {
    setg(buffer_.data(), buffer_.data(), buffer_.data() + buffer_.size());
    return traits_type::to_int_type(*gptr());
  }

  std::vector<char> buffer_;
};

class repeatstream : public std::istream {
 public:
  explicit repeatstream(const std::string& pattern) : std::istream(&repeatstreambuf_), repeatstreambuf_(pattern) {}
 private:
  repeatstreambuf repeatstreambuf_;
};

}  // namespace stream
}  // namespace yu

#endif  // YU_STREAM_REPEATSTREAM_HPP_
