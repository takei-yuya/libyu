// Copyright 2023 TAKEI Yuya (https://github.com/takei-yuya/)
#ifndef YU_HTTP_SERVER_STREAM_HPP_
#define YU_HTTP_SERVER_STREAM_HPP_

#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "yu/http/common.hpp"
#include "yu/lang/lexical_cast.hpp"
#include "yu/stream/nullstream.hpp"
#include "yu/string/utils.hpp"

namespace yu {
namespace http {

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
    : stream_(stream),
      request_parsed_(false), request_method_(), request_target_(), request_version_(), request_header_(),
      response_header_sent_(false),
      response_status_(200), response_status_message_("OK"), response_cookies_(), response_header_() {}
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
      throw TransferError("invalid request line: fee tokens");
    }

    Header header;
    header.read(stream_);

    request_method_ = requst_line_tokens[0];
    request_target_ = requst_line_tokens[1];
    request_version_ = requst_line_tokens[2];
    std::swap(request_header_, header);

    if (request_header_.has("Content-Length")) {
      std::streamsize n = yu::lang::lexical_cast<std::streamsize>(request_header_.field("Content-Length"));
      return std::make_unique<sized_istream>(stream_, n);

    } else if (request_header_.has("Transfer-Encoding")) {
      if (request_header_.field("Transfer-Encoding") == "chunked") {
        return std::make_unique<chunked_istream>(stream_);
      } else {
        throw std::runtime_error("Not impl");  // TODO: not impl
      }

    } else {
      return std::make_unique<yu::stream::nullstream>();
    }
  }

  const std::string& request_method() const { return request_method_; }
  const std::string& request_target() const { return request_target_; }
  const std::string& request_version() const { return request_version_; }
  const Header& request_header() const { return request_header_; }

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
    response_header_.add(key, value);
  }

  std::unique_ptr<chunked_ostream> send_header() {
    if (response_header_sent_) throw std::runtime_error("header already sent, cannot send header twice");
    response_header_sent_ = true;
    request_parsed_ = false;

    response_header_.add("Transfer-Encoding", "chunked");  // TODO: check Content-Length
    stream_ << "HTTP/1.1 " << response_status_ << " " << response_status_message_ << "\r\n";
    response_header_.write(stream_);
    return std::make_unique<chunked_ostream>(stream_);
  }

 private:
  std::iostream& stream_;

  // Request
  bool request_parsed_;
  std::string request_method_;
  std::string request_target_;
  std::string request_version_;
  Header request_header_;

  // Response
  bool response_header_sent_;
  int response_status_;
  std::string response_status_message_;
  std::vector<std::string> response_cookies_;
  Header response_header_;
};

}  // namespace http
}  // namespace yu

#endif  // YU_HTTP_SERVER_STREAM_HPP_
