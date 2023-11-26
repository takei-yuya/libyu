#include "yu/utf8.hpp"

#include <iostream>
#include <sstream>

int main() {
  uint32_t code_point = 0x1f363;
  std::string str = yu::utf8::encode(code_point);
  std::cout << str << std::endl;

  std::istringstream iss(str);
  yu::utf8::Decoder decoder(iss);
  std::cout << "U+" << std::hex << std::uppercase << decoder.next() << std::endl;
}
