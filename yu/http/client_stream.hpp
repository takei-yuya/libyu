// Copyright 2023 TAKEI Yuya (https://github.com/takei-yuya/)
#ifndef YU_HTTP_CLIENT_STREAM_HPP_
#define YU_HTTP_CLIENT_STREAM_HPP_

#include <iostream>
#include <memory>

#include "yu/http/common.hpp"
#include "yu/lang/lexical_cast.hpp"
#include "yu/stream/nullstream.hpp"

namespace yu {
namespace http {

class ClientStream {
 public:
  explicit ClientStream(std::iostream& stream)
    : stream_(stream), request_header_(),
      response_version_(), response_code_(), response_code_message_(), response_header_() {}

  // Request
  void set_header(const std::string& key, const std::string& value) {
    request_header_.add(key, value);
  }

  std::unique_ptr<std::ostream> request(const std::string& request_method,
                                        const std::string& request_target,
                                        const std::string& request_version = "HTTP/1.1") {
    // TODO: validate
    stream_ << request_method << " " << request_target << " " << request_version << "\r\n";
    request_header_.add("Transfer-Encoding", "chunked");  // TODO: check Content-Length
    request_header_.write(stream_);
    return std::unique_ptr<std::ostream>(new chunked_ostream(stream_));
  }

  // Response
  std::unique_ptr<std::istream> parse_respose() {
    // response code
    std::string line;
    std::getline(stream_, line);
    line = yu::string::rstrip(line, "\r");
    std::vector<std::string> response_code_tokens = yu::string::split(line, ' ', false, 3);
    if (response_code_tokens.size() != 3) throw TransferError("invalid response line: fee tokens");
    int response_code = yu::lang::lexical_cast<int>(response_code_tokens[1]);

    // header
    Header response_header;
    response_header.read(stream_);

    response_version_ = response_code_tokens[0];
    response_code_ = response_code;
    response_code_message_ = response_code_tokens[2];
    std::swap(response_header_, response_header);

    if (response_header_.has("Content-Length")) {
      std::streamsize n = yu::lang::lexical_cast<std::streamsize>(response_header_.field("Content-Length"));
      return std::unique_ptr<std::istream>(new sized_istream(stream_, n));

    } else if (response_header_.has("Transfer-Encoding")) {
      if (response_header_.field("Transfer-Encoding") == "chunked") {
        return std::unique_ptr<std::istream>(new chunked_istream(stream_));
      } else {
        throw std::runtime_error("Not impl");  // TODO: not impl
      }

    } else {
      return std::unique_ptr<std::istream>(new yu::stream::nullstream());
    }
  }

  const std::string& response_version() const { return response_version_; }
  int response_code() const { return response_code_; }
  const std::string& response_code_message() const { return response_code_message_; }
  const Header& response_header() const { return response_header_; }

 private:
  std::iostream& stream_;
  Header request_header_;

  std::string response_version_;
  int response_code_;
  std::string response_code_message_;
  Header response_header_;
};

}  // namespace http
}  // namespace yu

#endif  // YU_HTTP_CLIENT_STREAM_HPP_
