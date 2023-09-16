// Copyright 2023 TAKEI Yuya (https://github.com/takei-yuya/)
#ifndef YU_HTTP_SERVER_STREAM_HPP_
#define YU_HTTP_SERVER_STREAM_HPP_

#include <stdexcept>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <sstream>
#include <memory>

#include "yu/string/utils.hpp"
#include "yu/stream/nullstream.hpp"
#include "yu/lang/lexical_cast.hpp"

namespace yu {
namespace http {

class RequestError : public std::runtime_error {
 public:
  explicit RequestError(const std::string& error) : std::runtime_error(error) {}
};

class chunked_ostreambuf : public std::streambuf {
 public:
  explicit chunked_ostreambuf(std::ostream& out) : out_(out), buffer_(1024) {
    setp(buffer_.data(), buffer_.data() + buffer_.size());
  }
  ~chunked_ostreambuf() {
    finish();
  }

  void finish() {
    send_all();
    out_ << "0\r\n";  // last-chunk
    // TODO: no trailer
    out_ << "\r\n";
    out_.flush();
  }

 private:
  virtual int overflow(int ch = traits_type::eof()) {
    send_all();
    if (ch != traits_type::eof()) {
      *pbase() = ch;
      pbump(1);
    }
    return ch;
  }

  bool send_all() {
    size_t size = pptr() - pbase();
    std::ostringstream oss;
    oss << std::hex << std::uppercase << size;
    out_ << oss.str() << "\r\n";  // chunk size
    out_.write(pbase(), size);  // chunk
    out_ << "\r\n";  // chunk boundary
    pbump(-size);
    return true;
  }

  std::ostream& out_;
  std::vector<char> buffer_;
};

class chunked_ostream : public std::ostream {
 public:
  explicit chunked_ostream(std::ostream& out) : std::ostream(new chunked_ostreambuf(out)) {}
  ~chunked_ostream() { delete rdbuf(); }
  void finish() { reinterpret_cast<chunked_ostreambuf*>(rdbuf())->finish(); }
};

class chunked_istreambuf : public std::streambuf {
 public:
  explicit chunked_istreambuf(std::istream& in) : in_(in), unread_size_(0), buffer_(1024), eof_(false) {
    setg(buffer_.data(), buffer_.data() + buffer_.size(), buffer_.data() + buffer_.size());
  }
 private:
  virtual int underflow() {
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
          // TODO: ignore trailer
        }
        return traits_type::eof();
      }
    }
    in_.read(buffer_.data(), std::min(buffer_.size(), unread_size_));
    std::streamsize read_count = in_.gcount();
    unread_size_ -= read_count;
    if (unread_size_ == 0) {  // end of chunk
      // read skip \r\n
      if (in_.get() != '\r') throw RequestError("Invalid chunk end: expect '\\r', but not");
      if (in_.get() != '\n') throw RequestError("Invalid chunk end: expect '\\n', but not");
    }
    setg(buffer_.data(), buffer_.data(), buffer_.data() + read_count);
    return *gptr();
  }

  std::istream& in_;
  size_t unread_size_;
  std::vector<char> buffer_;
  bool eof_;
};

class chunked_istream : public std::istream {
 public:
  explicit chunked_istream(std::istream& in) : std::istream(new chunked_istreambuf(in)) {}
  ~chunked_istream() { delete rdbuf(); }
};

class sized_istreambuf : public std::streambuf {
 public:
  sized_istreambuf(std::istream& in, std::streamsize size) : in_(in), size_(size), buffer_(1024) {
    setg(buffer_.data(), buffer_.data() + buffer_.size(), buffer_.data() + buffer_.size());
  }

 private:
  virtual int underflow() {
    if (gptr() < egptr()) return *gptr();
    if (size_ == 0) return traits_type::eof();

    in_.read(buffer_.data(), size_);
    size_t read_count = in_.gcount();
    size_ -= read_count;
    setg(buffer_.data(), buffer_.data(), buffer_.data() + read_count);
    return *gptr();
  }

  std::istream& in_;
  std::streamsize size_;
  std::vector<char> buffer_;
};

class sized_istream : public std::istream {
 public:
  sized_istream(std::istream& in, std::streamsize n) : std::istream(new sized_istreambuf(in, n)) {}
  ~sized_istream() { delete rdbuf(); }
};

namespace detail {
const static std::unordered_map<int,std::string> kResponseStatusMessage = {
  { 200, "OK" },
  { 300, "Multiple Choices" },
  { 400, "Bad Request" },
  { 500, "Internal Server Error" },
};
}  // detail

class ServerStream {
 public:
  explicit ServerStream(std::iostream& stream)
    : stream_(stream), request_parsed_(false), response_header_sent_(false), response_status_(200) {}
  // Request
  std::unique_ptr<std::istream> parse_request() {
    request_parsed_ = true;
    response_header_sent_ = false;

    // parse request line
    std::string line;
    std::getline(stream_, line);
    line = yu::string::rstrip(line, "\r");
    std::vector<std::string> requst_line_tokens = yu::string::split(line, ' ', false, 3);
    if (requst_line_tokens.size() != 3) {
      throw RequestError("invalid request line: fee tokens");
    }

    // parse headers
    std::unordered_map<std::string, std::string> headers;
    while (std::getline(stream_, line)) {
      line = yu::string::rstrip(line, "\r");
      if (line.empty()) break;  // end of header

      std::vector<std::string> header_tokens = yu::string::split(line, ':', false, 2);
      if (header_tokens.size() != 2) throw RequestError("invalid header: header is not 'key: value', line = " + line);

      std::string key = yu::string::strip(header_tokens[0], " \t");
      std::string value = yu::string::strip(header_tokens[1], " \t");
      if (key != header_tokens[0]) throw RequestError("invalid header: obs-fold");
      if (headers[key].empty()) {
        headers[key] = value;
      } else {
        headers[key] += "," + value;
      }
    }

    request_method_ = requst_line_tokens[0];
    request_target_ = requst_line_tokens[1];
    request_version_ = requst_line_tokens[2];
    request_headers_.swap(headers);
    const auto& content_length = request_headers_.find("Content-Length");
    const auto& transfer_encoding = request_headers_.find("Transfer-Encoding");
    if (content_length != request_headers_.end()) {
      std::streamsize n = yu::lang::lexical_cast<std::streamsize>(content_length->second);
      return std::make_unique<sized_istream>(stream_, n);
    } else if (transfer_encoding != request_headers_.end()) {
      return std::make_unique<chunked_istream>(stream_);
    } else {
      return std::make_unique<yu::stream::nullstream>();
    }
  }

  const std::string& request_method() const { return request_method_; }
  const std::string& request_target() const { return request_target_; }
  const std::string& request_version() const { return request_version_; }
  const std::unordered_map<std::string, std::string>& request_headers() const { return request_headers_; }

  // Response
  void set_status(int status, const std::string& messgae = "") {
    if (response_header_sent_) throw std::runtime_error("header already sent, cannot change status");
    response_status_ = status;
    if (messgae.empty()) {
      auto it = detail::kResponseStatusMessage.find(status);
      if (it != detail::kResponseStatusMessage.end()) {
        response_status_message_ = it->second;
      } else {
        response_status_message_ = "Unknown";
      }
    } else {
      response_status_message_ = messgae;
    }
  }

  void set_header(const std::string& key, const std::string& value) {
    std::string striped_key = yu::string::strip(key, " \t");
    if (striped_key == "Set-Cookie") {
      response_cookies_.push_back(value);
    } else if (response_headers_[striped_key].empty()) {
      response_headers_[striped_key] = value;
    } else {
      response_headers_[striped_key] += "," + value;
    }
  }

  std::unique_ptr<chunked_ostream> send_header() {
    if (response_header_sent_) throw std::runtime_error("header already sent, cannot send header twice");
    response_header_sent_ = true;
    request_parsed_ = false;

    stream_ << "HTTP/1.1 " << response_status_ << " " << response_status_message_ << "\r\n";
    for (const auto& cookie : response_cookies_) {
      stream_ << "Set-Cookie: " << cookie << "\r\n";
    }
    for (const auto& header : response_headers_) {
      stream_ << header.first << ": " << header.second << "\r\n";
    }
    stream_ << "Transfer-Encoding: chunked\r\n";
    stream_ << "\r\n";
    return std::make_unique<chunked_ostream>(stream_);
  }

 private:
  std::iostream& stream_;

  // Request
  bool request_parsed_;
  std::string request_method_;
  std::string request_target_;
  std::string request_version_;
  std::unordered_map<std::string, std::string> request_headers_;

  // Response
  bool response_header_sent_;
  int response_status_;
  std::string response_status_message_;
  std::vector<std::string> response_cookies_;
  std::unordered_map<std::string, std::string> response_headers_;
};

}  // namespace http
}  // namespace yu

#endif  // YU_HTTP_SERVER_STREAM_HPP_
