#ifndef YU_STREAM_TEESTREAM_HPP_
#define YU_STREAM_TEESTREAM_HPP_

#include <iostream>
#include <vector>

namespace yu {
namespace stream {

class oteestreambuf : public std::streambuf {
 public:
  oteestreambuf(std::ostream& out1, std::ostream& out2) : out1_(out1), out2_(out2) {
  }
 private:
  virtual std::streamsize xsputn(const char* s, std::streamsize n) {
    out1_.write(s, n);
    out2_.write(s, n);
    return n;
  }

  std::ostream& out1_;
  std::ostream& out2_;
};

class oteestream : public std::ostream {
 public:
  oteestream(std::ostream& out1, std::ostream& out2) : std::ostream(new oteestreambuf(out1, out2)) {}
  ~oteestream() {
    delete rdbuf();
  }
};

}  // namespace stream
}  // namespace yu

#endif  // YU_STREAM_TEESTREAM_HPP_
