#include "yu/base_kanji.hpp"

#include "yu/test.hpp"

class BaseKanjiTest : public yu::Test {
 public:
  BaseKanjiTest() : kanji_cps() {}

  void prepare() override {
    std::istringstream iss(yu::base_kanji::kKanjis);
    yu::utf8::Decoder decoder(iss);
    while (decoder.has_next()) {
      kanji_cps.push_back(decoder.next());
    }
  }

 protected:
  std::vector<uint32_t> kanji_cps;
};

TEST(BaseKanjiTest, testEncode_level1) {
  //        H        e        l        l        o        _        W        o        r        l        d        .
  //    4   8    6   5    6   c    6   c    6   f    2   0    5   7    6   f    7   2    6   c    6   4    2   e
  // 01001000 01100101 01101100 01101100 01101111 00100000 01010111 01101111 01110010 01101100 01100100 00101110
  // 01001000011 00101011011 00011011000 11011110010 00000101011 10110111101 11001001101 10001100100 00101110000
  //         579         347         216        1778          43        1469        1613        1124         368(zero padding)
  std::vector<uint16_t> bits = { 579, 347, 216, 1778, 43, 1469, 1613, 1124, 368 };
  std::string expected;
  for (uint16_t i : bits) {
    expected += yu::utf8::encode(kanji_cps[i]);
  }

  std::string encoded = yu::base_kanji::encode<11>("Hello World.");
  EXPECT("彊鰍化託位須前寺蒲", ==, encoded);
  EXPECT(expected, ==, encoded);
  EXPECT("Hello World.", ==, yu::base_kanji::decode<11>(encoded));
}

TEST(BaseKanjiTest, testEncode_level2) {
  //        H        e        l        l        o        _        W        o        r        l        d        .
  //    4   8    6   5    6   c    6   c    6   f    2   0    5   7    6   f    7   2    6   c    6   4    2   e
  // 01001000 01100101 01101100 01101100 01101111 00100000 01010111 01101111 01110010 01101100 01100100 00101110
  // 010010000110 010101101100 011011000110 111100100000 010101110110 111101110010 011011000110 010000101110
  //         1158         1388         1734         3872         1398         3954         1734         1070
  std::vector<uint16_t> bits = { 1158, 1388, 1734, 3872, 1398, 3954, 1734, 1070 };
  std::string expected;
  for (uint16_t i : bits) {
    expected += yu::utf8::encode(kanji_cps[i]);
  }

  std::string encoded = yu::base_kanji::encode<12>("Hello World.");
  EXPECT("漆冗陀掵杖斷陀仕", ==, encoded);
  EXPECT(expected, ==, encoded);
  EXPECT("Hello World.", ==, yu::base_kanji::decode<12>(encoded));
}

TEST(BaseKanjiTest, testEncode_level4) {
  //        H        e        l        l        o        _        W        o        r        l        d        .
  //    4   8    6   5    6   c    6   c    6   f    2   0    5   7    6   f    7   2    6   c    6   4    2   e
  // 01001000 01100101 01101100 01101100 01101111 00100000 01010111 01101111 01110010 01101100 01100100 00101110
  // 0100100001100 1010110110001 1011000110111 1001000000101 0111011011110 1110010011011 0001100100001 01110________
  //          2316          5553          5687          4613          3806          7323           801    14(pad)
  std::vector<uint16_t> bits = { 2316, 5553, 5687, 4613, 3806, 7323, 801, 14+(1<<13) };
  std::string expected;
  for (uint16_t i : bits) {
    expected += yu::utf8::encode(kanji_cps[i]);
  }

  std::string encoded = yu::base_kanji::encode<13>("Hello World.");
  EXPECT("否誡踵疵戉褹糊揔" , ==, encoded);
  EXPECT(expected, ==, encoded);
  EXPECT("Hello World.", ==, yu::base_kanji::decode<13>(encoded));
}
