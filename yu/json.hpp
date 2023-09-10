#ifndef YU_JSON_HPP_
#define YU_JSON_HPP_

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

class Stringifier {
 public:
  explicit Stringifier(std::ostream& out) : out_(out) {}

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
    for (const char ch : str) {
      if (ch == '\\' || ch == '"') { out_ << '\\' << ch; }
      else if (ch == '\b') { out_ << "\\b"; }
      else if (ch == '\f') { out_ << "\\f"; }
      else if (ch == '\n') { out_ << "\\n"; }
      else if (ch == '\r') { out_ << "\\r"; }
      else if (ch == '\t') { out_ << "\\t"; }
      else if (0x00 <= ch && ch <= 0x1f) { out_ << "\\u" << std::hex << std::setfill('0') << std::setw(4) << static_cast<int>(ch); }
      else { out_ << ch; }
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
    MemberStringifier(Stringifier& stringifier, T& obj) : stringifier_(stringifier), obj_(obj) {}

    // デストラクタで処理するので例外が飛びうる
    ~MemberStringifier() noexcept(false) {
      stringifier_.beginObject();
      for (auto& it : getters_) {
        stringifier_.valueDelim();
        stringifier_.stringify(it.first);
        stringifier_.keyDelim();
        it.second(stringifier_, obj_);
      }
      stringifier_.endObject();
    }

    class MemberGetter {
     public:
      MemberGetter(const std::string& name, void (*stringify)(Stringifier&, T&)) : name(name), stringify(stringify) {}

     private:
      friend MemberStringifier<T>;
      std::string name;
      void (*stringify)(Stringifier&, T&);
    };

    MemberStringifier<T>& operator<<(MemberGetter&& getter) {
      getters_.emplace_back(getter.name, getter.stringify);
      return *this;
    }

   private:
    Stringifier& stringifier_;
    T& obj_;
    std::vector<std::pair<std::string, void (*)(Stringifier&, T&)>> getters_;
  };

 private:
  void beginObject() {
    out_ << "{";
    is_first_.push_back(true);
  }

  void endObject() {
    out_ << "}";
    is_first_.pop_back();
  }

  void beginArray() {
    out_ << "[";
    is_first_.push_back(true);
  }

  void endArray() {
    out_ << "]";
    is_first_.pop_back();
  }

  void valueDelim() {
    if (!is_first_.back()) out_ << ",";
    is_first_.back() = false;
  }

  void keyDelim() {
    out_ << ":";
  }

 private:
  std::vector<bool> is_first_;
  std::ostream& out_;
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

  std::string getNumberPart() {
    std::string result;
    //  符号部
    int ch = expectAny("-0123456789", true);
    if (ch == '-') { result += ch; ch = expectAny("0123456789"); }

    // 整数部
    if (ch == '0') {
      result += ch;
      ch = in_.get();
    } else {  // leading zeros are not allowed
      result += ch;
      while ((ch = in_.get()) != EOF) {
        if (ch == '.') { break; }
        if (ch == 'e' || ch == 'E') { break; }
        if (!('0' <= ch && ch <= '9')) { break; }  // 数値の終わり
        result += ch;
      }
    }

    // 小数部
    if (ch == '.') {
      result += ch;
      ch = expectAny("0123456789");  // 小数部は少なくとも1桁
      result += ch;
      while ((ch = in_.get()) != EOF) {
        if (ch == 'e' || ch == 'E') { break; }
        if (!('0' <= ch && ch <= '9')) { break; }  // 数値の終わり
        result += ch;
      }
    }

    // 指数部
    if (ch == 'e' || ch == 'E') {
      result += ch;
      ch = expectAny("+-0123456789");  // 指数部は少なくとも1桁
      if (ch == '+' || ch == '-') { result += ch; ch = expectAny("0123456789"); }
      result += ch;
      while ((ch = in_.get()) != EOF) {
        if (!('0' <= ch && ch <= '9')) { break; }  // 数値の終わり
        result += ch;
      }
    }

    // 基本的に先読みしているのでungetが必要
    in_.unget();
    ws();
    return result;
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
          int cp = parse4Hexs();
          // surrogate
          if (0xD800 <= cp && cp <= 0xDBFF) {
            expect('\\');
            expect('u');
            int cp2 = parse4Hexs();
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
    ws();
    out.swap(result);
  }

  template <typename T>
  void parse(std::vector<T>& vec) {
    std::vector<T> result;
    expect('[', true);
    while (1) {
      if (tryPeek(true) == ']') { tryGet(); break; }
      T value;
      parse(value);
      result.push_back(value);
      if (expectAny(",]", true) == ']') break;
    }
    ws();
    vec.swap(result);
  }

  template <typename T>
  void parse(std::unordered_map<std::string, T>& map) {
    std::unordered_map<std::string, T> result;
    expect('{', true);
    while (1) {
      // TODO: key重複の扱い
      if (tryPeek(true) == '}') { tryGet(); break; }
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
      while (1) {
        if (tryPeek(true) == '}') { tryGet(); break; }
        std::string key;  // keyは文字列固定
        parse(key);
        expect(':', true);
        parseAny();  // 値は任意
        if (expectAny(",}", true) == '}') break;
      }
    } else if (ch == '[') {
      while (1) {
        if (tryPeek(true) == ']') { tryGet(); break; }
        parseAny();  // 値は任意
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
    MemberParser(Parser& parser, T& obj) : parser_(parser), obj_(obj) {}

    // デストラクタで処理するので例外が飛びうる
    ~MemberParser() noexcept(false) {
      parser_.expect('{', true);
      while (1) {
        if (parser_.tryPeek(true) == '}') { parser_.tryGet(); break; }
        std::string name;
        parser_.parse(name);
        parser_.expect(':', true);
        auto it = setters_.find(name);
        if (it != setters_.end()) {
          it->second(parser_, obj_);
        } else {
          parser_.parseAny();  // 値を読み飛ばす
        }
        if (parser_.expectAny("},", true) == '}') break;
      }
      parser_.ws();
    }

    class MemberSetter {
     public:
      MemberSetter(const std::string& name, void (*parse)(Parser&, T&)) : name(name), parse(parse) {}

     private:
      friend MemberParser<T>;
      std::string name;
      void (*parse)(Parser&, T&);
    };

    MemberParser<T>& operator<<(MemberSetter&& setter) {
      setters_[setter.name] = setter.parse;
      return *this;
    }

   private:
    Parser& parser_;
    T& obj_;
    std::unordered_map<std::string, void (*)(Parser&, T&)> setters_;
  };

 private:
  bool isJsonWhiteSpace(char ch) {
    return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\r';
  }

  void ws() {
    while (isJsonWhiteSpace(in_.peek())) in_.get();
  }

  int tryGet() {
    int ch = in_.get();
    if (ch == EOF) throw InvalidJson("unexpected EOF");
    return ch;
  }

  int tryPeek(bool skipPreWhiteSpace = false) {
    ws();
    int ch = in_.peek();
    if (ch == EOF) throw InvalidJson("unexpected EOF");
    return ch;
  }

  int expect(char expectedChar, bool skipPreWhiteSpace = false) {
    if (skipPreWhiteSpace) ws();
    int ch = tryGet();
    if (ch != expectedChar) throw InvalidJson("expect '" + std::string(1, expectedChar) + "', but '" + std::string(1, ch) + "'");
    return ch;
  }

  int expectAny(const std::string& expectedChars, bool skipPreWhiteSpace = false) {
    if (skipPreWhiteSpace) ws();
    int ch = tryGet();
    if (expectedChars.find(ch) == std::string::npos) throw InvalidJson("expect one of '" + expectedChars + "', but '" + std::string(1, ch) + "'");
    return ch;
  }

  int parseHex() {
    int ch = tryGet();
    if ('0' <= ch && ch <= '9') return ch - '0';
    if ('a' <= ch && ch <= 'f') return ch - 'a' + 10;
    if ('A' <= ch && ch <= 'F') return ch - 'A' + 10;
    throw InvalidJson("expect HEX, but '" + std::string(1,ch) + "'");
  }

  uint16_t parse4Hexs() {
    int ch1 = parseHex();
    int ch2 = parseHex();
    int ch3 = parseHex();
    int ch4 = parseHex();
    return ch1 << 12 | ch2 << 8 | ch3 << 4 | ch4;
  }

  // なぜstd::stou が標準にないのか……。
  unsigned int stou(const std::string& s) {
    unsigned long ul = std::stoul(s);
    if (std::numeric_limits<unsigned int>::max() < ul) {
      throw std::out_of_range("stou");
    }
    return ul;
  }

  std::istream& in_;
};

//
// === stream interface ===
//

template <typename T>
inline void stringify(std::ostream& out, const T& val) {
  Stringifier stringifier(out);
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
inline std::string to_json(const T& v) {
  std::ostringstream oss;
  stringify(oss, v);
  return oss.str();
}

template <typename T>
inline T from_json(const std::string& str) {
  std::istringstream iss(str);
  Parser parser(iss);
  T result;
  parser.parse(result);
  // check rest part
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
//   void stringifyJson(yu::json::Stringifier& stringifier) {
//     yu::json::createMemberStringifier(stringifier, *this)
//       << JSON_GETTER(str)
//       << JSON_NAMED_GETTER("number", num);
//   }
//   void parseJson(yu::json::Parser& parser) {
//     yu::json::createMemberParser(parser, *this)
//       << JSON_SETTER(str)
//       << JSON_NAMED_SETTER("number", num);
//   }
// }

#define JSON_GETTER(variable) \
  yu::json::Stringifier::MemberStringifier<std::remove_reference<decltype(*this)>::type>::MemberGetter(#variable, [](yu::json::Stringifier& stringifier, decltype(*this)& obj){ stringifier.stringify(obj.variable); })
#define JSON_NAMED_GETTER(name, variable) \
  yu::json::Stringifier::MemberStringifier<std::remove_reference<decltype(*this)>::type>::MemberGetter(name, [](yu::json::Stringifier& stringifier, decltype(*this)& obj){ stringifier.stringify(obj.variable); })

#define JSON_SETTER(variable) \
  yu::json::Parser::MemberParser<std::remove_reference<decltype(*this)>::type>::MemberSetter(#variable, [](yu::json::Parser& parser, decltype(*this)& obj){ parser.parse(obj.variable); })
#define JSON_NAMED_SETTER(name, variable) \
  yu::json::Parser::MemberParser<std::remove_reference<decltype(*this)>::type>::MemberSetter(name, [](yu::json::Parser& parser, decltype(*this)& obj){ parser.parse(obj.variable); })

template <class T>
inline Stringifier::MemberStringifier<T> createMemberStringifier(Stringifier& out, T& obj) {
  return Stringifier::MemberStringifier<T>(out, obj);
}

template <class T>
inline Parser::MemberParser<T> createMemberParser(Parser& in, T& obj) {
  return Parser::MemberParser<T>(in, obj);
}

}  // namespace json
}  // namespace yu

#endif  // YU_JSON_HPP_
