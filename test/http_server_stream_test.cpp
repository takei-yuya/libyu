#include "yu/http/server_stream.hpp"

#include "yu/test.hpp"

#include <sys/socket.h>
#include <string.h>

#include "yu/stream/fdstream.hpp"

class HTTPServerStreamTest : public yu::Test {
 public:
  HTTPServerStreamTest() : yu::Test(), user_agent(), server() {}

  void prepare() override {
    int ret = ::socketpair(AF_UNIX, SOCK_STREAM, 0, fds);
    if (ret < 0) throw std::runtime_error(strerror(errno));
    user_agent.reset(new yu::stream::fdstream(fds[0]));
    server.reset(new yu::stream::fdstream(fds[1]));
  }
  void teardown() override {
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

TEST(HTTPServerStreamTest, testSimple) {
  // Request
  *user_agent
    << "GET /test HTTP/1.1\r\n"
    << "Host: example.com\r\n"
    << "User-Agent: test\r\n"
    << "\r\n";
  user_agent->flush();

  yu::http::ServerStream ss(*server);

  ss.parse_request();
  EXPECT("GET", ==, ss.request_method());
  EXPECT("/test", ==, ss.request_target());
  EXPECT("HTTP/1.1", ==, ss.request_version());

  EXPECT("example.com", ==, ss.request_header().field("Host"));
  EXPECT("test", ==, ss.request_header().field("User-Agent"));

  // Response
  ss.set_status(200);
  ss.set_header("Server", "libyu");
  ss.set_header("Set-Cookie", "foo=1; Path=/");
  ss.set_header("Server", "version 0.0.0");
  ss.set_header("Set-Cookie", "bar=2; Path=/");
  {
    auto out = ss.send_header();
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
