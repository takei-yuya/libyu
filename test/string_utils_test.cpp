#include "yu/string/utils.hpp"

#include "yu/test.hpp"

#include <sstream>
#include <iostream>

class StringTest : public yu::Test {
};


TEST(StringTest, testLstrip) {
  EXPECT("foo bar \n", ==, yu::string::lstrip("   foo bar \n"));
}

TEST(StringTest, testLstripEmpty) {
  EXPECT("", ==, yu::string::lstrip(""));
  EXPECT("", ==, yu::string::lstrip("     "));
}

TEST(StringTest, testRstrip) {
  EXPECT("   foo bar", ==, yu::string::rstrip("   foo bar \n"));
}

TEST(StringTest, testRstripEmpty) {
  EXPECT("", ==, yu::string::rstrip(""));
  EXPECT("", ==, yu::string::rstrip("     "));
}

TEST(StringTest, testStrip) {
  EXPECT("foo bar", ==, yu::string::strip("   foo bar \n"));
}

TEST(StringTest, testStripEmpty) {
  EXPECT("", ==, yu::string::strip(""));
  EXPECT("", ==, yu::string::strip("     "));
}

TEST(StringTest, testSplit) {
  std::vector<std::string> expected = { "foo", " bar", " baz" };
  std::vector<std::string> actual = yu::string::split("foo, bar, baz", ',');
  EXPECT(expected, ==, actual);
}

TEST(StringTest, testSplitStrip) {
  std::vector<std::string> expected = { "foo", "bar", "baz" };
  std::vector<std::string> actual = yu::string::split("foo, bar, baz", ',', true);
  EXPECT(expected, ==, actual);
}

TEST(StringTest, testSplitStripMax) {
  std::vector<std::string> expected = { "foo", "bar, baz" };
  std::vector<std::string> actual = yu::string::split("foo, bar, baz", ',', true, 2);
  EXPECT(expected, ==, actual);
}

TEST(StringTest, testStartsWith) {
  EXPECT(true, ==, yu::string::starts_with("foobar", "foo"));
  EXPECT(true, ==, yu::string::starts_with("foobar", "foobar"));
  EXPECT(false, ==, yu::string::starts_with("foobar", "Foo"));
  EXPECT(false, ==, yu::string::starts_with("foobar", "foobarbaz"));
}

TEST(StringTest, testEndsWith) {
  EXPECT(true, ==, yu::string::ends_with("foobar", "bar"));
  EXPECT(true, ==, yu::string::ends_with("foobar", "foobar"));
  EXPECT(false, ==, yu::string::ends_with("foobar", "Bar"));
  EXPECT(false, ==, yu::string::ends_with("foobar", "__foobar"));
}
