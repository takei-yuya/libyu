// Copyright 2023 TAKEI Yuya (https://github.com/takei-yuya/)
#ifndef YU_STREAM_NULLSTREAM_
#define YU_STREAM_NULLSTREAM_

#include <iostream>
#include <vector>

namespace yu {
namespace stream {

class nullstreambuf : public std::streambuf {
 private:
  virtual std::streamsize xsputn(const char* s, std::streamsize n) {
    return n;
  }

  virtual int underflow() {
    return traits_type::eof();
  }
};

class nullstream : public std::iostream {
 public:
  nullstream() : std::iostream(new nullstreambuf()) {
  }
  ~nullstream() {
    delete rdbuf();
  }
};

}  // namespace stream
}  // namespace yu

#endif  // YU_STREAM_NULLSTREAM_
