#include "yu/utf8.hpp"

#include <iostream>
#include <sstream>

int main() {
  uint32_t code_point = 0x1f363;
  std::string str = yu::utf8::encode(code_point);
  std::cout << str << std::endl;

  // invalid char '\xff' will be replaced by U+FFFD
  std::istringstream iss("ABCあいう\xff" + str);
  yu::utf8::Decoder decoder(iss);
  while (decoder.has_next()) {
    code_point = decoder.next();
    std::cout << yu::utf8::encode(code_point) << " U+" << std::hex << std::uppercase << code_point << std::endl;
  }
  std::cout << std::dec
    << decoder.num_processed_bytes() << " bytes, "
    << decoder.num_processed_chars() << " chars, "
    << decoder.num_processed_errors() << " errors" << std::endl;
}
