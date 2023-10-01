#include <iostream>
#include <fstream>
#include <string>

#include "yu/digest/sha2.hpp"
#include "yu/stream/teestream.hpp"

void print_digest(const std::string& filename, std::istream& in) {
  yu::digest::sha224_stream sha224s;
  yu::digest::sha256_stream sha256s;
  yu::digest::sha384_stream sha384s;
  yu::digest::sha512_stream sha512s;
  yu::digest::sha512_224_stream sha512_224s;
  yu::digest::sha512_256_stream sha512_256s;

  yu::stream::oteestream tos(sha224s, sha256s, sha384s, sha512s, sha512_224s, sha512_256s);
  tos << in.rdbuf() << std::flush;

  std::cout << "SHA224(" << filename << ")= " << sha224s.hash_hex() << std::endl;
  std::cout << "SHA256(" << filename << ")= " << sha256s.hash_hex() << std::endl;
  std::cout << "SHA384(" << filename << ")= " << sha384s.hash_hex() << std::endl;
  std::cout << "SHA512(" << filename << ")= " << sha512s.hash_hex() << std::endl;
  std::cout << "SHA512-224(" << filename << ")= " << sha512_224s.hash_hex() << std::endl;
  std::cout << "SHA512-256(" << filename << ")= " << sha512_256s.hash_hex() << std::endl;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    print_digest("stdin", std::cin);
  } else {
    for (int i = 1; i < argc; ++i) {
      std::ifstream ifs(argv[i]);
      print_digest(argv[i], ifs);
    }
  }
}
