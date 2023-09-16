// Copyright 2023 TAKEI Yuya (https://github.com/takei-yuya/)
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

class iteestreambuf : public std::streambuf {
 public:
  iteestreambuf(std::istream& in, std::ostream& out) : in_(in), out_(out), buffer_(1024) {
    setg(buffer_.data(), buffer_.data() + buffer_.size(), buffer_.data() + buffer_.size());
  }

 private:
  virtual int underflow() {
    if (gptr() < egptr()) return *gptr();

    in_.read(buffer_.data(), buffer_.size());
    std::streamsize size = in_.gcount();
    if (size == 0) return traits_type::eof();
    out_.write(buffer_.data(), size);
    setg(buffer_.data(), buffer_.data(), buffer_.data() + size);
    return *gptr();
  }


  std::istream& in_;
  std::ostream& out_;
  std::vector<char> buffer_;
};

class iteestream : public std::istream {
 public:
  iteestream(std::istream& in, std::ostream& out) : std::istream(new iteestreambuf(in, out)) {}
  ~iteestream() {
    delete rdbuf();
  }
};

}  // namespace stream
}  // namespace yu

#endif  // YU_STREAM_TEESTREAM_HPP_
