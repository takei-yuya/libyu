#include "yu/stream/fdstream.hpp"

#include <iostream>

int main() {
  yu::stream::fdstream fds(1);
  fds << "Hello World." << std::endl;
}
