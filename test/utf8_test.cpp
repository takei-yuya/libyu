#include "yu/utf8.hpp"

#include "yu/test.hpp"

#include <sstream>
#include <iostream>

class UTF8Test : public yu::Test {
};

TEST(UTF8Test, testEncodeASCII) {
  for (int ch = '\x00'; ch < '\x80'; ++ch) {
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
