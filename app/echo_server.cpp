#include "yu/stream/fdstream.hpp"

#include <fcntl.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

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

  struct sockaddr_in addr {};
  addr.sin_family = PF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(kPort);
  if (::bind(sock, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) < 0) {
    std::cerr << "bind failure: " << strerror(errno) << std::endl;
    return 1;
  }

  if (::listen(sock, 5) < 0) {
    std::cerr << "listen failure: " << strerror(errno) << std::endl;
    return 1;
  }
  std::cerr << "listen in port " << kPort << std::endl;

  int fd;
  while ((fd = ::accept(sock, nullptr, nullptr)) >= 0) {
    if (fd < 0) {
      std::cerr << "accept failure: " << strerror(errno) << std::endl;  // TODO: Logger
      break;
    }
    std::cerr << "accept" << std::endl;
    FDCloser fd_closer(fd);

    yu::stream::fdstream fds(fd);
    size_t count = 0;
    int ch;
    while ((ch = fds.get()) != EOF) {
      fds.put(static_cast<char>(ch));
      ++count;
    }
    std::cerr << "read/write " << count << "bytes" << std::endl;
  }
}
