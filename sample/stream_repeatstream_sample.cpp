#include "yu/stream/repeatstream.hpp"

#include <iostream>
#include <iomanip>

int main() {
  yu::stream::repeatstream repeat("yes\n");
  std::string line;
  for (size_t i = 0; i < 10; ++i) {
    std::getline(repeat, line);
    std::cout << line << std::endl;
  }
}

