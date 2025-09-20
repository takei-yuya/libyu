// Copyright 2025 TAKEI Yuya (https://github.com/takei-yuya/)
#ifndef YU_HTTP_SIMPLE_SERVER_HPP_
#define YU_HTTP_SIMPLE_SERVER_HPP_

#include <cerrno>
#include <cstring>
#include <csignal>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../lang/unique_resource.hpp"
#include "../stream/fdstream.hpp"
#include "server_stream.hpp"

namespace yu {
namespace http {

namespace detail {
void raise_errno(const std::string& func) {
  std::error_code ec(errno, std::generic_category());
  throw std::system_error(ec, func + " failed");
}

std::string format_time(const std::chrono::system_clock::time_point& tp,
                        const std::string& format = "%Y-%m-%d %H:%M:%S.%N") {
  std::time_t t = std::chrono::system_clock::to_time_t(tp);
  auto tp_sec = std::chrono::time_point_cast<std::chrono::seconds>(tp);
  auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(tp - tp_sec).count();
  struct tm tm;
  localtime_r(&t, &tm);

  // support %N (nanoseconds)
  std::ostringstream nano_oss;
  nano_oss << std::setw(9) << std::setfill('0') << ns;
  std::string nano_str = nano_oss.str();
  std::string fmt = format;
  for (size_t pos = 0; (pos = fmt.find("%N", pos)) != std::string::npos; pos += nano_str.size() + 1) {
    fmt.replace(pos, 2, nano_str);
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
    std::string query;
    size_t pos = path.find('?');
    if (pos != std::string::npos) {
      query = path.substr(pos + 1);
      path = path.substr(0, pos);
    }

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
      return true;
    }

    if (expand_template_) {
      std::unordered_map<std::string, std::string> variables;

      std::string host = "unknown_host";
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
    yu::http::detail::raise_errno(#expr); \
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
    int raw_server_fd;
    SYSCALL(raw_server_fd, ::socket(AF_INET6, SOCK_STREAM, 0));
    auto server_fd = yu::lang::make_unique_resource(raw_server_fd, ::close);

    // set socket options
    int opt = 1;
    SYSCALL(ret, ::setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)));
    opt = 0;
    SYSCALL(ret, ::setsockopt(server_fd, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt)));

    // bind
    struct sockaddr_in6 addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET6;
    addr.sin6_addr = in6addr_any;  // :: (all interfaces)
    addr.sin6_port = htons(port_);
    SYSCALL(ret, ::bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)));

    // listen
    SYSCALL(ret, ::listen(server_fd, backlog_));

    while (true) {
      // accept
      struct sockaddr_in6 client_addr;
      socklen_t client_addr_len = sizeof(client_addr);
      int client_fd;
      SYSCALL(client_fd, ::accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len));

      std::string client_ip = "unknown_ip";
      std::string client_port = "0";
      std::string client_ip_port = "unknown_ip:0";
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
          if (client_ip.find(':') != std::string::npos) {
            client_ip_port = "[" + client_ip + "]:" + client_port;
          } else {
            client_ip_port = client_ip + ":" + client_port;
          }
        }
      }
      std::cerr << client_fd << ": Accepted connection from " << client_ip_port << std::endl;

      std::thread(&SimpleServer::process, this, client_fd).detach();
    }
  }

  void process(int raw_client_fd) {
    auto client_fd = yu::lang::make_unique_resource(raw_client_fd, ::close);
    auto start_time = std::chrono::system_clock::now();
    std::cerr << client_fd << ": Start thread for client_fd: " << client_fd << " at " << detail::format_time(start_time) << std::endl;
    try {
      yu::stream::fdstream stream(client_fd);
      yu::http::ServerStream server_stream(stream);
      try {
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
        std::cerr << client_fd << ":   Error while processing request: " << e.what() << std::endl;
        server_stream.set_status(500, "Internal Server Error");
        server_stream.set_header("Content-Type", "text/plain; charset=utf-8");
        server_stream.set_header("Connection", "close");
        auto response_stream = server_stream.send_header();
        *response_stream << "500 Internal Server Error\n";
      }
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
