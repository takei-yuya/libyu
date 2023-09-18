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

TEST(StringTest, testICompareSimple) {
  EXPECT(0, ==, yu::string::icompare("abc", "abc"));
  EXPECT(0, ==, yu::string::icompare("Abc", "abC"));
  EXPECT(0, >, yu::string::icompare("Abc", "abCd"));
  EXPECT(0, <, yu::string::icompare("AbcD", "abC"));
}

TEST(StringTest, testICompareNullChar) {
  std::string s1("abc\0def", 7);
  std::string s2("ABC\0def", 7);
  std::string s3("ABC", 3);
  EXPECT(0, ==, yu::string::icompare(s1, s2));
  EXPECT(0, <, yu::string::icompare(s1, s3));
}

TEST(StringTest, testILess) {
  EXPECT(false, ==, yu::string::iless("abc", "abc"));
  EXPECT(false, ==, yu::string::iless("Abc", "abC"));
  EXPECT(true, ==, yu::string::iless("Abc", "abCd"));
  EXPECT(false, ==, yu::string::iless("AbcD", "abC"));
}

TEST(StringTest, testILessNullChar) {
  std::string s1("abc\0def", 7);
  std::string s2("ABC\0def", 7);
  std::string s3("ABC", 3);
  EXPECT(false, ==, yu::string::iless(s1, s2));
  EXPECT(false, ==, yu::string::iless(s1, s3));
  EXPECT(true, ==, yu::string::iless(s3, s1));
}
