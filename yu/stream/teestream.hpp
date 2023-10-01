// Copyright 2023 TAKEI Yuya (https://github.com/takei-yuya/)
#ifndef YU_STREAM_TEESTREAM_HPP_
#define YU_STREAM_TEESTREAM_HPP_

#include <iostream>
#include <vector>

namespace yu {
namespace stream {

class oteestreambuf : public std::streambuf {
 public:
  oteestreambuf() : bufs_() {}
  void add_buffer(std::streambuf* buf) { bufs_.push_back(buf); }

 private:
  std::streamsize xsputn(const char* s, std::streamsize n) override {
    std::streamsize ret = n;
    for (auto& buf : bufs_) {
      std::streamsize sz;
      if ((sz = buf->sputn(s, n)) != n) ret = std::min(sz, ret);
    }
    return ret;
  }

  int sync() override {
    int ret = 0;
    for (auto& buf : bufs_) {
      if (buf->pubsync() < 0) ret = -1;
    }
    return ret;
  }

  int overflow(int ch = traits_type::eof()) override {
    if (ch == traits_type::eof()) return traits_type::not_eof(ch);
    int ret = ch;
    for (auto& buf : bufs_) {
      if (buf->sputc(traits_type::to_char_type(ch)) == traits_type::eof()) ret = traits_type::eof();
    }
    return ret;
  }

  std::vector<std::streambuf*> bufs_;
};

class oteestream : public std::ostream {
 public:
  template <class... Args>
  explicit oteestream(Args&&... args) : std::ostream(&buf_), buf_() {
    initialize(args...);
  }

 private:
  void initialize(std::ostream& out) {
    buf_.add_buffer(out.rdbuf());
  }
  template <class... Args>
  void initialize(std::ostream& out, Args&&... args) {
    buf_.add_buffer(out.rdbuf());
    initialize(args...);
  }

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

    in_.read(buffer_.data(), static_cast<std::streamsize>(buffer_.size()));
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
