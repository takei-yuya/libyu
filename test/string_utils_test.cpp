#include "yu/string_utils.hpp"

#include "yu/test.hpp"

#include <sstream>
#include <iostream>

class StringTest : public yu::Test {
};


TEST(StringTest, testLstrip) {
  EXPECT("foo bar \n", ==, yu::string_utils::lstrip("   foo bar \n"));
}

TEST(StringTest, testLstripEmpty) {
  EXPECT("", ==, yu::string_utils::lstrip(""));
  EXPECT("", ==, yu::string_utils::lstrip("     "));
}

TEST(StringTest, testRstrip) {
  EXPECT("   foo bar", ==, yu::string_utils::rstrip("   foo bar \n"));
}

TEST(StringTest, testRstripEmpty) {
  EXPECT("", ==, yu::string_utils::rstrip(""));
  EXPECT("", ==, yu::string_utils::rstrip("     "));
}

TEST(StringTest, testStrip) {
  EXPECT("foo bar", ==, yu::string_utils::strip("   foo bar \n"));
}

TEST(StringTest, testStripEmpty) {
  EXPECT("", ==, yu::string_utils::strip(""));
  EXPECT("", ==, yu::string_utils::strip("     "));
}

TEST(StringTest, testSplit) {
  std::vector<std::string> expected = { "foo", " bar", " baz" };
  std::vector<std::string> actual = yu::string_utils::split("foo, bar, baz", ',');
  EXPECT(expected, ==, actual);
}

TEST(StringTest, testSplitStrip) {
  std::vector<std::string> expected = { "foo", "bar", "baz" };
  std::vector<std::string> actual = yu::string_utils::split("foo, bar, baz", ',', true);
  EXPECT(expected, ==, actual);
}

TEST(StringTest, testSplitStripMax) {
  std::vector<std::string> expected = { "foo", "bar, baz" };
  std::vector<std::string> actual = yu::string_utils::split("foo, bar, baz", ',', true, 2);
  EXPECT(expected, ==, actual);
}

TEST(StringTest, testStartsWith) {
  EXPECT(true, ==, yu::string_utils::starts_with("foobar", "foo"));
  EXPECT(true, ==, yu::string_utils::starts_with("foobar", "foobar"));
  EXPECT(false, ==, yu::string_utils::starts_with("foobar", "Foo"));
  EXPECT(false, ==, yu::string_utils::starts_with("foobar", "foobarbaz"));
}

TEST(StringTest, testEndsWith) {
  EXPECT(true, ==, yu::string_utils::ends_with("foobar", "bar"));
  EXPECT(true, ==, yu::string_utils::ends_with("foobar", "foobar"));
  EXPECT(false, ==, yu::string_utils::ends_with("foobar", "Bar"));
  EXPECT(false, ==, yu::string_utils::ends_with("foobar", "__foobar"));
}
