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
  std::streamsize xsputn(const char* s, std::streamsize n) override {
    out1_.write(s, n);
    out2_.write(s, n);
    return n;
  }

  int sync() override {
    out1_.flush();
    out2_.flush();
    return 0;
  }

  int overflow(int ch = traits_type::eof()) override {
    if (ch == traits_type::eof()) return ch;
    out1_.put(static_cast<char>(ch));
    out2_.put(static_cast<char>(ch));
    return ch;
  }

  std::ostream& out1_;
  std::ostream& out2_;
};

class oteestream : public std::ostream {
 public:
  oteestream(std::ostream& out1, std::ostream& out2) : std::ostream(&buf_), buf_(out1, out2) {}
 private:
  oteestreambuf buf_;
};

class iteestreambuf : public std::streambuf {
 public:
  iteestreambuf(std::istream& in, std::ostream& out) : in_(in), out_(out), buffer_(1024) {
    setg(buffer_.data(), buffer_.data() + buffer_.size(), buffer_.data() + buffer_.size());
  }

 private:
  int underflow() override {
    if (gptr() < egptr()) return *gptr();

    in_.read(buffer_.data(), buffer_.size());
    std::streamsize size = in_.gcount();
    if (size == 0) return traits_type::eof();
    out_.write(buffer_.data(), size);
    setg(buffer_.data(), buffer_.data(), buffer_.data() + size);
    return traits_type::to_int_type(*gptr());
  }


  std::istream& in_;
  std::ostream& out_;
  std::vector<char> buffer_;
};

class iteestream : public std::istream {
 public:
  iteestream(std::istream& in, std::ostream& out) : std::istream(&buf_), buf_(in, out) {}
 private:
  iteestreambuf buf_;
};

}  // namespace stream
}  // namespace yu

#endif  // YU_STREAM_TEESTREAM_HPP_
