#ifndef YU_JSON_HTTP_
#define YU_JSON_HTTP_

#include <cmath>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "utf8.hpp"

namespace yu {
namespace json {

class InvalidJson : public std::runtime_error {
 public:
  InvalidJson(const std::string& message) : std::runtime_error(message.c_str()) {}
};

//
// === stringify ===
//

template <typename T>
void stringify(std::ostream& out, const T& v);

template <typename T>
inline void stringify(std::ostream& out, const T& value) {
  value.stringifyJson(out);
}

#define JSON_STRINGIFY_VIA_STREAM(TYPE) inline void stringify(std::ostream& out, TYPE num) { out << num; }
JSON_STRINGIFY_VIA_STREAM(int);
JSON_STRINGIFY_VIA_STREAM(long);
JSON_STRINGIFY_VIA_STREAM(long long);
JSON_STRINGIFY_VIA_STREAM(unsigned int);
JSON_STRINGIFY_VIA_STREAM(unsigned long);
JSON_STRINGIFY_VIA_STREAM(unsigned long long);
#undef JSON_STRINGIFY_VIA_STREAM

// JSONではNaNや無限大は許容されていない
#define JSON_STRINGIFY_VIA_STREAM(TYPE) \
  inline void stringify(std::ostream& out, TYPE num) { \
    if (std::isnan(num)) throw InvalidJson("NaN not allowed in JSON"); \
    if (std::isinf(num)) throw InvalidJson("Infinity not allowed in JSON"); \
    out << num; \
  }
JSON_STRINGIFY_VIA_STREAM(float);
JSON_STRINGIFY_VIA_STREAM(double);
JSON_STRINGIFY_VIA_STREAM(long double);
#undef JSON_STRINGIFY_VIA_STREAM


inline void stringify(std::ostream& out, bool t) {
  out << (t ? "true" : "false");
}

inline void stringify(std::ostream& out, const std::string& str) {
  out << '"';
  for (const char ch : str) {
    if (ch == '\\' || ch == '"') { out << '\\' << ch; }
    else if (ch == '\b') { out << "\\b"; }
    else if (ch == '\f') { out << "\\f"; }
    else if (ch == '\n') { out << "\\n"; }
    else if (ch == '\r') { out << "\\r"; }
    else if (ch == '\t') { out << "\\t"; }
    else if (0x00 <= ch && ch <= 0x1f) { out << "\\u" << std::hex << std::setfill('0') << std::setw(4) << static_cast<int>(ch); }
    else { out << ch; }
  }
  out << '"';
}

inline void stringify(std::ostream& out, const char* str) {
  return stringify(out, std::string(str));
}

template <typename T>
void stringify(std::ostream& out, const std::vector<T>& vec) {
  bool is_first = true;
  out << "[";
  for (const auto& it : vec) {
    if (!is_first) { out << ","; }
    is_first = false;
    stringify(out, it);
  }
  out << "]";
}

template <typename T>
void stringify(std::ostream& out, const std::unordered_map<std::string, T>& map) {
  bool is_first = true;
  out << "{";
  for (const auto& it : map) {
    if (!is_first) { out << ","; }
    is_first = false;
    stringify(out, it.first);
    out << ":";
    stringify(out, it.second);
  }
  out << "}";
}

//
// === parse ===
//

template <typename T>
void parse(std::ostream& out, const T& v);

template <typename T>
inline void parse(std::istream& in, T& value) {
  value.parseJson(in);
}

inline bool isJsonWhiteSpace(char ch) {
  return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\r';
}

inline void ws(std::istream& in) {
  while (isJsonWhiteSpace(in.peek())) in.get();
}

inline int tryGet(std::istream& in) {
  int ch = in.get();
  if (ch == EOF) throw InvalidJson("unexpected EOF");
  return ch;
}

inline int tryPeek(std::istream& in, bool skipPreWhiteSpace = false) {
  ws(in);
  int ch = in.peek();
  if (ch == EOF) throw InvalidJson("unexpected EOF");
  return ch;
}

inline int expect(std::istream& in, char expectedChar, bool skipPreWhiteSpace = false) {
  if (skipPreWhiteSpace) ws(in);
  int ch = tryGet(in);
  if (ch != expectedChar) throw InvalidJson("expect '" + std::string(1, expectedChar) + "', but '" + std::string(1, ch) + "'");
  return ch;
}

inline int expectAny(std::istream& in, const std::string& expectedChars, bool skipPreWhiteSpace = false) {
  if (skipPreWhiteSpace) ws(in);
  int ch = tryGet(in);
  if (expectedChars.find(ch) == std::string::npos) throw InvalidJson("expect one of '" + expectedChars + "', but '" + std::string(1, ch) + "'");
  return ch;
}

inline int parseHex(std::istream& in) {
  int ch = tryGet(in);
  if ('0' <= ch && ch <= '9') return ch - '0';
  if ('a' <= ch && ch <= 'f') return ch - 'a' + 10;
  if ('A' <= ch && ch <= 'F') return ch - 'A' + 10;
  throw InvalidJson("expect HEX, but '" + std::string(1,ch) + "'");
}

inline uint16_t parse4Hexs(std::istream& in) {
  int ch1 = parseHex(in);
  int ch2 = parseHex(in);
  int ch3 = parseHex(in);
  int ch4 = parseHex(in);
  return ch1 << 12 | ch2 << 8 | ch3 << 4 | ch4;
}

inline std::string getNumberPart(std::istream& in) {
  std::string result;
  //  符号部
  int ch = expectAny(in, "-0123456789", true);
  if (ch == '-') { result += ch; ch = expectAny(in, "0123456789"); }

  // 整数部
  if (ch == '0') {
    result += ch;
    ch = in.get();
  } else {  // 先頭0は許容されないので、0以外のときだけ続きを処理する
    result += ch;
    while ((ch = in.get()) != EOF) {
      if (ch == '.') { break; }
      if (ch == 'e' || ch == 'E') { break; }
      if (!('0' <= ch && ch <= '9')) { break; }  // 数値の終わり
      result += ch;
    }
  }

  // 小数部
  if (ch == '.') {
    result += ch;
    ch = expectAny(in, "0123456789");  // 小数部は少なくとも1桁
    result += ch;
    while ((ch = in.get()) != EOF) {
      if (ch == 'e' || ch == 'E') { break; }
      if (!('0' <= ch && ch <= '9')) { break; }  // 数値の終わり
      result += ch;
    }
  }

  // 指数部
  if (ch == 'e' || ch == 'E') {
    result += ch;
    ch = expectAny(in, "+-0123456789");  // 指数部は少なくとも1桁
    if (ch == '+' || ch == '-') { result += ch; ch = expectAny(in, "0123456789"); }
    result += ch;
    while ((ch = in.get()) != EOF) {
      if (!('0' <= ch && ch <= '9')) { break; }  // 数値の終わり
      result += ch;
    }
  }

  // 基本的に先読みしているのでungetが必要
  in.unget();
  return result;
}

// なぜstd::stou が標準にないのか……。
inline unsigned int stou(const std::string& s) {
  unsigned long ul = std::stoul(s);
  if (std::numeric_limits<unsigned int>::max() < ul) {
    throw std::out_of_range("stou");
  }
  return ul;
}

// TODO: 小数表記を整数としてパースしようとした時どうするか？エラーにする or 黙って小数点以下切り捨て
// long doubleがdouble-extendedならuint64_tの値でも正しく保持できるが、そうとも限らないので、指数表記があるときだけ、long dounle を経由してパースする
// そのまま整数値としてパースしようとすると、整数部だけパースされ、0.1e1が0になってしまう。
// stou* 系は負数を符号反転してout_of_range扱いしてくれない……。
#define JSON_PARSE_NUMBER(type, func) \
  inline void parse(std::istream& in, type& v) try { \
    std::string number_part = getNumberPart(in); \
    if (std::numeric_limits<type>::min() == 0 && number_part[0] == '-') throw InvalidJson("number out of range"); \
    if (number_part.find('e') == std::string::npos) { \
      v = func(number_part); \
    } else { \
      long double ld = std::stold(number_part); \
      if (ld < std::numeric_limits<type>::min() || std::numeric_limits<type>::max() < ld) { \
        throw InvalidJson("number out of range"); \
      } \
      v = static_cast<type>(ld); \
    } \
  } catch (const std::out_of_range& e) { \
    throw InvalidJson("number out of range"); \
  }
JSON_PARSE_NUMBER(int, std::stoi)
JSON_PARSE_NUMBER(long, std::stol)
JSON_PARSE_NUMBER(long long, std::stoll)
JSON_PARSE_NUMBER(unsigned int, stou)
JSON_PARSE_NUMBER(unsigned long, std::stoul)
JSON_PARSE_NUMBER(unsigned long long, std::stoull)
#undef JSON_PARSE_NUMBER

#define JSON_PARSE_NUMBER(type, func) \
  inline void parse(std::istream& in, type& v) try { \
    v = func(getNumberPart(in)); \
  } catch (const std::out_of_range& e) { \
    throw InvalidJson("number out of range"); \
  }
JSON_PARSE_NUMBER(float, std::stof)
JSON_PARSE_NUMBER(double, std::stod)
JSON_PARSE_NUMBER(long double, std::stold)
#undef JSON_PARSE_NUMBER

inline void parse(std::istream& in, bool& t) {
  if (expectAny(in, "tf", true) == 't') {
    expect(in, 'r');
    expect(in, 'u');
    expect(in, 'e');
    t = true;
  } else {
    expect(in, 'a');
    expect(in, 'l');
    expect(in, 's');
    expect(in, 'e');
    t = false;
  }
}

inline void parse(std::istream& in, std::string& out) {
  std::string result;
  expect(in, '"', true);
  while (1) {
    char ch = tryGet(in);
    if (ch == '"') break;
    if (ch == '\\') {
      ch = tryGet(in);
      if (ch == '"' || ch == '\\' || ch == '/') { result += ch; }
      else if (ch == 'b') { result += '\b'; }
      else if (ch == 'f') { result += '\f'; }
      else if (ch == 'n') { result += '\n'; }
      else if (ch == 'r') { result += '\r'; }
      else if (ch == 't') { result += '\t'; }
      else if (ch == 'u') {
        int cp = parse4Hexs(in);
        // surrogate
        if (0xD800 <= cp && cp <= 0xDBFF) {
          expect(in, '\\');
          expect(in, 'u');
          int cp2 = parse4Hexs(in);
          if (0xDC00 <= cp2 && cp2 <= 0xDFFF) {
            cp = 0x10000 + ((cp - 0xD800) << 10) + (cp2 - 0xDC00);
          } else {
            throw InvalidJson("invalid surrogate: missing low surrogate");
          }
        } else if (0xDC00 <= cp && cp <= 0xDFFF) {
          throw InvalidJson("invalid surrogate: low surrogate without high surrogate");
        }
        result += utf8::encode(cp);
      } else {
        throw InvalidJson("unexpected espcape " + std::string(1, ch));
      }
    } else {
      result += ch;
    }
  }
  out.swap(result);
}

template <typename T>
inline void parse(std::istream& in, std::vector<T>& vec) {
  std::vector<T> result;
  expect(in, '[', true);
  while (1) {
    if (tryPeek(in, true) == ']') { tryGet(in); break; }
    T value;
    parse(in, value);
    result.push_back(value);
    if (expectAny(in, ",]", true) == ']') break;
  }
  vec.swap(result);
}

template <typename T>
inline void parse(std::istream& in, std::unordered_map<std::string, T>& map) {
  std::unordered_map<std::string, T> result;
  expect(in, '{', true);
  while (1) {
    // TODO: key重複の扱い
    if (tryPeek(in, true) == '}') { tryGet(in); break; }
    std::string key;
    parse(in, key);
    expect(in, ':', true);
    T value;
    parse(in, value);
    result[key] = value;
    if (expectAny(in, ",}", true) == '}') break;
  }
  map.swap(result);
}

// skip a single value
inline void parseAny(std::istream& in) {
  int ch = expectAny(in, "{[\"-0123456789tf", true);
  if (ch == '{') {
    while (1) {
      if (tryPeek(in, true) == '}') { tryGet(in); break; }
      std::string key;  // keyは文字列固定
      parse(in, key);
      expect(in, ':', true);
      parseAny(in);  // 値は任意
      if (expectAny(in, ",}", true) == '}') break;
    }
  } else if (ch == '[') {
    while (1) {
      if (tryPeek(in, true) == ']') { tryGet(in); break; }
      parseAny(in);  // 値は任意
      if (expectAny(in, ",]", true) == ']') break;
    }
  } else if (ch == '"') {
    in.unget();
    std::string s;
    parse(in, s);
  } else if (ch == 't' || ch == 'f') {
    in.unget();
    bool t;
    parse(in, t);
  } else {  // number
    in.unget();
    double d;
    parse(in, d);
  }
}

//
// === string interface ===
//

template <typename T>
std::string to_json(const T& v) {
  std::ostringstream oss;
  stringify(oss, v);
  return oss.str();
}

template <typename T>
inline T from_json(const std::string& str) {
  std::istringstream iss(str);
  T result;
  parse(iss, result);
  // check rest part
  ws(iss);
  char ch = iss.get();
  if (ch != EOF) {
    std::string trails(1, ch);
    while ((ch = iss.get()) != EOF) trails += ch;
    throw InvalidJson("unexpected rest part: rest = " + trails);
  }
  return result;
}

//
// === Macro ===
//

// Usage
// class Klass {
//  private:
//   std::string str;
//   int num;
//
//  pubic;
//   void stringifyJson(std::ostream& out) {
//     json::createStringifier(out, *this)
//       << JSON_GETTER(str)
//       << JSON_NAMED_GETTER("number", num);
//   }
//   void parseJson(std::istream& in) {
//     json::createParser(int, *this)
//       << JSON_SETTER(str)
//       << JSON_NAMED_SETTER("number"m num);
//   }
// }


template <class T>
class Stringifier;
template <class T>
class Parser;

namespace detail {
template <class T>
class MemberGetter {
 public:
  MemberGetter(const std::string& name, void (*stringify)(std::ostream&, T&)) : name(name), stringify(stringify) {}

 private:
  friend Stringifier<T>;
  std::string name;
  void (*stringify)(std::ostream&, T&);
};

template <class T>
class MemberSetter {
 public:
  MemberSetter(const std::string& name, void (*parse)(std::istream&, T&)) : name(name), parse(parse) {}

 private:
  friend Parser<T>;
  std::string name;
  void (*parse)(std::istream&, T&);
};
}  // namespace detail

#define JSON_GETTER(variable) \
  yu::json::detail::MemberGetter<std::remove_reference<decltype(*this)>::type>(#variable, [](std::ostream& out, decltype(*this)& obj){ yu::json::stringify(out, obj.variable); })
#define JSON_NAMED_GETTER(name, variable) \
  yu::json::detail::MemberGetter<std::remove_reference<decltype(*this)>::type>(name, [](std::ostream& out, decltype(*this)& obj){ yu::json::stringify(out, obj.variable); })

#define JSON_SETTER(variable) \
  yu::json::detail::MemberSetter<std::remove_reference<decltype(*this)>::type>(#variable, [](std::istream& in, decltype(*this)& obj){ yu::json::parse(in, obj.variable); })
#define JSON_NAMED_SETTER(name, variable) \
  yu::json::detail::MemberSetter<std::remove_reference<decltype(*this)>::type>(name, [](std::istream& in, decltype(*this)& obj){ yu::json::parse(in, obj.variable); })

template <class T>
class Stringifier {
 public:
  Stringifier(std::ostream& out, T& obj) : out_(out), obj_(obj) {}

  // デストラクタで処理するので例外が飛びうる
  ~Stringifier() noexcept(false) {
    out_ << "{";
    bool is_first = true;
    for (auto& it : getters_) {
      if (!is_first) { out_ << ","; }
      is_first = false;
      stringify(out_, it.first);
      out_ << ":";
      it.second(out_, obj_);
    }
    out_ << "}";
  }

  Stringifier<T>& operator<<(detail::MemberGetter<T>&& getter) {
    getters_.emplace_back(getter.name, getter.stringify);
    return *this;
  }

 private:
  std::ostream& out_;
  T& obj_;
  std::vector<std::pair<std::string, void (*)(std::ostream&, T&)>> getters_;
};

template <class T>
Stringifier<T> createStringifier(std::ostream& out, T& obj) {
  return Stringifier<T>(out, obj);
}

template <class T>
class Parser {
 public:
  Parser(std::istream& in, T& obj) : in_(in), obj_(obj) {}

  // デストラクタで処理するので例外が飛びうる
  ~Parser() noexcept(false) {
    expect(in_, '{', true);
    while (1) {
      if (tryPeek(in_, true) == '}') { tryGet(in_); break; }
      std::string name;
      parse(in_, name);
      expect(in_, ':', true);
      auto it = setters_.find(name);
      if (it != setters_.end()) {
        it->second(in_, obj_);
      } else {
        parseAny(in_);  // 値を読み飛ばす
      }
      if (expectAny(in_, "},", true) == '}') break;
    }
  }

  Parser<T>& operator<<(detail::MemberSetter<T>&& setter) {
    setters_[setter.name] = setter.parse;
    return *this;
  }

 private:
  std::istream& in_;
  T& obj_;
  std::unordered_map<std::string, void (*)(std::istream&, T&)> setters_;
};

template <class T>
Parser<T> createParser(std::istream& in, T& obj) {
  return Parser<T>(in, obj);
}

}  // namespace json
}  // namespace yu

#endif  // YU_JSON_HTTP_
