#include "yu/json.hpp"

#include "yu/test.hpp"

#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>

#include "yu/lang/lexical_cast.hpp"

class JSONStringifyTest : public yu::Test {
};

TEST(JSONStringifyTest, testValueSimple) {
  EXPECT(R"(42)", ==, yu::json::to_json(42));
  EXPECT(R"(42)", ==, yu::json::to_json(42l));
  EXPECT(R"(42)", ==, yu::json::to_json(42u));
  EXPECT(R"(42)", ==, yu::json::to_json(42lu));
  EXPECT(R"(3.14)", ==, yu::json::to_json(3.14));
  EXPECT(R"(true)", ==, yu::json::to_json(true));
  EXPECT(R"(false)", ==, yu::json::to_json(false));
}

TEST(JSONStringifyTest, testInvalidValue) {
  EXPECT_THROW_WHAT(yu::json::to_json(std::numeric_limits<float>::quiet_NaN()), yu::json::InvalidJson, "NaN not allowed in JSON");
  EXPECT_THROW_WHAT(yu::json::to_json(std::numeric_limits<float>::signaling_NaN()), yu::json::InvalidJson, "NaN not allowed in JSON");
  EXPECT_THROW_WHAT(yu::json::to_json(std::numeric_limits<float>::infinity()), yu::json::InvalidJson, "Infinity not allowed in JSON");
  EXPECT_THROW_WHAT(yu::json::to_json(-std::numeric_limits<float>::infinity()), yu::json::InvalidJson, "Infinity not allowed in JSON");
}

TEST(JSONStringifyTest, testStringSimple) {
  std::string str = "abc";
  std::string actual = yu::json::to_json(str);
  EXPECT(R"("abc")", ==, actual);
}

TEST(JSONStringifyTest, testStringEscape) {
  std::string str = "r\r n\n t\t b\b f\f u\x01\x1f";
  std::string actual = yu::json::to_json(str);
  EXPECT(R"("r\r n\n t\t b\b f\f u\u0001\u001f")", ==, actual);
}

TEST(JSONStringifyTest, testStringMutiByte) {
  std::string str = "aЯあ";
  std::string actual = yu::json::to_json(str);
  EXPECT(R"("aЯあ")", ==, actual);
}

TEST(JSONStringifyTest, testStringMultiByteAsciiOnly) {
  std::string str = "aЯあ";
  std::string actual = yu::json::to_json(str, false, true);
  EXPECT(R"("a\u042f\u3042")", ==, actual);
}

TEST(JSONStringifyTest, testUnicodeNewLine) {
  std::string str = "LS: \u2028 PS: \u2029";
  std::string actual = yu::json::to_json(str);
  EXPECT(R"("LS: \u2028 PS: \u2029")", ==, actual);
  std::string actual_ascii = yu::json::to_json(str, false, true);
  EXPECT(R"("LS: \u2028 PS: \u2029")", ==, actual_ascii);
}

TEST(JSONStringifyTest, testVectorSimple) {
  std::vector<int> vec = { 1, 2, 3 };
  std::string actual = yu::json::to_json(vec);
  EXPECT(R"([1,2,3])", ==, actual);
}

TEST(JSONStringifyTest, testMapSimple) {
  std::unordered_map<std::string, bool> map = { { "foo", true }, { "bar", false } };
  std::string actual = yu::json::to_json(map);
  ASSERT(3u, <=, actual.size());
  if (actual[2] == 'f') {
    EXPECT(R"({"foo":true,"bar":false})", ==, actual);
  } else {
    EXPECT(R"({"bar":false,"foo":true})", ==, actual);
  }
}

TEST(JSONStringifyTest, testPretty) {
  std::map<std::string, std::vector<std::map<std::string, int>>> value = {
    {
      "x",
      {
        {
          { "a", 1 },
          { "b", 2 }
        },
        {
          { "c", 3 }
        },
        {
        }
      }
    },
    { "y", {} },
  };
  std::string expected = R"({
  "x":[
    {
      "a":1,
      "b":2
    },
    {
      "c":3
    },
    {
      
    }
  ],
  "y":[
    
  ]
})";
  std::ostringstream oss;
  {
    yu::json::Stringifier stringifier(oss, true);
    stringifier.stringify(value);
  }
  EXPECT(expected, ==, oss.str());
  yu::json::from_json<decltype(value)>(oss.str());  // no throw
}

class JSONParseTest : public yu::Test {
};

TEST(JSONParseTest, testParseNumber) {
  EXPECT_THROW_WHAT(yu::json::from_json<double>("x"), yu::json::InvalidJson, "expect one of '-0123456789', but 'x'");
  EXPECT_F(0.0, ==, yu::json::from_json<double>("0"));
  EXPECT_THROW_WHAT(yu::json::from_json<double>("0X"), yu::json::InvalidJson, "unexpected rest part: rest = X");
  EXPECT_F(-0.0, ==, yu::json::from_json<double>("-0"));
  EXPECT_THROW_WHAT(yu::json::from_json<double>("-0X"), yu::json::InvalidJson, "unexpected rest part: rest = X");
  EXPECT_THROW_WHAT(yu::json::from_json<double>("--"), yu::json::InvalidJson, "expect one of '0123456789', but '-'");
  EXPECT_THROW_WHAT(yu::json::from_json<double>("+0"), yu::json::InvalidJson, "expect one of '-0123456789', but '+'");  //  先頭 + は許容しない
  EXPECT_THROW_WHAT(yu::json::from_json<double>("01"), std::runtime_error, "unexpected rest part: rest = 1");  // 先頭 0 は許容しないので、0まで読んで1が残る
  EXPECT_F(1.0, ==, yu::json::from_json<double>("1"));
  EXPECT_THROW_WHAT(yu::json::from_json<double>("1X"), yu::json::InvalidJson, "unexpected rest part: rest = X");
  EXPECT_F(12.0, ==, yu::json::from_json<double>("12"));
  EXPECT_THROW_WHAT(yu::json::from_json<double>("12X"), yu::json::InvalidJson, "unexpected rest part: rest = X");
  // 小数部
  EXPECT_THROW_WHAT(yu::json::from_json<double>("0."), yu::json::InvalidJson, "unexpected EOF");
  EXPECT_THROW_WHAT(yu::json::from_json<double>("0.,"), yu::json::InvalidJson, "expect one of '0123456789', but ','");
  EXPECT_F(-12.3, ==, yu::json::from_json<double>("-12.3"));
  EXPECT_THROW_WHAT(yu::json::from_json<double>("-12.3X"), yu::json::InvalidJson, "unexpected rest part: rest = X");
  // 指数部
  EXPECT_THROW_WHAT(yu::json::from_json<double>("0e"), yu::json::InvalidJson, "unexpected EOF");
  EXPECT_THROW_WHAT(yu::json::from_json<double>("0e,"), yu::json::InvalidJson, "expect one of '+-0123456789', but ','");
  EXPECT_F(-0e0, ==, yu::json::from_json<double>("-0e0"));
  EXPECT_THROW_WHAT(yu::json::from_json<double>("-0e0X"), yu::json::InvalidJson, "unexpected rest part: rest = X");
  EXPECT_F(1e0, ==, yu::json::from_json<double>("1e0"));
  EXPECT_THROW_WHAT(yu::json::from_json<double>("1e0X"), yu::json::InvalidJson, "unexpected rest part: rest = X");
  EXPECT_THROW_WHAT(yu::json::from_json<double>("0e+"), yu::json::InvalidJson, "unexpected EOF");
  EXPECT_THROW_WHAT(yu::json::from_json<double>("0e+,"), yu::json::InvalidJson, "expect one of '0123456789', but ','");
  EXPECT_THROW_WHAT(yu::json::from_json<double>("0e-"), yu::json::InvalidJson, "unexpected EOF");
  EXPECT_THROW_WHAT(yu::json::from_json<double>("0e-,"), yu::json::InvalidJson, "expect one of '0123456789', but ','");
  EXPECT_F(1e+0, ==, yu::json::from_json<double>("1e+0"));
  EXPECT_THROW_WHAT(yu::json::from_json<double>("1e-0X"), yu::json::InvalidJson, "unexpected rest part: rest = X");
  EXPECT_F(1E+0, ==, yu::json::from_json<double>("1E+0"));
  EXPECT_THROW_WHAT(yu::json::from_json<double>("1E-0X"), yu::json::InvalidJson, "unexpected rest part: rest = X");
  // 小数部 + 指数部
  EXPECT_THROW_WHAT(yu::json::from_json<double>("0.e"), yu::json::InvalidJson, "expect one of '0123456789', but 'e'");
  EXPECT_THROW_WHAT(yu::json::from_json<double>("0.1e"), yu::json::InvalidJson, "unexpected EOF");
  EXPECT_THROW_WHAT(yu::json::from_json<double>("0.1e,"), yu::json::InvalidJson, "expect one of '+-0123456789', but ','");
  EXPECT_F(12.34e56, ==, yu::json::from_json<double>("12.34e56"));
  EXPECT_THROW_WHAT(yu::json::from_json<double>("12.34e56X"), yu::json::InvalidJson, "unexpected rest part: rest = X");
  EXPECT_F(12.3e+4, ==, yu::json::from_json<double>("12.3e+4"));
  EXPECT_THROW_WHAT(yu::json::from_json<double>("12.3e+4X"), yu::json::InvalidJson, "unexpected rest part: rest = X");
  EXPECT_F(12.3e-4, ==, yu::json::from_json<double>("12.3e-4"));
  EXPECT_THROW_WHAT(yu::json::from_json<double>("12.3e-4X"), yu::json::InvalidJson, "unexpected rest part: rest = X");
}

TEST(JSONParseTest, testValueSimple) {
  EXPECT(true, ==, yu::json::from_json<bool>(R"(true)"));
  EXPECT(false, ==, yu::json::from_json<bool>(R"(false)"));
  EXPECT_THROW_WHAT(yu::json::from_json<bool>(R"(truE)"), yu::json::InvalidJson, "expect 'e', but 'E'");
  EXPECT_THROW_WHAT(yu::json::from_json<bool>(R"(False)"), yu::json::InvalidJson, "expect one of 'tf', but 'F'");
  EXPECT_F(6.022e23, ==, yu::json::from_json<double>(R"(6.022e+23)"));
  // // TODO: エラーにすべき？
  EXPECT(299792458lu, ==, yu::json::from_json<uint64_t>(R"(2.99792458e8)"));

  std::string num_max_str = yu::lang::lexical_cast<std::string>(std::numeric_limits<uint64_t>::max());
  EXPECT(std::numeric_limits<uint64_t>::max(), ==, yu::json::from_json<uint64_t>(num_max_str));

  std::string num_min_str = yu::lang::lexical_cast<std::string>(std::numeric_limits<uint64_t>::min());
  EXPECT(std::numeric_limits<uint64_t>::min(), ==, yu::json::from_json<uint64_t>(num_min_str));
}

TEST(JSONParseTest, testInvalidValue) {
  EXPECT_THROW_WHAT(yu::json::from_json<double>(R"(NaN)"), yu::json::InvalidJson, "expect one of '-0123456789', but 'N'");
  EXPECT_THROW_WHAT(yu::json::from_json<double>(R"(Inf)"), yu::json::InvalidJson, "expect one of '-0123456789', but 'I'");
  EXPECT_THROW_WHAT(yu::json::from_json<double>(R"(-Inf)"), yu::json::InvalidJson, "expect one of '0123456789', but 'I'");
  // string interface not allow rest part
  EXPECT_THROW_WHAT(yu::json::from_json<int>(R"(123,456)"), yu::json::InvalidJson, "unexpected rest part: rest = ,456");
  // invalid spaces
  EXPECT_THROW_WHAT(yu::json::from_json<int>(R"(- 0)"), yu::json::InvalidJson, "expect one of '0123456789', but ' '");
  EXPECT_THROW_WHAT(yu::json::from_json<int>(R"(-0 .0)"), yu::json::InvalidJson, "unexpected rest part: rest = .0");
  EXPECT_THROW_WHAT(yu::json::from_json<int>(R"(-0. 0)"), yu::json::InvalidJson, "expect one of '0123456789', but ' '");
  EXPECT_THROW_WHAT(yu::json::from_json<int>(R"(-0.0 e0)"), yu::json::InvalidJson, "unexpected rest part: rest = e0");
  EXPECT_THROW_WHAT(yu::json::from_json<int>(R"(-0.0e 0)"), yu::json::InvalidJson, "expect one of '+-0123456789', but ' '");
  EXPECT_THROW_WHAT(yu::json::from_json<bool>(R"(t r u e)"), yu::json::InvalidJson, "expect 'r', but ' '");
  EXPECT_THROW_WHAT(yu::json::from_json<bool>(R"(f a l s e)"), yu::json::InvalidJson, "expect 'a', but ' '");
}

TEST(JSONParseTest, testInteger) {
#define MIN_MAX_TEST(type, minmax) \
  { \
    std::ostringstream oss; \
    oss << std::numeric_limits<type>::minmax(); \
    EXPECT(std::numeric_limits<type>::minmax(), ==, yu::json::from_json<type>(oss.str())); \
  }
  MIN_MAX_TEST(int, min);
  MIN_MAX_TEST(int, max);
  MIN_MAX_TEST(long, min);
  MIN_MAX_TEST(long, max);
  MIN_MAX_TEST(long long, min);;
  MIN_MAX_TEST(long long, max);
  MIN_MAX_TEST(unsigned int, min);
  MIN_MAX_TEST(unsigned int, max);
  MIN_MAX_TEST(unsigned long, min);
  MIN_MAX_TEST(unsigned long, max);
  MIN_MAX_TEST(unsigned long long, min);
  MIN_MAX_TEST(unsigned long long, max);
#undef MIN_MAX_TEST

  // 一桁足して確実にoverflowする値にする。数値列版と浮動小数点表記版
#define OVERFLOW_TEST(type) \
  { \
    std::ostringstream oss; \
    oss << std::numeric_limits<type>::max() << "0"; \
    EXPECT_THROW_WHAT(yu::json::from_json<type>(oss.str()), yu::json::InvalidJson, "number out of range"); \
  } \
  { \
    std::ostringstream oss; \
    oss << std::numeric_limits<type>::max() << ".0e1"; \
    EXPECT_THROW_WHAT(yu::json::from_json<type>(oss.str()), yu::json::InvalidJson, "number out of range"); \
  }
  OVERFLOW_TEST(int);
  OVERFLOW_TEST(long);
  OVERFLOW_TEST(long long);
  OVERFLOW_TEST(unsigned);
  OVERFLOW_TEST(unsigned long);
  OVERFLOW_TEST(unsigned long long);
#undef OVERFLOW_TEST

  EXPECT_THROW_WHAT(yu::json::from_json<unsigned>(" -1 "), yu::json::InvalidJson, "number out of range");
  EXPECT_THROW_WHAT(yu::json::from_json<unsigned long>(" -1 "), yu::json::InvalidJson, "number out of range");
  EXPECT_THROW_WHAT(yu::json::from_json<unsigned long long>(" -1 "), yu::json::InvalidJson, "number out of range");
  EXPECT_THROW_WHAT(yu::json::from_json<unsigned>(" -1e0 "), yu::json::InvalidJson, "number out of range");
  EXPECT_THROW_WHAT(yu::json::from_json<unsigned long>(" -1e0 "), yu::json::InvalidJson, "number out of range");
  EXPECT_THROW_WHAT(yu::json::from_json<unsigned long long>(" -1e0 "), yu::json::InvalidJson, "number out of range");
}

TEST(JSONParseTest, testFloat) {
  {
    std::ostringstream oss;
    oss << "1.5e" << (std::numeric_limits<float>::max_exponent10 - 1);
    yu::json::from_json<float>(oss.str());  // no throw
  }
  {
    std::ostringstream oss;
    oss << "1.5e" << (std::numeric_limits<float>::max_exponent10 + 1);
    EXPECT_THROW_WHAT(yu::json::from_json<float>(oss.str()), yu::json::InvalidJson, "number out of range");
  }
  {
    std::ostringstream oss;
    oss << "1.5e" << (std::numeric_limits<double>::max_exponent10 + 1);
    EXPECT_THROW_WHAT(yu::json::from_json<double>(oss.str()), yu::json::InvalidJson, "number out of range");
  }
  {
    std::ostringstream oss;
    oss << "1.5e" << (std::numeric_limits<long double>::max_exponent10 + 1);
    EXPECT_THROW_WHAT(yu::json::from_json<long double>(oss.str()), yu::json::InvalidJson, "number out of range");
  }
}

TEST(JSONParseTest, testStringSimple) {
  EXPECT("abc", ==, yu::json::from_json<std::string>(R"(   "abc"   )"));
}

TEST(JSONParseTest, testStringEscape) {
  EXPECT("r\r n\n t\t b\b f\f u\xC2\xA0\xD0\xB4\xf0\x9f\x8D\xa3", ==,
         yu::json::from_json<std::string>(R"("r\r n\n t\t b\b f\f u\u00a0\u0434\ud83c\udf63")"));
}

TEST(JSONParseTest, testStringEmpty) {
  EXPECT("", ==, yu::json::from_json<std::string>(R"("")"));
}

TEST(JSONParseTest, testInvalidEscape) {
  EXPECT_THROW_WHAT(yu::json::from_json<std::string>(R"("\e")"), yu::json::InvalidJson, "unexpected escape e");
}

TEST(JSONParseTest, testStringInvalidUnicode) {
  EXPECT("\xf0\x9f\x8D\xa3", ==, yu::json::from_json<std::string>(R"("\uD83C\uDF63")"));

  EXPECT_THROW_WHAT(yu::json::from_json<std::string>(R"("\uDF63")"), yu::json::InvalidJson, "invalid surrogate: low surrogate without high surrogate");
  EXPECT_THROW_WHAT(yu::json::from_json<std::string>(R"("\uD83C)"), yu::json::InvalidJson, "unexpected EOF");
  EXPECT_THROW_WHAT(yu::json::from_json<std::string>(R"("\uD83C")"), yu::json::InvalidJson, "expect '\\', but '\"'");
  EXPECT_THROW_WHAT(yu::json::from_json<std::string>(R"("\uD83C\)"), yu::json::InvalidJson, "unexpected EOF");
  EXPECT_THROW_WHAT(yu::json::from_json<std::string>(R"("\uD83C\")"), yu::json::InvalidJson, "expect 'u', but '\"'");
  EXPECT_THROW_WHAT(yu::json::from_json<std::string>(R"("\uD83C\u)"), yu::json::InvalidJson, "unexpected EOF");
  EXPECT_THROW_WHAT(yu::json::from_json<std::string>(R"("\uD83C\u")"), yu::json::InvalidJson, "expect HEX, but '\"'");
  EXPECT_THROW_WHAT(yu::json::from_json<std::string>(R"("\uD83C\u0000")"), yu::json::InvalidJson, "invalid surrogate: missing low surrogate");
  EXPECT_THROW_WHAT(yu::json::from_json<std::string>("\"\\uD83C\xcd\xbd\xa3\""), yu::json::InvalidJson, "expect '\\', but '\xcd'");

  EXPECT("\x6d\xa0\xbc\xcd\xbd\xa3", ==, yu::json::from_json<std::string>("\"\x6d\xa0\xbc\xcd\xbd\xa3\""));
}

TEST(JSONParseTest, testVectorSimple) {
  std::vector<std::string> expected = { "foo", "bar", "baz" };
  EXPECT(expected, ==, yu::json::from_json<std::vector<std::string>>(R"( [ "foo", "bar", "baz" ] )"));
}

TEST(JSONParseTest, testVectorEmpty) {
  std::vector<std::string> expected = {};
  EXPECT(expected, ==, yu::json::from_json<std::vector<std::string>>(R"( [ ] )"));
}

TEST(JSONParseTest, testVectorNested) {
  std::vector<std::vector<bool>> expected = { { true, false }, { } };
  EXPECT(expected, ==, yu::json::from_json<std::vector<std::vector<bool>>>(R"([[true,false],[]])"));
}

TEST(JSONParseTest, testInvalidVector) {
  EXPECT_THROW_WHAT(yu::json::from_json<std::vector<double>>(R"([,])"), yu::json::InvalidJson, "expect one of '-0123456789', but ','");
  EXPECT_THROW_WHAT(yu::json::from_json<std::vector<double>>(R"([)"), yu::json::InvalidJson, "unexpected EOF");
  EXPECT_THROW_WHAT(yu::json::from_json<std::vector<double>>(R"([[)"), yu::json::InvalidJson, "expect one of '-0123456789', but '['");
}

TEST(JSONParseTest, testMapSimple) {
  std::unordered_map<std::string, double> expected = { { "pi", 3.14159 }, { "e", 2.71828 } };
  std::unordered_map<std::string, double> actual = yu::json::from_json<std::unordered_map<std::string, double>>(R"( { "pi" : 0.314159e1 , "e" : 271.828e-2 } )");
  EXPECT(expected, ==, actual);
}

TEST(JSONParseTest, testMapEmpty) {
  std::unordered_map<std::string, double> expected;
  std::unordered_map<std::string, double> actual = yu::json::from_json<std::unordered_map<std::string, double>>(R"( { } )");
  EXPECT(expected, ==, actual);
}

TEST(JSONParseTest, testInvalidMap) {
  using map_type = std::unordered_map<std::string, double>;
  EXPECT_THROW_WHAT(yu::json::from_json<map_type>(R"({,})"), yu::json::InvalidJson, "expect '\"', but ','");
  EXPECT_THROW_WHAT(yu::json::from_json<map_type>(R"({)"), yu::json::InvalidJson, "unexpected EOF");
  EXPECT_THROW_WHAT(yu::json::from_json<map_type>(R"({}})"), yu::json::InvalidJson, "unexpected rest part: rest = }");
  EXPECT_THROW_WHAT(yu::json::from_json<map_type>(R"({{)"), yu::json::InvalidJson, "expect '\"', but '{'");
  EXPECT_THROW_WHAT(yu::json::from_json<map_type>(R"({"key"})"), yu::json::InvalidJson, "expect ':', but '}'");
  EXPECT_THROW_WHAT(yu::json::from_json<map_type>(R"({"key":})"), yu::json::InvalidJson, "expect one of '-0123456789', but '}'");
  EXPECT_THROW_WHAT(yu::json::from_json<map_type>(R"({"key":1,})"), yu::json::InvalidJson, "expect '\"', but '}'");
}

TEST(JSONParseTest, testFloarVectorToIntVector) {
  // TODO: エラーにすべき？
  std::vector<int> expected = { 3, 2 };
  EXPECT(expected, ==, yu::json::from_json<std::vector<int>>(R"( [ 0.314159e1 , 271.828e-2 ] )"));
}

class Klass {
 public:
  Klass() : s("str"), i(42), d(3.14), vt({ true, false }), mu({ { "a", 1 } }) {}
  Klass(const std::string& s, int i, double d, const std::vector<bool>& vt, const std::unordered_map<std::string, unsigned int>& mu)
    : s(s), i(i), d(d), vt(vt), mu(mu) {}

  bool operator==(const Klass& rhs) const {
    if (s != rhs.s) return false;
    if (i != rhs.i) return false;
    if (yu::test::float_compare(d, rhs.d) != 0) return false;
    if (vt != rhs.vt) return false;
    if (mu != rhs.mu) return false;

    return true;
  }

  std::string s;
  int i;
  double d;
  std::vector<bool> vt;
  std::unordered_map<std::string, unsigned int> mu;

  void stringifyJson(yu::json::Stringifier& stringifier) const {
    JSON_MEMBER_STRINGIFIER(stringifier)
      << JSON_NAMED_GETTER("str", s)
      << JSON_GETTER(i)
      << JSON_GETTER(d)
      << JSON_GETTER(vt)
      << JSON_GETTER(mu)
      << JSON_STRINGIFY;
  }

  void parseJson(yu::json::Parser& parser) {
    JSON_MEMBER_PARSER(parser)
      << JSON_NAMED_SETTER("str", s)
      << JSON_SETTER(i)
      << JSON_SETTER(d)
      << JSON_SETTER(vt)
      << JSON_SETTER(mu)
      << JSON_PARSE;
  }
};

class SuperKlass {
 public:
  SuperKlass() : obj(), vec(2), map({ { "x", Klass("x", 123, -0.5, { true }, { { "x", 1 } }) } }) {}
  bool operator==(const SuperKlass& rhs) const {
    return obj == rhs.obj && vec == rhs.vec && map == rhs.map;
  }
 private:
  Klass obj;
  std::vector<Klass> vec;
  std::unordered_map<std::string, Klass> map;
 public:
  void stringifyJson(yu::json::Stringifier& stringifier) const {
    JSON_MEMBER_STRINGIFIER(stringifier) << JSON_GETTER(obj) << JSON_GETTER(vec) << JSON_GETTER(map) << JSON_STRINGIFY;
  }
  void parseJson(yu::json::Parser& parser) {
    JSON_MEMBER_PARSER(parser) << JSON_SETTER(obj) << JSON_SETTER(vec) << JSON_SETTER(map) << JSON_PARSE;
  }
};

class JSONObjectMappingTest : public yu::Test {
};

TEST(JSONObjectMappingTest, testStringify) {
  Klass obj;
  obj.s = "str\x01";
  std::ostringstream oss;
  yu::json::stringify(oss, obj);
  // Stringifier keep order
  EXPECT(R"({"str":"str\u0001","i":42,"d":3.14,"vt":[true,false],"mu":{"a":1}})", ==, oss.str());
}

TEST(JSONObjectMappingTest, testParse) {
  Klass obj;
  std::istringstream iss(R"( { "mu": { "x": 9, "y": 8, "z": 7 }, "vt": [false, true], "d": 2.71828, "i": -1, "str": "Hello" } )");
  yu::json::parse(iss, obj);
  Klass expected("Hello", -1, 2.71828, { false, true }, { { "x", 9 }, { "y", 8 }, { "z", 7 } });
  EXPECT(expected, ==, obj);
}

TEST(JSONObjectMappingTest, testParseEmpty) {
  Klass obj;
  std::istringstream iss(R"( {} )");
  yu::json::parse(iss, obj);
  Klass expected;  // keep all original values
  EXPECT(expected, ==, obj);
}

TEST(JSONObjectMappingTest, testParseKeyMissmatch) {
  Klass obj;
  // ignore input xxx, keep original mu value
  std::istringstream iss(R"( { "xxx": { "x": 9, "y": 8, "z": 7 }, "vt": [false, true], "d": 2.71828, "i": -1, "str": "Hello" } )");
  yu::json::parse(iss, obj);
  Klass expected("Hello", -1, 2.71828, { false, true }, { { "a", 1 } });
  EXPECT(expected, ==, obj);
}

TEST(JSONObjectMappingTest, testParseError) {
  EXPECT_THROW_WHAT(yu::json::from_json<Klass>(R"( { "mu": [ 1, 2, 3 ], "vt": [false, true], "d": 2.71828, "i": -1, "s": "Hello" } )"), yu::json::InvalidJson, "expect '{', but '['");
}

TEST(JSONObjectMappingTest, testSuperStringifyParse) {
  SuperKlass s;
  std::string json = yu::json::to_json(s);
  SuperKlass actual = yu::json::from_json<SuperKlass>(json);
  EXPECT(s, ==, actual);
}
