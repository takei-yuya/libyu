#include "yu/stream/fdstream.hpp"

#include "yu/test.hpp"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

class FDStreamTest : public yu::Test {
 public:
  FDStreamTest() : yu::Test(), tmpdir_() {}

 protected:
  void prepare() override {
    const char tmp_template[] = "fdstream_test_XXXXXX";
    std::vector<char> tmpdir_name(tmp_template, tmp_template + sizeof(tmp_template));
    tmpdir_ = mkdtemp(tmpdir_name.data());
  }

  void teardown() override {
    // TODO: system
    system(("rm -rf " + tmpdir_).c_str());
  }

  std::string tmpdir_;
};

TEST(FDStreamTest, testSimple) {
  std::string tmp_file(tmpdir_ + "/test_file");
  {
    int fd = ::open(tmp_file.c_str(), O_WRONLY | O_CREAT, S_IRWXU);
    ASSERT(fd, >, 0);
    yu::stream::fdstream out(fd);
    out << "Hello World." << std::endl;
    ::close(fd);
  }
  {
    int fd = ::open(tmp_file.c_str(), O_RDONLY);
    ASSERT(fd, >, 0);
    yu::stream::fdstream in(fd);
    std::string s;
    in >> s;
    EXPECT("Hello", ==, s);
    in >> s;
    EXPECT("World.", ==, s);
    ::close(fd);
  }
}

TEST(FDStreamTest, invalidFileDescriptorGet) {
  yu::stream::fdstream fds(999);
  EXPECT(-1, ==, fds.get());
  EXPECT(false, ==, fds.good());
}

TEST(FDStreamTest, invalidFileDescriptorPut) {
  yu::stream::fdstream fds(999);
  fds.put('x');
  fds.flush();
  EXPECT(false, ==, fds.good());
}
