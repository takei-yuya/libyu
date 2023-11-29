#include "yu/digest/bcrypt.hpp"

#include <iostream>

int main() {
  std::cout << yu::digest::bcrypt_sha256("salt", "password") << std::endl;
  std::cout << yu::digest::bcrypt_sha512("salt", "password") << std::endl;
  std::string digest = yu::digest::bcrypt_sha512("", "password");  // random salt
  std::cout << digest << std::endl;
  std::cout << yu::digest::bcrypt_check(digest, "password") << std::endl;
  std::cout << yu::digest::bcrypt_check(digest, "Password") << std::endl;
}
