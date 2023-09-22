// Copyright 2023 TAKEI Yuya (https://github.com/takei-yuya/)
#ifndef YU_HTTP_COMMON_HPP_
#define YU_HTTP_COMMON_HPP_

#include <iostream>
#include <map>
#include <sstream>

#include "yu/string/utils.hpp"

namespace yu {
namespace http {

class TransferError : public std::runtime_error {
 public:
  explicit TransferError(const std::string& error) : std::runtime_error(error) {}
};

class Header {
 public:
  using Fields = std::map<std::string, std::string, yu::string::iLess>;  // ignore case key

  Header() : set_cookies_(), fields_() {}

  void read(std::istream& in) {
    std::vector<std::string> set_cookies;
    Fields fields;
    std::string line;
    while (std::getline(in, line)) {
      line = yu::string::rstrip(line, "\r");
      if (line.empty()) break;  // end of header

      std::vector<std::string> header_tokens = yu::string::split(line, ':', false, 2);
      if (header_tokens.size() != 2) throw TransferError("invalid header: header is not 'key: value', line = " + line);

      // TODO: validate
      std::string key = yu::string::strip(header_tokens[0], " \t");
      std::string value = yu::string::strip(header_tokens[1], " \t");  // TODO: escape non v-char
      if (key != header_tokens[0]) throw TransferError("invalid header: obs-fold");
      if (yu::string::icompare(key, "Set-Cookie") == 0) {
        set_cookies.push_back(value);
      } else if (fields[key].empty()) {
        fields[key] = value;
      } else {
        fields[key] += "," + value;
      }
    }

    set_cookies_.swap(set_cookies);
    fields_.swap(fields);
  }

  void write(std::ostream& out) const {
    for (const auto& set_cookie : set_cookies_) {
      out << "Set-Cookie: " << set_cookie << "\r\n";
    }
    for (const auto& field : fields_) {
      out << field.first << ": " << field.second << "\r\n";
    }
    out << "\r\n";
  }

  void add(const std::string& key, const std::string& value) {
    // TODO: validate
    std::string striped_value = yu::string::strip(value, " \t");  // TODO: escape RFC2047
    if (yu::string::icompare(key, "Set-Cookie") == 0) {
      set_cookies_.push_back(striped_value);
    } else if (fields_[key].empty()) {
      fields_[key] = striped_value;
    } else {
      fields_[key] += "," + striped_value;
    }
  }

  bool has(const std::string& key) const {
    const auto it = fields_.find(key);
    return it != fields_.end();
  }

  const std::string& field(const std::string& key) const {
    static const std::string kEmpty = "";
    const auto it = fields_.find(key);
    if (it == fields_.end()) return kEmpty;
    return it->second;
  }

  const std::vector<std::string>& get_set_cookies() const {
    return set_cookies_;
  }

 private:
  std::vector<std::string> set_cookies_;
  Fields fields_;
};

class chunked_ostreambuf : public std::streambuf {
 public:
  explicit chunked_ostreambuf(std::ostream& out) : out_(out), buffer_(1024), finished_(false) {
    setp(buffer_.data(), buffer_.data() + buffer_.size());
  }
  ~chunked_ostreambuf() {
    finish();
  }

  void finish() {
    if (finished_) return;
    send_all();
    out_ << "0\r\n";  // last-chunk
    // TODO: trailer
    out_ << "\r\n";
    out_.flush();
    finished_ = true;
  }

 private:
  int overflow(int ch = traits_type::eof()) override {
    send_all();
    if (ch != traits_type::eof()) {
      *pbase() = static_cast<char>(ch);
      pbump(1);
    }
    return ch;
  }

  bool send_all() {
    size_t size = static_cast<size_t>(pptr() - pbase());
    if (size == 0) return true;  // do not send empty chunk
    std::ostringstream oss;
    oss << std::hex << std::uppercase << size;
    out_ << oss.str() << "\r\n";  // chunk size
    out_.write(pbase(), static_cast<std::streamsize>(size));  // chunk
    out_ << "\r\n";  // chunk boundary
    out_.flush();
    pbump(static_cast<int>(-size));
    return true;
  }

  int sync() override {
    if (send_all()) return 0;
    return -1;
  }

  std::ostream& out_;
  std::vector<char> buffer_;
  bool finished_;
};

class chunked_ostream : public std::ostream {
 public:
  explicit chunked_ostream(std::ostream& out) : std::ostream(&buf_), buf_(out) {}
  void finish() { buf_.finish(); }
 private:
  chunked_ostreambuf buf_;
};

class chunked_istreambuf : public std::streambuf {
 public:
  explicit chunked_istreambuf(std::istream& in) : in_(in), unread_size_(0), buffer_(1024), eof_(false) {
    setg(buffer_.data(), buffer_.data() + buffer_.size(), buffer_.data() + buffer_.size());
  }
 private:
  int underflow() override {
    if (eof_) return traits_type::eof();
    if (gptr() < egptr()) return *gptr();

    if (unread_size_ == 0) {
      // new chunk
      std::string line;
      std::getline(in_, line);
      line = yu::string::rstrip(line, "\r");
      std::istringstream iss(line);
      iss >> std::hex >> unread_size_;
      if (unread_size_ == 0) {  // last chunk
        eof_ = true;
        std::string line;
        while (std::getline(in_, line)) {
          line = yu::string::rstrip(line, "\r");
          if (line.empty()) break;  // end of trailer
          // TODO: trailer
        }
        return traits_type::eof();
      }
    }
    in_.read(buffer_.data(), static_cast<std::streamsize>(std::min(buffer_.size(), unread_size_)));
    std::streamsize read_count = in_.gcount();
    unread_size_ -= static_cast<size_t>(read_count);
    if (unread_size_ == 0) {  // end of chunk
      // read skip \r\n
      if (in_.get() != '\r') throw TransferError("Invalid chunk end: expect '\\r', but not");
      if (in_.get() != '\n') throw TransferError("Invalid chunk end: expect '\\n', but not");
    }
    setg(buffer_.data(), buffer_.data(), buffer_.data() + read_count);
    return traits_type::to_int_type(*gptr());
  }

  std::istream& in_;
  size_t unread_size_;
  std::vector<char> buffer_;
  bool eof_;
};

class chunked_istream : public std::istream {
 public:
  explicit chunked_istream(std::istream& in) : std::istream(&buf_), buf_(in) {}
 private:
  chunked_istreambuf buf_;
};

class sized_istreambuf : public std::streambuf {
 public:
  sized_istreambuf(std::istream& in, std::streamsize size) : in_(in), size_(size), buffer_(1024) {
    setg(buffer_.data(), buffer_.data() + buffer_.size(), buffer_.data() + buffer_.size());
  }

 private:
  int underflow() override {
    if (gptr() < egptr()) return *gptr();
    if (size_ == 0) return traits_type::eof();

    in_.read(buffer_.data(), size_);
    std::streamsize read_count = in_.gcount();
    size_ -= read_count;
    setg(buffer_.data(), buffer_.data(), buffer_.data() + read_count);
    return traits_type::to_int_type(*gptr());
  }

  std::istream& in_;
  std::streamsize size_;
  std::vector<char> buffer_;
};

class sized_istream : public std::istream {
 public:
  sized_istream(std::istream& in, std::streamsize n) : std::istream(&buf_), buf_(in, n) {}
 private:
  sized_istreambuf buf_;
};

}  // namespace http
}  // namespace yu

#endif  // YU_HTTP_COMMON_HPP_
