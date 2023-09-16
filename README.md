# misc library for C++

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


### test.hpp

see `test/*_test.cpp`

## Author

TAKEI Yuya (https://github.com/takei-yuya)
