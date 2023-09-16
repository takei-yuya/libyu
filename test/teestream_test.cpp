#include "yu/stream/teestream.hpp"

#include "yu/test.hpp"

class TeeStreamTest : public yu::Test {
};

TEST(TeeStreamTest, testOTeeStream) {
  std::ostringstream out1;
  std::ostringstream out2;
  yu::stream::oteestream ots(out1, out2);

  std::istringstream iss("Hello World!!");
  ots << iss.rdbuf();
  EXPECT("Hello World!!", ==, out1.str());
  EXPECT("Hello World!!", ==, out2.str());
}

TEST(TeeStreamTest, testITeeStream) {
  std::istringstream iss("Hello World!!");
  std::ostringstream oss;
  yu::stream::iteestream its(iss, oss);

  std::string str;
  its >> str;
  EXPECT("Hello", ==, str);
  EXPECT("Hello World!!", ==, oss.str());  // ostream recieve whole buffer
  its >> str;
  EXPECT("World!!", ==, str);
}

TEST(TeeStreamTest, testITeeStreamLarge) {
  std::vector<char> buf(1024 * 1024);
  for (size_t i = 0; i < buf.size(); ++i) {
    buf[i] = static_cast<char>(i % 0b11111111);
  }
  std::string input(buf.data(), buf.size());
  std::istringstream iss(input);
  std::ostringstream oss;
  yu::stream::iteestream its(iss, oss);

  std::vector<char> read_buf(buf.size());
  its.read(read_buf.data(), read_buf.size());
  std::string actual(read_buf.data(), its.gcount());

  EXPECT(input, ==, actual);
  EXPECT(input, ==, oss.str());
}
