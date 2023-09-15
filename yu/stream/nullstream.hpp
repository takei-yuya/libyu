#ifndef YU_STREAM_NULLSTREAM_
#define YU_STREAM_NULLSTREAM_

#include <iostream>
#include <vector>

namespace yu {
namespace stream {

class nullstreambuf : public std::streambuf {
 public:
  nullstreambuf() : buffer_(1024) {
    setp(buffer_.data(), buffer_.data() + buffer_.size());
  }

 private:
  virtual int overflow(int ch = traits_type::eof()) {
    size_t size = pptr() - pbase();
    pbump(-size);
    return ch;
  }

  virtual int underflow() {
    return traits_type::eof();
  }
  std::vector<char> buffer_;
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
