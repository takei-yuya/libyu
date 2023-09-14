#include <iostream>

#include "yu/stream/fdstream.hpp"

int main() {
  yu::stream::fdstream fds(1);
  fds << "Hello World." << std::endl;
}
