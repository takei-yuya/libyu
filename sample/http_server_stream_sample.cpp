#include "yu/http/server_stream.hpp"

#include <string.h>
#include <sys/socket.h>

#include "yu/stream/fdstream.hpp"

int main() {
  int fds[2];
  ::socketpair(AF_UNIX, SOCK_STREAM, 0, fds);

  yu::stream::fdstream user_agent(fds[0]);
  yu::stream::fdstream server(fds[1]);

  yu::http::ServerStream ss(server);

  std::cout << "* Send request" << std::endl;
  user_agent
    << "POST / HTTP/1.1\r\n"
    << "Host: example.com\r\n"
    << "User-Agent: test\r\n"
    << "Transfer-Encoding: chunked\r\n"
    << "\r\n"
    << "6\r\n"
    << "Hello \r\n"
    << "6\r\n"
    << "World!\r\n"
    << "0\r\n"
    << "\r\n";
  user_agent.flush();

  std::cout << "* Recieve request" << std::endl;
  {
    std::unique_ptr<std::istream> request_body_stream = ss.parse_request();
    std::cout << ss.request_method() << " " << ss.request_target() << " " << ss.request_version() << std::endl;
    ss.request_header().write(std::cout);

    std::vector<char> buffer(1024);
    request_body_stream->read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    std::string request_body(buffer.data(), buffer.data() + request_body_stream->gcount());
    std::cout << request_body << std::endl;
  }

  std::cout << "* Send response" << std::endl;
  ss.set_status(200);
  ss.set_header("Server", "libyu http::ServerStream");
  ss.set_header("Set-Cookie", "foo=1; path=/");
  ss.set_header("Server", "version 0.0.0");
  ss.set_header("Set-Cookie", "bar=2; path=/");
  {
    std::unique_ptr<std::ostream> response_body_stream = ss.send_header();
    *response_body_stream << "It's ";
    *response_body_stream << "wonderful ";
    *response_body_stream << "wordl!!";
  }

  std::cout << "* Recieve response" << std::endl;
  std::string line;
  // header
  while (std::getline(user_agent, line)) {
    line = yu::string::rstrip(line, "\r");
    if (line.empty()) break;
    std::cout << line << std::endl;
  }
  std::cout << std::endl;
  // body
  while (std::getline(user_agent, line)) {
    line = yu::string::rstrip(line, "\r");
    if (line.empty()) break;
    std::cout << line << std::endl;
  }

  close(fds[0]);
  close(fds[1]);
}

