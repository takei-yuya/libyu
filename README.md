# misc libraries for C++

misc header only libiraries depend on only C++ standard libiraries.

## Install

copy `yu` directory to your project, then include it.

## usage

### utf8.hpp

```cpp
namespace yu {
namespace utf8 {
std::string yu::utf8::encode(uint32_t);
}
}
```

example: `sample/utf8_sample.cpp`
```cpp
uint32_t code_point = 0x1f363;
std::string str = yu::utf8::encode(code_point);
std::cout << str << std::endl;
// 🍣
```

### digest/sha2.hpp

```cpp
namespace yu {
namespace digest {
class sha256_stream : public std::ostream {
 public:
  sha256_stream();
  std::string hash_hex();
  std::vector<char> hash_bin();
  std::string hash_base64();
};

std::string sha256_hex(const std::string& str);
std::vector<char> sha256_bin(const std::string& str);
std::string sha256_base64(const std::string& str);

// same as 224, 512 and 384
}
}
```

example: `sample/digest_sha2_sample.cpp`
```cpp
std::string message = "Hello World.";
std::cout << yu::digest::sha256_hex(message) << std::endl;
std::cout << yu::digest::sha224_hex(message) << std::endl;
std::cout << yu::digest::sha512_hex(message) << std::endl;
std::cout << yu::digest::sha384_hex(message) << std::endl;
std::cout << yu::digest::sha512_224_hex(message) << std::endl;
std::cout << yu::digest::sha512_256_hex(message) << std::endl;
// f4bb1975bf1f81f76ce824f7536c1e101a8060a632a52289d530a6f600d52c92
// f871ab68ccdf47a7afb935f9f2f05365a61dee3aa6ebb7ef22be5de1
// fee4e02329c0e1c9005d0590f4773d8e519e0cda859775ac9c83641e3a960c57e7ad461354e4860722b6e3c161e493e04f5ef07d9169ff7bdab659d6a57cc316
// ded020e0ea23fd2d983f7d833c44811f9e3fa96e412f84f7427250af07a5630e26366a69c44bac94fd31ec73b1b847d1
// 53a8f45fd2b7631b90d2c84b5dd223389b90ef503059f4c86fe6857d
// cc296ed308cbe384e0de66c8580b3373ac2ae88dd53a9bd8542df1431e87f01d
```

### lang/lexical_cast.hpp

```cpp
template <typename T1, typename T2>
T1 lexical_cast(const T2& val);
```

example: `sample/lexical_cast_sample.cpp`
```cpp
std::cout << yu::lang::lexical_cast<int>("42") << std::endl;
std::cout << (yu::lang::lexical_cast<std::string>(42) + "0") << std::endl;
// 42
// 420
```

### stream/fdstream.hpp

```cpp
namespace yu {
namespace stream {
class fdstream : public std::iostream {
 public:
  explicit fdstream(int fd);
};
}
}
```

example: `sample/fdstream_sample.cpp`
```cpp
yu::stream::fdstream fds(1);
fds << "Hello World." << std::endl;
// Hello World.
```

### stream/nullstream.hpp

```cpp
namespace yu {
namespace stream {
class nullstream : public std::iostream {
 public:
  nullstream();
};
}
}
```

example: `sample/nullstream_sample.cpp`
```cpp
yu::stream::nullstream ns;
std::vector<char> buffer(1024);
ns.read(buffer.data(), buffer.size());
std::cout << "read = " << ns.gcount() << ", eof = " << ns.eof() << std::endl;
size_t write_count = 0;
for (size_t i = 0; i < 1024 * 1024; ++i) {
  ns.write(buffer.data(), buffer.size());
  write_count += buffer.size();
}
std::cout << "write = " << write_count << std::endl;
// read = 0, eof = 1
// write = 1073741824
```

### stream/teestream.hpp

```cpp
namespace yu {
namespace stream {
class oteestream : public std::ostream {
 public:
  oteestream(std::ostream& out1, std::ostream& out2);
};
}
}
```

example: `sample/teestream_sample.cpp`
```cpp
{
  std::ostringstream out1;
  std::ostringstream out2;
  yu::stream::oteestream ots(out1, out2);
  ots << "Hello " << "T-stream" << " World!";
  std::cout << "out1 = " << out1.str() << std::endl;
  std::cout << "out2 = " << out2.str() << std::endl;
}
{
  std::istringstream iss("Hello World!");
  std::ostringstream oss;
  yu::stream::iteestream its(iss, oss);
  std::string str;
  its >> str;
  std::cout << "str = " << str << std::endl;
  std::cout << "oss = " << oss.str() << std::endl;  // ostream recieve whole buffer
  its >> str;
  std::cout << "str = " << str << std::endl;
}
// out1 = Hello T-stream World!
// out2 = Hello T-stream World!
// str = Hello
// oss = Hello World!
// str = World!
```

### base64.hpp

```cpp
namespace yu {
namespace base64 {
class Encoder {
 public:
  Encoder(size_t wrap_width = 76, char char62 = '+', char char63 = '/', char pad = '=', const std::string& newline = "\n");

  // read from in, encode, and write to out. return CRC24 for Radix-64
  uint32_t encode(std::istream& in, std::ostream& out) const;
};

// base64 encode
std::string encode(const std::string& str, size_t wrap_width = 76);
// base64 encode use '-' and '_' instead of '+' and '/'
std::string encodeURL(const std::string& str, size_t wrap_width = 76);

class Decoder {
 public:
  Decoder(char char62 = '+', char char63 = '/', char pad = '=', const std::string& allowed_white_space = " \r\n");

  // read from in, decode, and write to out. return CRC24 for Radix-64
  uint32_t decode(std::istream& in, std::ostream& out) const;
};

// base64 decode
std::string decode(const std::string& str);
// base64 decode use '-' and '_' instead of '+' and '/'
std::string decodeURL(const std::string& str);
}
}
```

example: `sample/base64_sample.cpp`
```cpp
std::cout << yu::base64::encode("Hello World.") << std::endl;;
std::cout << yu::base64::decode("SGVsbG8gV29ybGQu") << std::endl;
// SGVsbG8gV29ybGQu
// Hello World.
```

### json.hpp

```cpp
class Stringifier {
 public:
  explicit Stringifier(std::ostream& out, bool pretty = false);

  template <typename T>
  void stringify(const T& value);
}

class Parser {
 public:
  explicit Parser(std::istream& in) : in_(in) {}

  template <typename T>
  void parse(std::ostream& out, const T& v);
};

// stream interface
template <typename T>
inline void stringify(std::ostream& out, const T& val);

template <typename T>
inline void parse(std::istream& in, T& val);

// string interface
template <typename T>
inline std::string to_json(const T& v);

template <typename T>
inline T from_json(const std::string& str);

// Macros: Object mappring
Usage
class Klass {
 private:
  std::string str;
  int num;

 pubic;
  void stringifyJson(yu::json::Stringifier& stringifier) {
    JSON_MEMBER_STRINGIFIER(stringifier)
      << JSON_GETTER(str)
      << JSON_NAMED_GETTER("number", num)
      << JSON_STRINGIFY;
  }
  void parseJson(yu::json::Parser& parser) {
    JSON_MEMBER_PARSER(parser)
      << JSON_SETTER(str)
      << JSON_NAMED_SETTER("number", num)
      << JSON_PARSE;
  }
}
```

example: `sample/json_sample.cpp`
```cpp
class Klass {
 public:
  Klass() : str(), num(), map(), vec() {}
 private:
  std::string str;
  int num;
  std::map<std::string, float> map;
  std::vector<bool> vec;
 public:
  void stringifyJson(yu::json::Stringifier& stringifier) const {
    JSON_MEMBER_STRINGIFIER(stringifier)
      << JSON_GETTER(str) << JSON_GETTER(num)
      << JSON_GETTER(map) << JSON_GETTER(vec)
      << JSON_STRINGIFY;
  }
  void parseJson(yu::json::Parser& parser) {
    JSON_MEMBER_PARSER(parser)
      << JSON_SETTER(str) << JSON_SETTER(num)
      << JSON_SETTER(map) << JSON_SETTER(vec)
      << JSON_PARSE;
  }
};
std::string json = R"(
  {
    "str": "Hello World.", "num": 42,
    "map": { "a": 3.14 }, "vec": [ true, false ]
  }
)";
Klass obj = yu::json::from_json<Klass>(json);
std::cout << yu::json::to_json(obj) << std::endl;
// {"str":"Hello World.","num":42,"map":{"a":3.14},"vec":[true,false]}
```

### http/client_stream.hpp

```cpp
namespace yu {
namespace http {
class ClientStream {
 public:
  explicit ClientStream(std::iostream& stream);

  void set_header(const std::string& key, const std::string& value);
  std::unique_ptr<std::ostream> request(const std::string& request_method,
                                        const std::string& request_target,
                                        const std::string& request_version = "HTTP/1.1");

  std::unique_ptr<std::istream> parse_respose();
  const std::string& response_version() const;
  int response_code() const;
  const std::string& response_code_message() const;
  const Header& response_header() const;
};
}
}
```

example: `sample/http_client_stream_sample.cpp`
```cpp
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
// * Send request
// * Recieve request
// POST / HTTP/1.1
// Host: example.com
// Transfer-Encoding: chunked
// 
// D
// Hello World!!
// 0
// * Send response
// * Recieve response
// HTTP/1.1 200 OK
// Set-Cookie: foo=1; path=/
// Server: test
// Transfer-Encoding: chunked
// 
// Hello World!!
```

### http/server_stream.hpp

```cpp
namespace yu {
namespace http {
class ServerStream {
 public:
  explicit ServerStream(std::iostream& stream);

  std::unique_ptr<std::istream> parse_request();
  const std::string& request_method() const;
  const std::string& request_target() const;
  const std::string& request_version() const;
  const std::unordered_map<std::string, std::string>& request_headers() const;

  void set_status(int status, const std::string& messgae = "");
  void set_header(const std::string& key, const std::string& value);
  std::unique_ptr<chunked_ostream> send_header();
};
}
}
```

example: `sample/http_server_stream_sample.cpp`
```cpp
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
  request_body_stream->read(buffer.data(), buffer.size());
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
// * Send request
// * Recieve request
// POST / HTTP/1.1
// Host: example.com
// Transfer-Encoding: chunked
// User-Agent: test
// 
// Hello World!
// * Send response
// * Recieve response
// HTTP/1.1 200 OK
// Set-Cookie: foo=1; path=/
// Set-Cookie: bar=2; path=/
// Server: libyu http::ServerStream,version 0.0.0
// Transfer-Encoding: chunked
// 
// 16
// It's wonderful wordl!!
// 0
```

### test.hpp

see `test/*_test.cpp`

## Q&A

- なんで今更C++?
    - 書きたかったから
- base64とかOpenSSLとかにあるよ？
    - 書きたかったから書いた
- 実装の優先度は？
    - 書きたくなった順
- バグを見つけた/実装がいけていない
    - つ Pull Request

## LICENSE

see LICENSE file

## Author

Copyright 2023 TAKEI Yuya (https://github.com/takei-yuya)
