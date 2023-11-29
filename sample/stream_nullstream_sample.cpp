#include "yu/stream/nullstream.hpp"

#include <iostream>
#include <vector>

int main() {
  yu::stream::nullstream ns;
  std::vector<char> buffer(1024);

  // empty read
  ns.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
  std::cout << "read = " << ns.gcount() << " bytes, eof = " << ns.eof() << std::endl;

  // infinity write
  size_t write_count = 0;
  for (size_t i = 0; i < 1024 * 1024; ++i) {
    ns.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    write_count += buffer.size();
  }
  std::cout << "write = " << write_count << " bytes" << std::endl;
}
