#ifndef YU_STREAM_FDSTREAM__
#define YU_STREAM_FDSTREAM__

#include <vector>
#include <iostream>

#include <unistd.h>

namespace libyu {
namespace stream {

#define NO_INTR(res, func) \
  while (1) { (res)=(func); if ((res) < 0 && errno == EINTR) continue; break; }

class fdstreambuf : public std::streambuf {
 public:
  fdstreambuf(int fd)
   : fd_(fd), read_buffer_(1024), write_buffer_(1024) {
    setp(write_buffer_.data(), write_buffer_.data() + write_buffer_.size());
    setg(read_buffer_.data(), read_buffer_.data() + read_buffer_.size(), read_buffer_.data() + read_buffer_.size());
  }

  ~fdstreambuf() {
    sync();
  }

 private:
  virtual int overflow(int ch = traits_type::eof()) {
    if (!write_all()) return 0;
    if (ch != traits_type::eof()) {
      pbump(1);
      *pbase() = ch;
    }
    return ch;
  }

  bool write_all() {
    size_t size = pptr() - pbase();
    size_t total_sent = 0;
    while (total_sent < size) {
      int ret;
      NO_INTR(ret, ::write(fd_, pbase() + total_sent, size - total_sent))
      if (ret < 0) return false;
      total_sent += ret;
    }
    pbump(-size);
    return true;
  }

  virtual int underflow() {
    if (gptr() < egptr()) return *gptr();

    int ret;
    NO_INTR(ret, ::read(fd_, read_buffer_.data(), read_buffer_.size()))
    if (ret < 0) return traits_type::eof();
    if (ret == 0) return traits_type::eof();
    setg(read_buffer_.data(), read_buffer_.data(), read_buffer_.data() + ret);
    return *gptr();
  }

  virtual int sync() {
    if (write_all()) return 0;
    return -1;
  }

  int fd_;
  std::vector<char> read_buffer_;
  std::vector<char> write_buffer_;
};

class fdstream : public std::iostream {
 public:
  fdstream(int fd) : std::iostream(new fdstreambuf(fd)) {
  }
  ~fdstream() {
    flush();
    delete rdbuf();
  }
};

#undef NO_INTR

}  // namespace
}  // libyu

#endif  // YU_STREAM_FDSTREAM__
