#include "yu/http/client_stream.hpp"

#include "yu/test.hpp"

#include <sys/socket.h>
#include <string.h>

#include "yu/stream/fdstream.hpp"

class HTTPClientStreamTest : public yu::Test {
 public:
  virtual void prepare() {
    int ret = ::socketpair(AF_UNIX, SOCK_STREAM, 0, fds);
    if (ret < 0) throw std::runtime_error(strerror(errno));
    user_agent.reset(new yu::stream::fdstream(fds[0]));
    server.reset(new yu::stream::fdstream(fds[1]));
  }
  virtual void teardown() {
    user_agent.reset();
    server.reset();
    close(fds[0]);
    close(fds[1]);
  }

 protected:
  int fds[2];
  std::unique_ptr<yu::stream::fdstream> user_agent;
  std::unique_ptr<yu::stream::fdstream> server;
};

TEST(HTTPClientStreamTest, testSimple) {
  yu::http::ClientStream cs(*user_agent);

  cs.set_header("Host", "example.com");
  cs.set_header("Test", "foo");
  cs.set_header("User-Agent", "libyu 0.0.0");
  cs.set_header("test", "bar");

  {
    std::unique_ptr<std::ostream> request_body = cs.request("POST", "/greeting");
    *request_body << "Hello World!!";
  }

  std::string line;
  std::getline(*server, line); EXPECT("POST /greeting HTTP/1.1\r", ==, line);
  std::getline(*server, line); EXPECT("Host: example.com\r", ==, line);
  std::getline(*server, line); EXPECT("Test: foo,bar\r", ==, line);
  std::getline(*server, line); EXPECT("Transfer-Encoding: chunked\r", ==, line);
  std::getline(*server, line); EXPECT("User-Agent: libyu 0.0.0\r", ==, line);
  std::getline(*server, line); EXPECT("\r", ==, line);

  *server
    << "HTTP/1.1 200 OK\r\n"
    << "Set-Cookie: foo=1; path=/\r\n"
    << "Server: test\r\n"
    << "Set-Cookie: bar=2; path=/\r\n"
    << "Transfer-Encoding: chunked\r\n"
    << "\r\n"
    << "6\r\n"
    << "Hello \r\n"
    << "7\r\n"
    << "World!!\r\n"
    << "0\r\n"
    << "\r\n";
  server->flush();

  {
    std::unique_ptr<std::istream> response_body = cs.parse_respose();
    EXPECT("HTTP/1.1", ==, cs.response_version());
    EXPECT(200, ==, cs.response_code());
    EXPECT("OK", ==, cs.response_code_message());
    std::vector<char> buffer(1024);
    response_body->read(buffer.data(), buffer.size());
    std::string actual(buffer.data(), buffer.data() + response_body->gcount());
    EXPECT("Hello World!!", ==, actual);
  }
}
