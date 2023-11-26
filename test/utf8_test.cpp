#include "yu/utf8.hpp"

#include "yu/test.hpp"

#include <iostream>
#include <sstream>

class UTF8Test : public yu::Test {
};

TEST(UTF8Test, testEncodeASCII) {
  for (uint32_t ch = 0x00; ch < 0x80; ++ch) {
    EXPECT(std::string(1, static_cast<char>(ch)), ==, yu::utf8::encode(ch));
  }
}

TEST(UTF8Test, testEncodeSimple) {
  EXPECT("\xc2\x80", ==, yu::utf8::encode(0x80));  // <Padding Character> (PAD)
  EXPECT("\xd0\x94", ==, yu::utf8::encode(0x414));  // Д Cyrillic Capital Letter De
  EXPECT("\xdf\xbf", ==, yu::utf8::encode(0x7FF));  // ߿ Nko Taman Sign
  EXPECT("\xe0\xa0\x80", ==, yu::utf8::encode(0x800));  // ࠀ Samaritan Letter Alaf
  EXPECT("\xe3\x81\x82", ==, yu::utf8::encode(0x3042));  // あ Hiragana Letter A
  EXPECT("\xef\xbf\xbf", ==, yu::utf8::encode(0xFFFF));  // Undefined Character (U+FFFF)
  EXPECT("\xf0\x90\x80\x80", ==, yu::utf8::encode(0x10000));  // 𐀀 Linear B Syllable B008 A
  EXPECT("\xf0\x9f\x8d\xa3", ==, yu::utf8::encode(0x1f363));  // 🍣 Sushi
}

TEST(UTF8Test, testEncodeError) {
  EXPECT_THROW_WHAT(yu::utf8::encode(0x110000), std::runtime_error, "Invalid code point: code_point = U+110000");
  EXPECT_THROW_WHAT(yu::utf8::encode(0xD800), std::runtime_error, "UTF-8 not allow to encode surruogate code point: code_point = U+D800");
  EXPECT_THROW_WHAT(yu::utf8::encode(0xDFFF), std::runtime_error, "UTF-8 not allow to encode surruogate code point: code_point = U+DFFF");
  EXPECT_THROW_WHAT(yu::utf8::encode(0xDC00), std::runtime_error, "UTF-8 not allow to encode surruogate code point: code_point = U+DC00");
  EXPECT_THROW_WHAT(yu::utf8::encode(0xDFFF), std::runtime_error, "UTF-8 not allow to encode surruogate code point: code_point = U+DFFF");
}

TEST(UTF8Test, testDecoder) {
  std::vector<uint32_t> expected = {
    0x01, 0x41, 0x7f,
    0x80, 0x414, 0x7ff,
    0x800, 0x3042, 0xffff,
    0x10000, 0x1f363, 0x10ffff,
  };
  std::string str =
    "\x01" "\x41" "\x7f"
    "\xc2\x80" "\xd0\x94" "\xdf\xbf"
    "\xe0\xa0\x80" "\xe3\x81\x82" "\xef\xbf\xbf"
    "\xf0\x90\x80\x80" "\xf0\x9f\x8d\xa3" "\xf4\x8f\xbf\xbf";
  std::istringstream iss(str);
  yu::utf8::Decoder decoder(iss);
  size_t i = 0;
  while (decoder.has_next()) {
    uint32_t cp = decoder.next();
    EXPECT(expected[i], ==, cp);
    EXPECT("", ==, decoder.last_error());
    ++i;
  }
  EXPECT(expected.size(), ==, i);
}

TEST(UTF8Test, testDecoderErrors) {
  std::vector<std::pair<std::string, std::string>> error_strings = {
    std::make_pair("\xf8",              "invalid byte"), // 0b11111000
    std::make_pair("\xfc",              "invalid byte"), // 0b11111100
    std::make_pair("\xfe",              "invalid byte"), // 0b11111110
    std::make_pair("\xff",              "invalid byte"), // 0b11111111
    std::make_pair("\x80",              "an unexpected continuation byte"),
    std::make_pair("\xbf",              "an unexpected continuation byte"),
    std::make_pair("\xc2",              "the string ending before the end of the character"),  // 2bytes char
    std::make_pair("\xdf",              "the string ending before the end of the character"),  // 2bytes char
    std::make_pair("\xe0",              "the string ending before the end of the character"),  // 3bytes char
    std::make_pair("\xe0\xa0",          "the string ending before the end of the character"),  // 3bytes char
    std::make_pair("\xef",              "the string ending before the end of the character"),  // 3bytes char
    std::make_pair("\xef\xbf",          "the string ending before the end of the character"),  // 3bytes char
    std::make_pair("\xf0",              "the string ending before the end of the character"),  // 4bytes char
    std::make_pair("\xf0\x90",          "the string ending before the end of the character"),  // 4bytes char
    std::make_pair("\xf0\x90\x80",      "the string ending before the end of the character"),  // 4bytes char
    std::make_pair("\xc2\x01",          "a non-continuation byte before the end of the character"),
    std::make_pair("\xe0\xa0\xf0",      "a non-continuation byte before the end of the character"),
    std::make_pair("\xf0\x90\x80\xff",  "a non-continuation byte before the end of the character"),
    std::make_pair("\xc0\x80",          "an overlong encoding"),
    std::make_pair("\xe0\x80\x80",      "an overlong encoding"),
    std::make_pair("\xf0\x80\x80\x80",  "an overlong encoding"),
    std::make_pair("\xed\xa0\x80",      "a sequence that decodes to an invalid code point"),  // D800
    std::make_pair("\xed\xaf\xbf",      "a sequence that decodes to an invalid code point"),  // DBFF
    std::make_pair("\xed\xb0\x80",      "a sequence that decodes to an invalid code point"),  // DC00
    std::make_pair("\xed\xbf\xbf",      "a sequence that decodes to an invalid code point"),  // DFFF
  };
  for (const auto& it : error_strings) {
    std::istringstream iss(it.first);
    yu::utf8::Decoder decoder(iss);
    EXPECT(true, ==, decoder.has_next());
    EXPECT(0xfffd, ==, decoder.next());
    EXPECT(it.second, ==, decoder.last_error());
    EXPECT(false, ==, decoder.has_next());
  }
}
