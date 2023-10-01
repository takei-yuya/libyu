#include "yu/stream/repeatstream.hpp"

#include "yu/test.hpp"

#include <string>

class RepeatStreamTest : public yu::Test {
};

TEST(RepeatStreamTest, testZeros) {
  yu::stream::repeatstream repeat(std::string(1, '\x00'));
  for (size_t i = 0; i < 1024 * 1024; ++i) {
    int ch = repeat.get();
    EXPECT(0x00, ==, ch);
  }
}

TEST(RepeatStreamTest, testOnes) {
  yu::stream::repeatstream repeat(std::string(1, '\xff'));
  for (size_t i = 0; i < 1024 * 1024; ++i) {
    int ch = repeat.get();
    EXPECT(0xff, ==, ch);
  }
}

TEST(RepeatStreamTest, testPattern) {
  yu::stream::repeatstream repeat("abc");
  for (size_t i = 0; i < 1024 * 1024; ++i) {
    int ch = repeat.get();
    EXPECT(0x61, ==, ch);
    ch = repeat.get();
    EXPECT(0x62, ==, ch);
    ch = repeat.get();
    EXPECT(0x63, ==, ch);
  }
}
