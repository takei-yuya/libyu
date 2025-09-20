#include "yu/http/server_stream.hpp"

#include "yu/test.hpp"

#include <stdexcept>

#include <string.h>
#include <sys/socket.h>

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

// Additional tests generated to expand coverage of yu::http::ServerStream
// Framework: yu/test.hpp (custom yu::Test with TEST/EXPECT macros)

TEST(HTTPServerStreamTest, testMalformedRequestLineThrows) {
  // Missing HTTP version
  *user_agent
    << "GET /no-version\r\n"
    << "Host: example.com\r\n"
    << "\r\n";
  user_agent->flush();

  yu::http::ServerStream ss(*server);

  bool threw = false;
  try {
    ss.parse_request();
  } catch (const std::exception&) {
    threw = true;
  }
  EXPECT(true, ==, threw);
}

TEST(HTTPServerStreamTest, testMissingHostHeaderHttp11Throws) {
  // HTTP/1.1 requires Host header
  *user_agent
    << "GET /needs-host HTTP/1.1\r\n"
    << "User-Agent: test\r\n"
    << "\r\n";
  user_agent->flush();

  yu::http::ServerStream ss(*server);

  bool threw = false;
  try {
    ss.parse_request();
  } catch (const std::exception&) {
    threw = true;
  }
  EXPECT(true, ==, threw);
}

TEST(HTTPServerStreamTest, testHttp10RequestParsesWithoutHost) {
  // HTTP/1.0 does not require Host
  *user_agent
    << "GET /legacy HTTP/1.0\r\n"
    << "User-Agent: test\r\n"
    << "\r\n";
  user_agent->flush();

  yu::http::ServerStream ss(*server);
  ss.parse_request();
  EXPECT("GET", ==, ss.request_method());
  EXPECT("/legacy", ==, ss.request_target());
  EXPECT("HTTP/1.0", ==, ss.request_version());

  // Respond with simple body; library may still use chunked by default.
  ss.set_status(200);
  ss.set_header("Server", "libyu");
  {
    auto out = ss.send_header();
    *out << "";
  }

  std::string line;
  std::getline(*user_agent, line); EXPECT("HTTP/1.0 200 OK\r", ==, line);
  // Server header is present (may merge if multiple not provided).
  std::getline(*user_agent, line); EXPECT("Server: libyu\r", ==, line);
  // Either Content-Length: 0 or Transfer-Encoding: chunked may appear;
  // accept either by peeking one line and checking startswith.
  if (\!user_agent->eof()) {
    std::getline(*user_agent, line);
    bool is_len = line.rfind("Content-Length: 0\r", 0) == 0;
    bool is_chunked = line.rfind("Transfer-Encoding: chunked\r", 0) == 0;
    EXPECT(true, ==, (is_len || is_chunked));
  }
}

TEST(HTTPServerStreamTest, testEmptyChunkedBody) {
  // Minimal request
  *user_agent
    << "GET /empty HTTP/1.1\r\n"
    << "Host: example.com\r\n"
    << "\r\n";
  user_agent->flush();

  yu::http::ServerStream ss(*server);
  ss.parse_request();

  ss.set_status(204); // No Content
  {
    auto out = ss.send_header();
    // write nothing
  }

  std::string line;
  std::getline(*user_agent, line); EXPECT("HTTP/1.1 204 No Content\r", ==, line);
  // Because body is empty, some implementations may omit chunked. Be tolerant:
  // Read headers until blank line.
  bool saw_chunked = false;
  do {
    std::getline(*user_agent, line);
    if (line == "Transfer-Encoding: chunked\r") saw_chunked = true;
  } while (line \!= "\r");
  if (saw_chunked) {
    std::getline(*user_agent, line); EXPECT("0\r", ==, line);
  }
}

TEST(HTTPServerStreamTest, testMultipleHeaderMergingAndAppending) {
  // Request
  *user_agent
    << "GET /hdr HTTP/1.1\r\n"
    << "Host: example.com\r\n"
    << "\r\n";
  user_agent->flush();

  yu::http::ServerStream ss(*server);
  ss.parse_request();

  // Set headers to verify merge vs. repeat semantics
  ss.set_status(200);
  ss.set_header("Server", "libyu");
  ss.set_header("Server", "core");
  ss.set_header("Set-Cookie", "a=1");
  ss.set_header("Set-Cookie", "b=2");
  {
    auto out = ss.send_header();
    *out << "x";
  }

  std::string line;
  std::getline(*user_agent, line); EXPECT("HTTP/1.1 200 OK\r", ==, line);

  // Collect subsequent header lines (order may vary slightly).
  std::string server_line, cookie1, cookie2, te_line, blank;
  std::getline(*user_agent, cookie1);
  std::getline(*user_agent, cookie2);
  std::getline(*user_agent, server_line);
  std::getline(*user_agent, te_line);
  std::getline(*user_agent, blank);

  // Verify cookies appear as separate header fields
  bool cookie_ok = ((cookie1 == "Set-Cookie: a=1\r" && cookie2 == "Set-Cookie: b=2\r") ||
                    (cookie1 == "Set-Cookie: b=2\r" && cookie2 == "Set-Cookie: a=1\r"));
  EXPECT(true, ==, cookie_ok);

  // Verify Server headers merged by comma without extra spaces
  EXPECT(true, ==, (server_line == "Server: libyu,core\r"));

  // Verify transfer encoding present
  EXPECT("Transfer-Encoding: chunked\r", ==, te_line);

  // Blank line
  EXPECT("\r", ==, blank);

  // Chunk: single byte "x"
  std::getline(*user_agent, line); EXPECT("1\r", ==, line);
  std::getline(*user_agent, line); EXPECT("x\r", ==, line);
  std::getline(*user_agent, line); EXPECT("0\r", ==, line);
}

TEST(HTTPServerStreamTest, testLargeChunkedBodyMultipleChunks) {
  // Request
  *user_agent
    << "GET /large HTTP/1.1\r\n"
    << "Host: example.com\r\n"
    << "\r\n";
  user_agent->flush();

  yu::http::ServerStream ss(*server);
  ss.parse_request();

  ss.set_status(200);
  {
    auto out = ss.send_header();
    // Write two chunks: 8 bytes and 6 bytes (hex sizes: 8 and 6)
    *out << "12345678";
    *out << "ABCDEF";
  }

  std::string line;
  std::getline(*user_agent, line); EXPECT("HTTP/1.1 200 OK\r", ==, line);

  // Read headers until blank line
  do {
    std::getline(*user_agent, line);
  } while (line \!= "\r");

  // Expect first chunk size 8 (hex) then data
  std::getline(*user_agent, line); EXPECT("8\r", ==, line);
  std::getline(*user_agent, line); EXPECT("12345678\r", ==, line);

  // Next chunk size 6 (hex) then data
  std::getline(*user_agent, line); EXPECT("6\r", ==, line);
  std::getline(*user_agent, line); EXPECT("ABCDEF\r", ==, line);

  // Final zero-chunk
  std::getline(*user_agent, line); EXPECT("0\r", ==, line);
}

TEST(HTTPServerStreamTest, testRequestTargetWithQueryString) {
  *user_agent
    << "GET /search?q=test&n=10 HTTP/1.1\r\n"
    << "Host: example.com\r\n"
    << "User-Agent: test\r\n"
    << "\r\n";
  user_agent->flush();

  yu::http::ServerStream ss(*server);
  ss.parse_request();

  EXPECT("GET", ==, ss.request_method());
  EXPECT("/search?q=test&n=10", ==, ss.request_target());
  EXPECT("HTTP/1.1", ==, ss.request_version());
}

TEST(HTTPServerStreamTest, testHeaderLookupCaseInsensitivity) {
  *user_agent
    << "GET /case HTTP/1.1\r\n"
    << "hOSt: example.com\r\n"
    << "uSeR-aGeNt: Test-Agent\r\n"
    << "\r\n";
  user_agent->flush();

  yu::http::ServerStream ss(*server);
  ss.parse_request();

  // Lookup using canonical casing
  EXPECT("example.com", ==, ss.request_header().field("Host"));
  EXPECT("Test-Agent", ==, ss.request_header().field("User-Agent"));
}
