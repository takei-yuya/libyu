// Copyright 2023 TAKEI Yuya (https://github.com/takei-yuya/)
#ifndef YU_STREAM_NULLSTREAM_
#define YU_STREAM_NULLSTREAM_

#include <iostream>
#include <vector>

namespace yu {
namespace stream {

class nullstreambuf : public std::streambuf {
 private:
  std::streamsize xsputn(const char*, std::streamsize n) override {
    return n;
  }

  int overflow(int ch = traits_type::eof()) override {
    return traits_type::not_eof(ch);
  }

  int underflow() override {
    return traits_type::eof();
  }
};

class nullstream : public std::iostream {
 public:
  nullstream() : std::iostream(&buf_), buf_() {}
 private:
  nullstreambuf buf_;
};

}  // namespace stream
}  // namespace yu

#endif  // YU_STREAM_NULLSTREAM_
