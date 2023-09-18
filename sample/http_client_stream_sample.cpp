#include "yu/http/client_stream.hpp"

#include <sys/socket.h>
#include <string.h>

#include "yu/stream/fdstream.hpp"

int main() {
  int fds[2];
  ::socketpair(AF_UNIX, SOCK_STREAM, 0, fds);

  yu::stream::fdstream user_agent(fds[0]);
  yu::stream::fdstream server(fds[1]);

  std::cout << "* Send request" << std::endl;
  yu::http::ClientStream cs(user_agent);
  cs.set_header("Host", "example.com");
  {
    std::unique_ptr<std::ostream> request_body_stream = cs.request("POST", "/");
    *request_body_stream << "Hello World!!";
  }

  std::cout << "* Recieve request" << std::endl;
  std::string line;
  // header
  while (std::getline(server, line)) {
    line = yu::string::rstrip(line, "\r");
    if (line.empty()) break;
    std::cout << line << std::endl;
  }
  std::cout << std::endl;
  // body
  while (std::getline(server, line)) {
    line = yu::string::rstrip(line, "\r");
    if (line.empty()) break;
    std::cout << line << std::endl;
  }

  std::cout << "* Send response" << std::endl;
  server
    << "HTTP/1.1 200 OK\r\n"
    << "Server: test\r\n"
    << "Set-Cookie: foo=1; path=/\r\n"
    << "Transfer-Encoding: chunked\r\n"
    << "\r\n"
    << "6\r\n"
    << "Hello \r\n"
    << "7\r\n"
    << "World!!\r\n"
    << "0\r\n"
    << "\r\n";
  server.flush();

  std::cout << "* Recieve response" << std::endl;
  {
    std::unique_ptr<std::istream> response_body_stream = cs.parse_respose();
    std::cout << cs.response_version() << " " << cs.response_code() << " " << cs.response_code_message() << std::endl;
    cs.response_header().write(std::cout);

    std::vector<char> buffer(1024);
    response_body_stream->read(buffer.data(), buffer.size());
    std::string response_body(buffer.data(), buffer.data() + response_body_stream->gcount());
    std::cout << response_body << std::endl;
  }
}
