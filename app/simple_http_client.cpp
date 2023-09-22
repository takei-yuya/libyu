#include "yu/stream/fdstream.hpp"
#include "yu/http/client_stream.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

namespace {
class FDCloser {
 public:
  FDCloser(int fd) : fd_(fd) {}
  ~FDCloser() { if (fd_ >= 0) { close(fd_); } }
 private:
  int fd_;
};
}  // namespace

int main(int argc, char** argv) {
  // TODO: URI parser
  if (argc < 5) {
    std::cout << "Usage: " << argv[0] << " HOST PORT METHOD TARGET" << std::endl;
    return 1;
  }
  std::string host = argv[1];
  std::string port_str = argv[2];
  std::string method = argv[3];
  std::string target = argv[4];

  struct addrinfo ai {};
  ai.ai_family = AF_INET;  // ipv4
  ai.ai_socktype = SOCK_STREAM;
  ai.ai_protocol = 0;
  struct addrinfo *result;
  if (getaddrinfo(host.c_str(), port_str.c_str(), &ai, &result) < 0) {
    std::cerr << "getaddrinfo failure: " << strerror(errno) << std::endl;
    return 1;
  }

  int sock = -1;
  for (struct addrinfo* rp = result; rp != NULL; rp = rp->ai_next) {
    if ((sock = ::socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) < 0) {
      std::cerr << "socket failure: " << strerror(errno) << std::endl;
      return 1;
    }

    if (::connect(sock, rp->ai_addr, rp->ai_addrlen) == 0) break;

    close(sock);
    sock = -1;
  }

  if (sock < 0) {
    std::cerr << "Failed to connect" << std::endl;
  }
  FDCloser sock_closer(sock);

  yu::stream::fdstream fds(sock);
  yu::http::ClientStream cs(fds);
  cs.set_header("Host", host);
  cs.set_header("User-Agent", "libyu 0.0.0");
  cs.request(method, target);

  {
    auto resp = cs.parse_respose();
    std::cout << cs.response_version() << " " << cs.response_code() << " " << cs.response_code_message() << std::endl;
    cs.response_header().write(std::cout);
    std::cout << resp->rdbuf() << std::endl;
  }
}
