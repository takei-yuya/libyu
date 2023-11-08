#include "yu/base64.hpp"

#include <iostream>

int main() {
  std::cout << yu::base64::encode("Hello World.") << std::endl;;
  std::cout << yu::base64::decode("SGVsbG8gV29ybGQu") << std::endl;
}
