#include "yu/stream/nullstream.hpp"

#include "yu/test.hpp"

#include <vector>

class NullStreamTest : public yu::Test {
};

TEST(NullStreamTest, testRead) {
  std::vector<char> buffer(1024);
  yu::stream::nullstream ns;
  ns.read(buffer.data(), buffer.size());
  EXPECT(0, ==, ns.gcount());
  EXPECT(true, ==, ns.eof());
}

TEST(NullStreamTest, testWrite) {
  std::vector<char> buffer(1024);
  yu::stream::nullstream ns;
  for (size_t i = 0; i < 1024; ++i) {
    ns.write(buffer.data(), buffer.size());
  }
}
