#include <iostream>

#include "yu/lang/lexical_cast.hpp"

int main() {
  std::cout << yu::lang::lexical_cast<int>("42") << std::endl;
  std::cout << (yu::lang::lexical_cast<std::string>(42) + "0") << std::endl;
}