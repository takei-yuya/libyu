#include "yu/lang/lexical_cast.hpp"

#include "yu/test.hpp"

class LexicalCastTest : public yu::Test {
};

TEST(LexicalCastTest, testSimple) {
  EXPECT("123", ==, yu::lang::lexical_cast<std::string>(123));
  EXPECT(123, ==, yu::lang::lexical_cast<int>("123"));
  EXPECT(12, ==, yu::lang::lexical_cast<int>(12.3));
  int n = yu::lang::lexical_cast<int, int>(12);
  EXPECT(12, ==, n);
  std::string s = yu::lang::lexical_cast<std::string, std::string>("test");
  EXPECT("test", ==, s);
}
