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
