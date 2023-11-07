#include "yu/base64.hpp"

#include "yu/test.hpp"

#include <iostream>
#include <sstream>

class Base64Test : public yu::Test {
};

TEST(Base64Test, testEncodeSimple) {
  EXPECT("", ==, yu::base64::encode(""));
  EXPECT("QQ==", ==, yu::base64::encode("A"));
  EXPECT("QUI=", ==, yu::base64::encode("AB"));
  EXPECT("QUJD", ==, yu::base64::encode("ABC"));
  EXPECT("QUJDRA==", ==, yu::base64::encode("ABCD"));
  EXPECT("QUJDREVGR0hJSktMTU5PUFFSU1RVVldYWVphYmNkZWZnaGlqa2xtbm9wcXJzdHV2d3h5ejAxMjM0\nNTY3ODk=", ==,
         yu::base64::encode("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"));
  EXPECT("QUJDREVGR0hJSktMTU5PUFFSU1RVVldYWVphYmNkZWZnaGlqa2xtbm9wcXJzdHV2d3h5ejAxMjM0NTY3ODk=", ==,
         yu::base64::encode("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", 0));
  EXPECT("+/A=", ==, yu::base64::encode("\xfb\xf0"));
}

TEST(Base64Test, testDecodeSimple) {
  EXPECT("", ==, yu::base64::decode(""));
  EXPECT("A", ==, yu::base64::decode("QQ=="));
  EXPECT("AB", ==, yu::base64::decode("QUI="));
  EXPECT("ABC", ==, yu::base64::decode("QUJD"));
  EXPECT("ABCD", ==, yu::base64::decode("QUJDRA=="));
  EXPECT("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", ==,
         yu::base64::decode("QUJDREVGR0hJSktMTU5PUFFSU1RVVldYWVphYmNkZWZnaGlqa2xtbm9wcXJzdHV2d3h5ejAxMjM0\r\nNTY3ODk="));
  EXPECT("\xfb\xf0", ==, yu::base64::decode("+/A="));
}

TEST(Base64Test, testDecodeError) {
  EXPECT_THROW_WHAT(yu::base64::decode("_"), yu::base64::InvalidBase64, "unexpected char: ch = '_'");
  EXPECT_THROW_WHAT(yu::base64::decode("QQ"), yu::base64::InvalidBase64, "odd padding");
  EXPECT_THROW_WHAT(yu::base64::decode("QQ=A"), yu::base64::InvalidBase64, "unexpected char after padding: ch = 'A'");
}

TEST(Base64Test, testEncodeURLSimple) {
  EXPECT("-_A", ==, yu::base64::encodeURL("\xfb\xf0"));
}

TEST(Base64Test, testDecodeURLSimple) {
  EXPECT("\xfb\xf0", ==, yu::base64::decodeURL("-_A"));
}

TEST(Base64Test, testRadix64) {
  // sample in RFC4880
  const char raw_data[] =
    "\xc8\x38\x01\x3b\x6d\x96\xc4\x11\xef\xec\xef\x17\xec\xef\xe3\xca"
    "\x00\x04\xce\x89\x79\xea\x25\x0a\x89\x79\x95\xf9\x79\xa9\x0a\xd9"
    "\xa9\xa9\x05\x0a\x89\x0a\xc5\xa9\xc9\x45\xa9\x40\xc1\xa2\xfc\xd2"
    "\xbc\x14\x85\x8c\xd4\xa2\x54\x7b\x2e\x00";
  std::string data = std::string(raw_data, sizeof(raw_data) - 1);
  std::string base64 =
    "yDgBO22WxBHv7O8X7O/jygAEzol56iUKiXmV+XmpCtmpqQUKiQrFqclFqUDBovzS\n"
    "vBSFjNSiVHsuAA==";

  {
    yu::base64::Encoder encoder(64);
    std::istringstream iss(data);
    std::ostringstream oss;
    uint32_t crc24 = encoder.encode(iss, oss);
    EXPECT(base64, ==, oss.str());
    EXPECT(0x9e350dUL, ==, crc24);  // =njUN
  }

  {
    yu::base64::Decoder decoder;
    std::istringstream iss(base64);
    std::ostringstream oss;
    uint32_t crc24 = decoder.decode(iss, oss);
    EXPECT(data, ==, oss.str());
    EXPECT(0x9e350dUL, ==, crc24);  // =njUN
  }
}
