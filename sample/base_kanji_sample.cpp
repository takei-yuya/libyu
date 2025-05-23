#include "yu/base_kanji.hpp"

#include <iostream>

int main() {
  std::cout << yu::base_kanji::encode<11>("Hello World.") << std::endl;;
  std::cout << yu::base_kanji::encode<12>("Hello World.") << std::endl;;
  std::cout << yu::base_kanji::encode<13>("Hello World.") << std::endl;;

  std::cout << yu::base_kanji::decode<11>(yu::base_kanji::encode<11>("Hello World.")) << std::endl;;
  std::cout << yu::base_kanji::decode<12>(yu::base_kanji::encode<12>("Hello World.")) << std::endl;;
  std::cout << yu::base_kanji::decode<13>(yu::base_kanji::encode<13>("Hello World.")) << std::endl;;
}

