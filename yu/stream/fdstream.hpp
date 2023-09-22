// Copyright 2023 TAKEI Yuya (https://github.com/takei-yuya/)
#ifndef YU_STREAM_FDSTREAM__
#define YU_STREAM_FDSTREAM__

#include <iostream>
#include <vector>

#include <unistd.h>

namespace yu {
namespace stream {

#define NO_INTR(res, func) \
  while (1) { (res)=(func); if ((res) < 0 && errno == EINTR) continue; break; }

class fdstreambuf : public std::streambuf {
 public:
  explicit fdstreambuf(int fd)
   : fd_(fd), read_buffer_(1024), write_buffer_(1024) {
    setp(write_buffer_.data(), write_buffer_.data() + write_buffer_.size());
    setg(read_buffer_.data(), read_buffer_.data() + read_buffer_.size(), read_buffer_.data() + read_buffer_.size());
  }

  ~fdstreambuf() {
    sync();
  }

 private:
  int overflow(int ch = traits_type::eof()) override {
    if (!write_all()) return 0;
    if (ch != traits_type::eof()) {
      pbump(1);
      *pbase() = static_cast<char>(ch);
    }
    return ch;
  }

  bool write_all() {
    size_t size = static_cast<size_t>(pptr() - pbase());
    size_t total_sent = 0;
    while (total_sent < size) {
      ssize_t ret;
      NO_INTR(ret, ::write(fd_, pbase() + total_sent, size - total_sent))
      if (ret < 0) return false;
      total_sent += static_cast<size_t>(ret);
    }
    pbump(static_cast<int>(-size));
    return true;
  }

  int underflow() override {
    if (gptr() < egptr()) return *gptr();

    ssize_t ret;
    NO_INTR(ret, ::read(fd_, read_buffer_.data(), read_buffer_.size()))
    if (ret < 0) return traits_type::eof();
    if (ret == 0) return traits_type::eof();
    setg(read_buffer_.data(), read_buffer_.data(), read_buffer_.data() + ret);
    return traits_type::to_int_type(*gptr());
  }

  int sync() override {
    if (write_all()) return 0;
    return -1;
  }

  int fd_;
  std::vector<char> read_buffer_;
  std::vector<char> write_buffer_;
};

class fdstream : public std::iostream {
 public:
  explicit fdstream(int fd) : std::iostream(&buf_), buf_(fd) {}
 private:
  fdstreambuf buf_;
};

#undef NO_INTR

}  // namespace stream
}  // namespace yu

#endif  // YU_STREAM_FDSTREAM__
