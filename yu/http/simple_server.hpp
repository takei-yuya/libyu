// Copyright 2025 TAKEI Yuya (https://github.com/takei-yuya/)
#ifndef YU_HTTP_SIMPLE_SERVER_HPP_
#define YU_HTTP_SIMPLE_SERVER_HPP_

#include <cerrno>
#include <cstring>
#include <csignal>

#include <thread>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "../stream/fdstream.hpp"
#include "server_stream.hpp"

namespace yu {
namespace http {

namespace detail {
class FDCloser {
 public:
  explicit FDCloser(int fd) : fd_(fd) {}
  ~FDCloser() { Close(); }

  int Release() {
    int old = fd_;
    fd_ = -1;
    return old;
  }

  void Close() {
    if (fd_ != -1) {
      ::close(Release());
    }
  }

 private:
  int fd_;
};

void raise_errno(const std::string& func) {
  std::vector<char> buf(256);
  std::string err = strerror_r(errno, buf.data(), buf.size());
  throw std::runtime_error(func + " failed: " + err);
}

std::string format_time(const std::chrono::system_clock::time_point& tp,
                        const std::string& format = "%Y-%m-%d %H:%M:%S.%N") {
  time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  auto subsec = tp - std::chrono::system_clock::from_time_t(t);
  struct tm tm;
  localtime_r(&t, &tm);

  // support %N (nanoseconds)
  std::ostringstream nano_oss;
  nano_oss << std::setw(9) << std::setfill('0')
           << std::chrono::duration_cast<std::chrono::nanoseconds>(subsec).count();
  std::string fmt = format;
  size_t pos = fmt.find("%N");
  if (pos != std::string::npos) {
    fmt.replace(pos, 2, nano_oss.str());
  }

  std::ostringstream oss;
  oss << std::put_time(&tm, fmt.c_str());
  return oss.str();
}
}  // namespace detail

class WidgetBase {
 public:
  virtual ~WidgetBase() = default;
  // Return true if handled
  // WidgetBase should not read request body when it does not handle the request (and return false).
  virtual bool handle(ServerStream& stream, std::istream& request_body) = 0;
};
using WidgetPtr = std::shared_ptr<WidgetBase>;


class SimpleWidget : public WidgetBase {
 public:
  void add_content(const std::string& path,
                   const std::string& content_type,
                   const std::string& content,
                   bool expand_template = false) {
    contents_[path] = {content_type, content, expand_template};
  }

  bool handle(ServerStream& server_stream, std::istream&) override {
    if (server_stream.request_method() != "GET" &&
        server_stream.request_method() != "HEAD") {
      return false;
    }
    std::string path = server_stream.request_target();
    auto it = contents_.find(path);
    if (it == contents_.end()) {
      return false;
    }
    const auto& content_type_ = it->second.content_type;
    const auto& content_ = it->second.content;
    bool expand_template_ = it->second.expand_template;

    server_stream.set_status(200, "OK");
    server_stream.set_header("Content-Type", content_type_);
    server_stream.set_header("Connection", "close");
    auto response_stream = server_stream.send_header();

    if (server_stream.request_method() == "HEAD") {
      server_stream.send_header();  // send header only
      return true;
    }

    if (expand_template_) {
      std::unordered_map<std::string, std::string> variables;

      std::string host = "unknown";
      if (server_stream.request_header().has("Host")) {
        host = server_stream.request_header().field("Host");
      }
      variables["host"] = host;

      *response_stream << replace_template(content_, variables);
    } else {
      *response_stream << content_;
    }

    return true;
  }

 private:
  std::string replace_template(const std::string& content, const std::unordered_map<std::string, std::string>& variables) const {
    std::string result = content;
    for (const auto& var : variables) {
      std::string placeholder = "${" + var.first + "}";
      size_t pos = 0;
      while ((pos = result.find(placeholder, pos)) != std::string::npos) {
        result.replace(pos, placeholder.size(), var.second);
        pos += var.second.size();
      }
    }
    return result;
  }

  struct Content {
    std::string content_type;
    std::string content;
    bool expand_template;
  };
  std::unordered_map<std::string, Content> contents_;
};
using SimpleWidgetPtr = std::shared_ptr<SimpleWidget>;

class ChainWidget : public WidgetBase {
 public:
  ChainWidget(WidgetPtr first, WidgetPtr second)
    : first_(first), second_(second) {
    if (!first_ || !second_) {
      throw std::invalid_argument("invalid widget (nullptr)");
    }
  }

  bool handle(ServerStream& stream, std::istream& request_body) override {
    if (first_->handle(stream, request_body)) {
      return true;
    }
    return second_->handle(stream, request_body);
  }

 private:
  WidgetPtr first_;
  WidgetPtr second_;
};
WidgetPtr operator|(WidgetPtr first, WidgetPtr second) {
  return std::make_shared<ChainWidget>(first, second);
}


#define SYSCALL(ret, expr) \
  while ((ret = (expr)) == -1) { \
    if (errno == EINTR) continue; \
    perror(#expr); \
    exit(EXIT_FAILURE); \
  }

class SimpleServer {
 public:
  static constexpr int kBacklog = 5;
  SimpleServer(int port, WidgetPtr widget, int catch_all_status = 404, int backlog = kBacklog)
    : port_(port), widget_(widget), catch_all_status_(catch_all_status), backlog_(backlog) {
    if (!widget_) {
      throw std::invalid_argument("invalid widget (nullptr)");
    }
  }

  void run() {
    // TODO: logger
    std::cerr << "Starting server on port " << port_ << "..." << std::endl;

    signal(SIGPIPE, SIG_IGN);  // ignore SIGPIPE

    int ret;

    // create socket
    int server_fd;
    SYSCALL(server_fd, ::socket(AF_INET6, SOCK_STREAM, 0));
    if (server_fd < 0) {
      detail::raise_errno("socket");
    }
    detail::FDCloser server_fd_closer(server_fd);

    // set socket options
    int opt = 1;
    SYSCALL(ret, ::setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)));
    if (ret < 0) {
      detail::raise_errno("setsockopt");
    }
    opt = 0;
    SYSCALL(ret, ::setsockopt(server_fd, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt)));
    if (ret < 0) {
      detail::raise_errno("setsockopt");
    }

    // bind
    struct sockaddr_in6 addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET6;
    addr.sin6_addr = in6addr_any;  // :: (all interfaces)
    addr.sin6_port = htons(port_);
    SYSCALL(ret, ::bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)));
    if (ret < 0) {
      detail::raise_errno("bind");
    }

    // listen
    SYSCALL(ret, ::listen(server_fd, backlog_));
    if (ret < 0) {
      detail::raise_errno("listen");
    }

    while (true) {
      struct sockaddr_in6 client_addr;
      socklen_t client_addr_len = sizeof(client_addr);
      int client_fd;
      SYSCALL(client_fd, ::accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len));
      if (client_fd < 0) {
        detail::raise_errno("accept");
      }
      std::string client_ip = "[unknown]";
      std::string client_port = "0";
      {
        char host[NI_MAXHOST];
        char service[NI_MAXSERV];
        int rc = getnameinfo((struct sockaddr*)&client_addr, client_addr_len,
                             host, sizeof(host),
                             service, sizeof(service),
                             NI_NUMERICHOST | NI_NUMERICSERV);
        if (rc == 0) {
          client_ip = host;
          client_port = service;
        }
      }
      std::cerr << client_fd << ": Accepted connection from " << client_ip << ":" << client_port << std::endl;

      std::thread(&SimpleServer::process, this, client_fd).detach();
    }
  }

  void process(int client_fd) {
    auto start_time = std::chrono::system_clock::now();
    std::cerr << client_fd << ": Start thread for client_fd: " << client_fd << " at " << detail::format_time(start_time) << std::endl;
    detail::FDCloser client_fd_closer(client_fd);
    try {
      yu::stream::fdstream stream(client_fd);
      yu::http::ServerStream server_stream(stream);
      auto request_stream = server_stream.parse_request();

      std::cerr << client_fd << ":   Processing request: " << server_stream.request_line() << std::endl;
      std::cerr << client_fd << ":   Headers:" << std::endl;
      for (const auto& field : server_stream.request_header().field_names()) {
        std::cerr << client_fd << ":     " << field << ": " << server_stream.request_header().field(field) << std::endl;
      }
      if (!widget_->handle(server_stream, *request_stream)) {
        // Not handled, return catch-all status
        server_stream.set_status(catch_all_status_);
        server_stream.set_header("Content-Type", "text/plain; charset=utf-8");
        server_stream.set_header("Connection", "close");
        auto response_stream = server_stream.send_header();
        *response_stream << server_stream.status_line() << "\n";
      }
      std::cerr << client_fd << ":   Finished request: " << server_stream.status_line() << std::endl;
    } catch (const std::exception& e) {
      std::cerr << client_fd << ":   Error: " << e.what() << std::endl;
    }
    auto end_time = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cerr << client_fd << ": End thread for client_fd: " << client_fd << " at " << detail::format_time(end_time) << " (duration: " << duration.count() << " ms)" << std::endl;
  }

 private:
  int port_;
  WidgetPtr widget_;
  int catch_all_status_;
  int backlog_;
};

#undef SYSCALL

}  // namespace http
}  // namespace yu


#endif  // YU_HTTP_SIMPLE_SERVER_HPP_
