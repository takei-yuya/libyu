#include "yu/utf8.hpp"

#include <iostream>
#include <sstream>

int main() {
  uint32_t code_point = 0x1f363;
  std::string str = yu::utf8::encode(code_point);
  std::cout << str << std::endl;

  std::istringstream iss(u8"ABCあいう" + str + "\xff");
  yu::utf8::Decoder decoder(iss);
  while (decoder.has_next()) {
    std::cout << "U+" << std::hex << std::uppercase << decoder.next() << std::endl;
  }
  std::cout << std::dec
    << decoder.num_processed_bytes() << " bytes, "
    << decoder.num_processed_chars() << " chars, "
    << decoder.num_processed_errors() << " errors" << std::endl;
}
