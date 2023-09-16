#ifndef YU_STREAM_TEESTREAM_HPP_
#define YU_STREAM_TEESTREAM_HPP_

#include <iostream>
#include <vector>

namespace yu {
namespace stream {

class oteestreambuf : public std::streambuf {
 public:
  oteestreambuf(std::ostream& out1, std::ostream& out2) : out1_(out1), out2_(out2), buffer_(1024) {
    setp(buffer_.data(), buffer_.data() + buffer_.size());
  }
  ~oteestreambuf() {
    sync();
  }
 private:
  virtual int overflow(int ch = traits_type::eof()) {
    write_all();
    if (ch != traits_type::eof()) {
      pbump(1);
      *pbase() = ch;
    }
    return ch;
  }

  bool write_all() {
    size_t size = pptr() - pbase();
    out1_.write(pbase(), size);
    out2_.write(pbase(), size);
    return true;
  }

  virtual int sync() {
    if (write_all()) return 0;
    return -1;
  }

  std::ostream& out1_;
  std::ostream& out2_;
  std::vector<char> buffer_;
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
