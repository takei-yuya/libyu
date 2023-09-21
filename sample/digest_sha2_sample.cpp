#include "yu/digest/sha2.hpp"

#include <iostream>
#include <string>

int main() {
  std::string message = "Hello World.";
  std::cout << yu::digest::sha256_hex(message) << std::endl;
  std::cout << yu::digest::sha224_hex(message) << std::endl;
  std::cout << yu::digest::sha512_hex(message) << std::endl;
  std::cout << yu::digest::sha384_hex(message) << std::endl;
  std::cout << yu::digest::sha512_224_hex(message) << std::endl;
  std::cout << yu::digest::sha512_256_hex(message) << std::endl;
}
