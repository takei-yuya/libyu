#include "yu/stream/fdstream.hpp"
#include "yu/http/server_stream.hpp"
#include "yu/json.hpp"

#include <fcntl.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <mutex>

static const int kPort = 3360;

namespace {
class FDCloser {
 public:
  FDCloser(int fd) : fd_(fd) {}
  ~FDCloser() { if (fd_ >= 0) { close(fd_); } }
 private:
  int fd_;
};
}  // namespace

class App {
 public:
  App() : counter_(0) {}

  class GreetingResponse {
   public:
    explicit GreetingResponse(int n) {
      std::ostringstream oss;
      oss << "Hello! You are " << n << number_suffix(n) << " visiter!";
      message = oss.str();
    }
   private:
    std::string message;
   public:
    void stringifyJson(yu::json::Stringifier& stringifier) const {
      JSON_MEMBER_STRINGIFIER(stringifier) << JSON_GETTER(message) << JSON_STRINGIFY;
    }
  };

  class ResetCounterRequest {
   public:
    ResetCounterRequest() : count(0) {}
    int count;
   public:
    void parseJson(yu::json::Parser& parser) {
      JSON_MEMBER_PARSER(parser) << JSON_SETTER(count) << JSON_PARSE;
    }
  };

  class ResetCounterResponse {
   public:
    ResetCounterResponse() : message("Reset counter successfully") {}
   private:
    std::string message;
   public:
    void stringifyJson(yu::json::Stringifier& stringifier) const {
      JSON_MEMBER_STRINGIFIER(stringifier) << JSON_GETTER(message) << JSON_STRINGIFY;
    }
  };

  class ErrorResponse {
   public:
    explicit ErrorResponse(const std::string& error) : error(error) {
    }
   private:
    std::string error;
   public:
    void stringifyJson(yu::json::Stringifier& stringifier) const {
      JSON_MEMBER_STRINGIFIER(stringifier) << JSON_GETTER(error) << JSON_STRINGIFY;
    };
  };

  void request(yu::http::ServerStream& ss) try {
    std::unique_ptr<std::istream> request_body;
    request_body = ss.parse_request();

    std::cerr << ss.request_method() << " " << ss.request_target() << " " << ss.request_version() << std::endl;
    if (ss.request_method() == "GET" && ss.request_target() == "/") {
      int count = get_count();
      GreetingResponse response(count);
      ss.set_status(200);
      auto out = ss.send_header();
      yu::json::stringify(*out, response);
      return;
    } else if (ss.request_method() == "POST" && ss.request_target() == "/reset_count") {
      ResetCounterRequest request;
      try {
        yu::json::parse(*request_body, request);
      } catch (const std::runtime_error& e) {
        throw yu::http::TransferError("Invalid request body: error = " + std::string(e.what()));
      }
      reset_count(request.count);
      ss.set_status(200);
      ResetCounterResponse response;
      yu::json::stringify(*ss.send_header(), response);
      return;
    }

    ss.set_status(404, "Not Found");
    ErrorResponse response("Request path not found: method = " + ss.request_method() + ", target = " + ss.request_target());
    yu::json::stringify(*ss.send_header(), response);
  } catch (const yu::http::TransferError& e) {
    ss.set_status(400);
    ErrorResponse response("Request is invalid: error = " + std::string(e.what()));
    yu::json::stringify(*ss.send_header(), response);
  }

 private:
  int get_count() {
    std::unique_lock lock(counter_mutex_);
    return ++counter_;
  }

  void reset_count(int n) {
    std::unique_lock lock(counter_mutex_);
    counter_ = n;
  }

  static std::string number_suffix(int n) {
    if (n == 11) return "th";
    if (n % 10 == 1) return "st";
    if (n % 10 == 2) return "nd";
    if (n % 10 == 3) return "rd";
    return "th";
  }

  std::mutex counter_mutex_;
  int counter_;
};

int main() {
  int sock;
  if ((sock = ::socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    std::cerr << "socket failure: " << strerror(errno) << std::endl;
    return 1;
  }
  FDCloser sock_closer(sock);

  int yes = 1;
  if (::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
    std::cerr << "setsockopt failure: " << strerror(errno) << std::endl;
    return 1;
  }

  struct sockaddr_in addr = { 0 };
  addr.sin_family = PF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(kPort);
  if (::bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    std::cerr << "bind failure: " << strerror(errno) << std::endl;
    return 1;
  }

  if (::listen(sock, 5) < 0) {
    std::cerr << "listen failure: " << strerror(errno) << std::endl;
    return 1;
  }
  std::cerr << "listen in port " << kPort << std::endl;

  App app;

  int fd;
  while ((fd = ::accept(sock, nullptr, nullptr)) >= 0) {
    if (fd < 0) {
      std::cerr << "accept failure: " << strerror(errno) << std::endl;  // TODO: Logger
      break;
    }
    std::cerr << "accept" << std::endl;
    FDCloser fd_closer(fd);

    yu::stream::fdstream fds(fd);
    yu::http::ServerStream ss(fds);
    app.request(ss);
  }
}
