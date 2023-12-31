// Copyright 2023 TAKEI Yuya (https://github.com/takei-yuya/)
#ifndef YU_JSON_HPP_
#define YU_JSON_HPP_

#include <cmath>
#include <iomanip>
#include <limits>
#include <map>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "utf8.hpp"

namespace yu {
namespace json {

class InvalidJson : public std::runtime_error {
 public:
  explicit InvalidJson(const std::string& message) : std::runtime_error(message.c_str()) {}
};

#if __cplusplus >= 201703L
#define NO_DISCARD [[nodiscard]]
#else
#define NO_DISCARD
#endif

class Stringifier {
 public:
  explicit Stringifier(std::ostream& out, bool pretty = false, bool ascii_only = false)
    : is_first_(), out_(out), pretty_(pretty), ascii_only_(ascii_only) {}

  template <typename T>
  void stringify(const T& value) {
    value.stringifyJson(*this);
  }

  #define JSON_STRINGIFY_VIA_STREAM(TYPE) void stringify(const TYPE& num) { out_ << num; }
  JSON_STRINGIFY_VIA_STREAM(int);
  JSON_STRINGIFY_VIA_STREAM(long);
  JSON_STRINGIFY_VIA_STREAM(long long);
  JSON_STRINGIFY_VIA_STREAM(unsigned int);
  JSON_STRINGIFY_VIA_STREAM(unsigned long);
  JSON_STRINGIFY_VIA_STREAM(unsigned long long);
  #undef JSON_STRINGIFY_VIA_STREAM

  // JSONではNaNや無限大は許容されていない
  #define JSON_STRINGIFY_VIA_STREAM(TYPE) \
    void stringify(TYPE num) { \
      if (std::isnan(num)) throw InvalidJson("NaN not allowed in JSON"); \
      if (std::isinf(num)) throw InvalidJson("Infinity not allowed in JSON"); \
      out_ << num; \
    }
  JSON_STRINGIFY_VIA_STREAM(float);
  JSON_STRINGIFY_VIA_STREAM(double);
  JSON_STRINGIFY_VIA_STREAM(long double);
  #undef JSON_STRINGIFY_VIA_STREAM

  void stringify(bool t) {
    out_ << (t ? "true" : "false");
  }

  void stringify(const std::string& str) {
    out_ << '"';
    std::istringstream iss(str);
    yu::utf8::Decoder decoder(iss);
    while (decoder.has_next()) {
      uint32_t cp = decoder.next();
      std::string named_escape_char = namedEscapeChar(cp);
      if (named_escape_char != "") {
        out_ << named_escape_char;
      } else if (isControlChar(cp) || (ascii_only_ && cp > 0x7F)) {
        std::ostringstream oss;  // do not set iomanip to out_, keep out_ default format
        oss << "\\u" << std::hex << std::setfill('0') << std::setw(4) << cp;
        out_ << oss.str();
      } else {
        out_ << yu::utf8::encode(cp);
      }
    }
    out_ << '"';
  }

  void stringify(const char* str) {
    return stringify(std::string(str));
  }

  template <typename T>
  void stringify(const std::vector<T>& vec) {
    beginArray();
    for (const auto& it : vec) {
      valueDelim();
      stringify(it);
    }
    endArray();
  }

  void stringify(const std::vector<bool>& vec) {
    beginArray();
    for (bool it : vec) {
      valueDelim();
      stringify(it);
    }
    endArray();
  }

  template <typename T>
  void stringify(const std::map<std::string, T>& map) {
    beginObject();
    for (const auto& it : map) {
      valueDelim();
      stringify(it.first);
      keyDelim();
      stringify(it.second);
    }
    endObject();
  }

  template <typename T>
  void stringify(const std::unordered_map<std::string, T>& map) {
    beginObject();
    for (const auto& it : map) {
      valueDelim();
      stringify(it.first);
      keyDelim();
      stringify(it.second);
    }
    endObject();
  }

  template <class T>
  class MemberStringifier {
   public:
    MemberStringifier(Stringifier& stringifier) : stringifier_(stringifier), getters_(), done_(false) {}

    class MemberGetter {
     public:
      MemberGetter(const std::string& name, void (*stringify)(Stringifier&, T&)) : name(name), stringify(stringify) {}

     private:
      friend MemberStringifier<T>;
      std::string name;
      void (*stringify)(Stringifier&, T&);
    };

    NO_DISCARD MemberStringifier<T>& operator<<(MemberGetter&& getter) {
      if (done_) throw std::runtime_error("Member stringifier was already executed");
      getters_.emplace_back(getter.name, getter.stringify);
      return *this;
    }

    class Stringify {
     public:
      explicit Stringify(const T& obj) : obj_(obj) {}
     private:
      friend MemberStringifier<T>;
      const T& obj_;
    };

    void operator<<(Stringify&& stringify) {
      if (done_) throw std::runtime_error("Member stringifier was already executed");
      done_ = true;

      stringifier_.beginObject();
      for (auto& it : getters_) {
        stringifier_.valueDelim();
        stringifier_.stringify(it.first);
        stringifier_.keyDelim();
        it.second(stringifier_, stringify.obj_);
      }
      stringifier_.endObject();
    }

   private:
    Stringifier& stringifier_;
    std::vector<std::pair<std::string, void (*)(Stringifier&, T&)>> getters_;
    bool done_;
  };

 private:
  std::string namedEscapeChar(uint32_t cp) {
    if (cp == 0x22) return "\\\"";
    if (cp == 0x5c) return "\\\\";
    if (cp == 0x2f) return "\\/";
    if (cp == 0x08) return "\\b";
    if (cp == 0x0c) return "\\f";
    if (cp == 0x0a) return "\\n";
    if (cp == 0x0d) return "\\r";
    if (cp == 0x09) return "\\t";
    return "";
  }

  bool isControlChar(uint32_t cp) {
    return (cp <= 0x1F)             // C0 control characters
      || cp == 0x7F                 // DEL
      || (0x80 <= cp && cp <= 0x9F) // C1 control characters
      || cp == 0x2028               // LINE SEPARATOR (JSONエスケープは不要だが、安全のため常にエスケープする)
      || cp == 0x2029               // PARAGRAPH SEPARATOR (JSONエスケープは不要だが、安全のため常にエスケープする)
      || cp == 0xFEFF;              // BOM (JSONエスケープは不要だが、安全のため常にエスケープする)
  }

  void beginObject() {
    out_ << "{";
    is_first_.push_back(true);
    newline_indent();
  }

  void endObject() {
    is_first_.pop_back();
    newline_indent();
    out_ << "}";
  }

  void beginArray() {
    out_ << "[";
    is_first_.push_back(true);
    newline_indent();
  }

  void endArray() {
    is_first_.pop_back();
    newline_indent();
    out_ << "]";
  }

  void valueDelim() {
    if (!is_first_.back()) {
      out_ << ",";
      newline_indent();
    }
    is_first_.back() = false;
  }

  void keyDelim() {
    out_ << ":";
  }

  void newline_indent() {
    if (!pretty_) return;
    out_ << "\n" << std::string(is_first_.size() * 2, ' ');
  }

 private:
  std::vector<bool> is_first_;
  std::ostream& out_;
  bool pretty_;
  bool ascii_only_;
};

class Parser {
 public:
  explicit Parser(std::istream& in) : in_(in) {}

  template <typename T>
  void parse(std::ostream& out, const T& v);

  template <typename T>
  void parse(T& value) {
    value.parseJson(*this);
  }

  // TODO: 小数表記を整数としてパースしようとした時どうするか？エラーにする or 黙って小数点以下切り捨て
  // long doubleがdouble-extendedならuint64_tの値でも正しく保持できるが、そうとも限らないので、指数表記があるときだけ、long dounle を経由してパースする
  // そのまま整数値としてパースしようとすると、整数部だけパースされ、0.1e1が0になってしまう。
  // stou* 系は負数を符号反転してout_of_range扱いしてくれない……。
#define JSON_PARSE_NUMBER(type, func) \
  void parse(type& v) try { \
    std::string number_part = getNumberPart(); \
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
  void parse(type& v) try { \
    v = func(getNumberPart()); \
  } catch (const std::out_of_range& e) { \
    throw InvalidJson("number out of range"); \
  }
  JSON_PARSE_NUMBER(float, std::stof)
  JSON_PARSE_NUMBER(double, std::stod)
  JSON_PARSE_NUMBER(long double, std::stold)
#undef JSON_PARSE_NUMBER

  void parse(bool& t) {
    if (expectAny("tf", true) == 't') {
      expect('r');
      expect('u');
      expect('e');
      t = true;
    } else {
      expect('a');
      expect('l');
      expect('s');
      expect('e');
      t = false;
    }
    ws();
  }

  void parse(std::string& out) {
    std::string result;
    expect('"', true);
    while (1) {
      char ch = tryGet();
      if (ch == '"') break;
      if (ch == '\\') {
        ch = tryGet();
        if (ch == '"' || ch == '\\' || ch == '/') { result += ch; }
        else if (ch == 'b') { result += '\b'; }
        else if (ch == 'f') { result += '\f'; }
        else if (ch == 'n') { result += '\n'; }
        else if (ch == 'r') { result += '\r'; }
        else if (ch == 't') { result += '\t'; }
        else if (ch == 'u') {
          uint32_t cp = parse4Hexs();
          // surrogate
          if (0xD800 <= cp && cp <= 0xDBFF) {
            expect('\\');
            expect('u');
            uint32_t cp2 = parse4Hexs();
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
          throw InvalidJson("unexpected escape " + std::string(1, ch));
        }
      } else {
        result += ch;
      }
    }
    ws();
    out.swap(result);
  }

  template <typename T>
  void parse(std::vector<T>& vec) {
    std::vector<T> result;
    expect('[', true);
    if (tryPeek(true) == ']') { tryGet(); }
    else while (1) {
      T value;
      parse(value);
      result.push_back(value);
      if (expectAny(",]", true) == ']') break;
    }
    ws();
    vec.swap(result);
  }

  template <typename T>
  void parse(std::map<std::string, T>& map) {
    std::map<std::string, T> result;
    expect('{', true);
    if (tryPeek(true) == '}') { tryGet(); }
    else while (1) {
      // TODO: key重複の扱い
      std::string key;
      parse(key);
      expect(':', true);
      T value;
      parse(value);
      result[key] = value;
      if (expectAny(",}", true) == '}') break;
    }
    ws();
    map.swap(result);
  }

  template <typename T>
  void parse(std::unordered_map<std::string, T>& map) {
    std::unordered_map<std::string, T> result;
    expect('{', true);
    if (tryPeek(true) == '}') { tryGet(); }
    else while (1) {
      // TODO: key重複の扱い
      std::string key;
      parse(key);
      expect(':', true);
      T value;
      parse(value);
      result[key] = value;
      if (expectAny(",}", true) == '}') break;
    }
    ws();
    map.swap(result);
  }

  // skip a single value
  void parseAny() {
    int ch = expectAny("{[\"-0123456789tf", true);
    if (ch == '{') {
      if (tryPeek(true) == '}') { tryGet(); }
      else while (1) {
        std::string key;  // key must be string
        parse(key);
        expect(':', true);
        parseAny();  // any value
        if (expectAny(",}", true) == '}') break;
      }
    } else if (ch == '[') {
      if (tryPeek(true) == ']') { tryGet(); }
      else while (1) {
        parseAny();  // any value
        if (expectAny(",]", true) == ']') break;
      }
    } else if (ch == '"') {
      in_.unget();
      std::string s;
      parse(s);
    } else if (ch == 't' || ch == 'f') {
      in_.unget();
      bool t;
      parse(t);
    } else {  // number
      in_.unget();
      double d;
      parse(d);
    }
    ws();
  }

  template <class T>
  class MemberParser {
   public:
    MemberParser(Parser& parser) : parser_(parser), setters_(), done_(false) {}

    class MemberSetter {
     public:
      MemberSetter(const std::string& name, void (*parse)(Parser&, T&)) : name(name), parse(parse) {}

     private:
      friend MemberParser<T>;
      std::string name;
      void (*parse)(Parser&, T&);
    };

    NO_DISCARD MemberParser<T>& operator<<(MemberSetter&& setter) {
      if (done_) throw std::runtime_error("Member parse was already executed");
      setters_[setter.name] = setter.parse;
      return *this;
    }

    class Parse {
     public:
      explicit Parse(T& obj) : obj_(obj) {}
     private:
      friend MemberParser<T>;
      T& obj_;
    };

    void operator<<(Parse&& parse) {
      if (done_) throw std::runtime_error("Member parse was already executed");
      done_ = true;
      parser_.expect('{', true);
      if (parser_.tryPeek(true) == '}') { parser_.tryGet(); }
      else while (1) {
        std::string name;
        parser_.parse(name);
        parser_.expect(':', true);
        auto it = setters_.find(name);
        if (it != setters_.end()) {
          it->second(parser_, parse.obj_);
        } else {
          parser_.parseAny();  // 値を読み飛ばす
        }
        if (parser_.expectAny("},", true) == '}') break;
      }
      parser_.ws();
    }

   private:
    Parser& parser_;
    std::unordered_map<std::string, void (*)(Parser&, T&)> setters_;
    bool done_;
  };

 private:
  bool isJsonWhiteSpace(char ch) {
    return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
  }

  void ws() {
    while (in_.peek() != EOF && isJsonWhiteSpace(static_cast<char>(in_.peek()))) in_.get();
  }

  char tryGet() {
    int ch = in_.get();
    if (ch == EOF) throw InvalidJson("unexpected EOF");
    return static_cast<char>(ch);
  }

  char tryPeek(bool skipPreWhiteSpace = false) {
    if (skipPreWhiteSpace) ws();
    int ch = in_.peek();
    if (ch == EOF) throw InvalidJson("unexpected EOF");
    return static_cast<char>(ch);
  }

  char expect(char expectedChar, bool skipPreWhiteSpace = false) {
    if (skipPreWhiteSpace) ws();
    char ch = tryGet();
    if (ch != expectedChar) throw InvalidJson("expect '" + std::string(1, expectedChar) + "', but '" + std::string(1, ch) + "'");
    return ch;
  }

  char expectAny(const std::string& expectedChars, bool skipPreWhiteSpace = false) {
    if (skipPreWhiteSpace) ws();
    char ch = tryGet();
    if (expectedChars.find(ch) == std::string::npos) throw InvalidJson("expect one of '" + expectedChars + "', but '" + std::string(1, ch) + "'");
    return ch;
  }

  std::string getNumberPart() {
    std::string result;
    //  符号部
    int ch = expectAny("-0123456789", true);
    if (ch == '-') { result += static_cast<char>(ch); ch = expectAny("0123456789"); }

    // 整数部
    if (ch == '0') {
      result += static_cast<char>(ch);
      ch = in_.get();
    } else {  // leading zeros are not allowed
      result += static_cast<char>(ch);
      while ((ch = in_.get()) != EOF) {
        if (ch == '.') { break; }
        if (ch == 'e' || ch == 'E') { break; }
        if (!('0' <= ch && ch <= '9')) { break; }  // 数値の終わり
        result += static_cast<char>(ch);
      }
    }

    // 小数部
    if (ch == '.') {
      result += static_cast<char>(ch);
      ch = expectAny("0123456789");  // 小数部は少なくとも1桁
      result += static_cast<char>(ch);
      while ((ch = in_.get()) != EOF) {
        if (ch == 'e' || ch == 'E') { break; }
        if (!('0' <= ch && ch <= '9')) { break; }  // 数値の終わり
        result += static_cast<char>(ch);
      }
    }

    // 指数部
    if (ch == 'e' || ch == 'E') {
      result += static_cast<char>(ch);
      ch = expectAny("+-0123456789");  // 指数部は少なくとも1桁
      if (ch == '+' || ch == '-') { result += static_cast<char>(ch); ch = expectAny("0123456789"); }
      result += static_cast<char>(ch);
      while ((ch = in_.get()) != EOF) {
        if (!('0' <= ch && ch <= '9')) { break; }  // 数値の終わり
        result += static_cast<char>(ch);
      }
    }

    // 基本的に先読みしているのでungetが必要
    in_.unget();
    ws();
    return result;
  }

  uint16_t parseHex() {
    char ch = tryGet();
    if ('0' <= ch && ch <= '9') return static_cast<uint16_t>(ch - '0');
    if ('a' <= ch && ch <= 'f') return static_cast<uint16_t>(ch - 'a' + 10);
    if ('A' <= ch && ch <= 'F') return static_cast<uint16_t>(ch - 'A' + 10);
    throw InvalidJson("expect HEX, but '" + std::string(1, ch) + "'");
  }

  uint16_t parse4Hexs() {
    uint16_t ch1 = parseHex();
    uint16_t ch2 = parseHex();
    uint16_t ch3 = parseHex();
    uint16_t ch4 = parseHex();
    return static_cast<uint16_t>(ch1 << 12 | ch2 << 8 | ch3 << 4 | ch4);
  }

  // なぜstd::stou が標準にないのか……。
  unsigned int stou(const std::string& s) {
    unsigned long ul = std::stoul(s);
    if (std::numeric_limits<unsigned int>::max() < ul) {
      throw std::out_of_range("stou");
    }
    return static_cast<unsigned int>(ul);
  }

  std::istream& in_;
};

#undef NO_DISCARD

//
// === stream interface ===
//

template <typename T>
inline void stringify(std::ostream& out, const T& val, bool pretty = false, bool ascii_only = false) {
  Stringifier stringifier(out, pretty, ascii_only);
  stringifier.stringify(val);
}

template <typename T>
inline void parse(std::istream& in, T& val) {
  Parser parser(in);
  parser.parse(val);
}

//
// === string interface ===
//

template <typename T>
inline std::string to_json(const T& v, bool pretty = false, bool ascii_only = false) {
  std::ostringstream oss;
  stringify(oss, v, pretty, ascii_only);
  return oss.str();
}

template <typename T>
inline T from_json(const std::string& str) {
  std::istringstream iss(str);
  T result;
  parse(iss, result);
  // check rest part
  int ch = iss.get();
  if (ch != EOF) {
    std::string trails(1, static_cast<char>(ch));
    while ((ch = iss.get()) != EOF) trails += static_cast<char>(ch);
    throw InvalidJson("unexpected rest part: rest = " + trails);
  }
  return result;
}

// Usage
// class Klass {
//  private:
//   std::string str;
//   int num;
//
//  pubic;
//   void stringifyJson(yu::json::Stringifier& stringifier) {
//     JSON_MEMBER_STRINGIFIER(stringifier)
//       << JSON_GETTER(str)
//       << JSON_NAMED_GETTER("number", num)
//       << JSON_STRINGIFY;
//   }
//   void parseJson(yu::json::Parser& parser) {
//     JSON_MEMBER_PARSER(parser)
//       << JSON_SETTER(str)
//       << JSON_NAMED_SETTER("number", num)
//       << JSON_PARSE;
//   }
// }

#define JSON_MEMBER_STRINGIFIER(stringifier) \
  yu::json::Stringifier::MemberStringifier<std::remove_reference<decltype(*this)>::type>(stringifier)
#define JSON_GETTER(variable) \
  yu::json::Stringifier::MemberStringifier<std::remove_reference<decltype(*this)>::type>::MemberGetter(#variable, [](yu::json::Stringifier& stringifier, decltype(*this)& obj){ stringifier.stringify(obj.variable); })
#define JSON_NAMED_GETTER(name, variable) \
  yu::json::Stringifier::MemberStringifier<std::remove_reference<decltype(*this)>::type>::MemberGetter(name, [](yu::json::Stringifier& stringifier, decltype(*this)& obj){ stringifier.stringify(obj.variable); })
#define JSON_STRINGIFY \
  yu::json::Stringifier::MemberStringifier<std::remove_reference<decltype(*this)>::type>::Stringify(*this)

#define JSON_MEMBER_PARSER(parser) \
  yu::json::Parser::MemberParser<std::remove_reference<decltype(*this)>::type>(parser)
#define JSON_SETTER(variable) \
  yu::json::Parser::MemberParser<std::remove_reference<decltype(*this)>::type>::MemberSetter(#variable, [](yu::json::Parser& parser, decltype(*this)& obj){ parser.parse(obj.variable); })
#define JSON_NAMED_SETTER(name, variable) \
  yu::json::Parser::MemberParser<std::remove_reference<decltype(*this)>::type>::MemberSetter(name, [](yu::json::Parser& parser, decltype(*this)& obj){ parser.parse(obj.variable); })
#define JSON_PARSE \
  yu::json::Parser::MemberParser<std::remove_reference<decltype(*this)>::type>::Parse(*this)

}  // namespace json
}  // namespace yu

#endif  // YU_JSON_HPP_
