#include "yu/http/server_stream.hpp"

#include "yu/test.hpp"

#include <sys/socket.h>
#include <string.h>

#include "yu/stream/fdstream.hpp"

class HTTPStreamTest : public yu::Test {
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

TEST(HTTPStreamTest, testSimple) {
  *user_agent
    << "GET /test HTTP/1.1\r\n"
    << "Host: example.com\r\n"
    << "User-Agent: test\r\n"
    << "\r\n";
  user_agent->flush();

  yu::http::ServerStream hs(*server);

  hs.parse_request();
  EXPECT("GET", ==, hs.request_method());
  EXPECT("/test", ==, hs.request_target());
  EXPECT("HTTP/1.1", ==, hs.request_version());
  std::unordered_map<std::string, std::string> expected_headers = { { "Host", "example.com" }, { "User-Agent", "test" } };
  EXPECT(expected_headers, ==, hs.request_headers());

  hs.set_status(200);
  hs.set_header("Server", "libyu");
  hs.set_header("Set-Cookie", "foo=1; Path=/");
  hs.set_header("Server", "version 0.0.0");
  hs.set_header("Set-Cookie", "bar=2; Path=/");
  {
    auto out = hs.send_header();
    *out << "Hello World!";
  }

  std::string line;
  std::getline(*user_agent, line); EXPECT("HTTP/1.1 200 OK\r", ==, line);
  std::getline(*user_agent, line); EXPECT("Set-Cookie: foo=1; Path=/\r", ==, line);
  std::getline(*user_agent, line); EXPECT("Set-Cookie: bar=2; Path=/\r", ==, line);
  std::getline(*user_agent, line); EXPECT("Server: libyu,version 0.0.0\r", ==, line);
  std::getline(*user_agent, line); EXPECT("Transfer-Encoding: chunked\r", ==, line);
  std::getline(*user_agent, line); EXPECT("\r", ==, line);
  std::getline(*user_agent, line); EXPECT("C\r", ==, line);
  std::getline(*user_agent, line); EXPECT("Hello World!\r", ==, line);
  std::getline(*user_agent, line); EXPECT("0\r", ==, line);
}
