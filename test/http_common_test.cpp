#include "yu/http/common.hpp"

#include "yu/test.hpp"

#include <sstream>

class HTTPCommonTest : public yu::Test {
};

TEST(HTTPCommonTest, testHeaderSetGet) {
  yu::http::Header header;
  EXPECT(false, ==, header.has("Test"));
  EXPECT("", ==, header.field("Test"));

  header.add("Test", "foo");
  EXPECT(true, ==, header.has("Test"));
  EXPECT("foo", ==, header.field("Test"));
  EXPECT("foo", ==, header.field("tesT"));  // ignore case

  EXPECT(std::vector<std::string>({ "Test" }), ==, header.field_names());

  header.add("test", "bar");
  EXPECT(true, ==, header.has("Test"));
  EXPECT("foo,bar", ==, header.field("Test"));  // field folding

  EXPECT(std::vector<std::string>({ "Test" }), ==, header.field_names());

  header.add("test", " baz ");
  EXPECT(true, ==, header.has("Test"));
  EXPECT("foo,bar,baz", ==, header.field("Test"));  // striped

  EXPECT(std::vector<std::string>({ "Test" }), ==, header.field_names());  // no dup

  // Set-Cookie
  header.add("Set-Cookie", "foo=1; path=/");
  EXPECT(false, ==, header.has("Set-Cookie"));
  std::vector<std::string> expected_set_cookies = { "foo=1; path=/" };
  EXPECT(expected_set_cookies, ==, header.get_set_cookies());

  header.add("Set-Cookie", "bar=2; path=/");
  EXPECT(false, ==, header.has("Set-Cookie"));
  expected_set_cookies.push_back("bar=2; path=/");
  EXPECT(expected_set_cookies, ==, header.get_set_cookies());
}

TEST(HTTPCommonTest, testHeaderReadWeite) {
  std::ostringstream oss;
  oss
    << "Test: foo\r\n"
    << "Set-Cookie: foo=1; path=/\r\n"
    << "tesT:bar     \r\n"
    << "set-cookie: bar=2; path=/\r\n"
    << "Another: value\r\n"
    << "\r\n";
  std::istringstream iss(oss.str());
  yu::http::Header header;
  header.read(iss);

  EXPECT(true, ==, header.has("Test"));
  EXPECT("foo,bar", ==, header.field("test"));
  std::vector<std::string> expected_set_cookies = { "foo=1; path=/", "bar=2; path=/" };
  EXPECT(expected_set_cookies, ==, header.get_set_cookies());
  EXPECT(std::vector<std::string>({ "Another", "Test" }), ==, header.field_names());  // no dup, sorted

  std::ostringstream expected_output;
  expected_output
    << "Set-Cookie: foo=1; path=/\r\n"
    << "Set-Cookie: bar=2; path=/\r\n"
    << "Another: value\r\n"
    << "Test: foo,bar\r\n"
    << "\r\n";
  std::ostringstream actual_output;
  header.write(actual_output);
  EXPECT(expected_output.str(), ==, actual_output.str());
}

TEST(HTTPCommonTest, testChunkedOStream_chunked) {
  std::ostringstream oss;
  {
    yu::http::chunked_ostream cos(oss);
    cos << "Hello ";
    cos.flush();
    cos << "World!!";
    cos.flush();

    EXPECT("6\r\nHello \r\n7\r\nWorld!!\r\n", ==, oss.str());
  }
  EXPECT("6\r\nHello \r\n7\r\nWorld!!\r\n0\r\n\r\n", ==, oss.str());
}

TEST(HTTPCommonTest, testChunkedOStream_buffered) {
  std::ostringstream oss;
  {
    yu::http::chunked_ostream cos(oss);
    cos << "Hello ";
    cos << "World!!";
  }
  EXPECT("D\r\nHello World!!\r\n0\r\n\r\n", ==, oss.str());
}

TEST(HTTPCommonTest, testChunkedOStream_large) {
  std::string x2048(2048, 'X');

  std::ostringstream oss;
  {
    yu::http::chunked_ostream cos(oss);
    cos.write(x2048.data(), 2048);
  }
  std::string x1024(1024, 'X');
  EXPECT("400\r\n" + x1024 + "\r\n400\r\n" + x1024 + "\r\n0\r\n\r\n", ==, oss.str());
}

TEST(HTTPCommonTest, testChunkedIStream) {
  std::istringstream iss("6\r\nHello \r\n7\r\nWorld!!\r\n0\r\n\r\n");
  yu::http::chunked_istream cis(iss);
  std::vector<char> buffer(1024);
  cis.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
  std::string actual(buffer.data(), buffer.data() + cis.gcount());
  EXPECT("Hello World!!", ==, actual);
}

TEST(HTTPCommonTest, testSizedIStream) {
  std::istringstream iss("Hello World!!");
  yu::http::sized_istream sis(iss, 5);

  std::vector<char> buffer(1024);
  sis.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
  std::string actual(buffer.data(), buffer.data() + sis.gcount());
  EXPECT("Hello", ==, actual);

  iss.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
  std::string actual_rest(buffer.data(), buffer.data() + iss.gcount());
  EXPECT(" World!!", ==, actual_rest);
}
