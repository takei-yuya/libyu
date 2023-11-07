#include <iostream>
#include <random>

#include <getopt.h>
#include <unistd.h>

#include "yu/digest/bcrypt.hpp"

struct Options {
  Options() : mode(kDefault), salt(""), read_from_stdin(false) {}

  enum {
    kDefault,
    kSha256,
    kSha512,
  } mode;
  std::string salt;
  bool read_from_stdin;
};

void usage(std::ostream& out, int, char** argv) {
  out
    << "Usage: " << argv[0] << " [options] [password]..." << std::endl
    << std::endl
    << "  -5, --sha256          sha256" << std::endl
    << "  -6, --sha512          sha512" << std::endl
    << "  -s, --salt=val        salt" << std::endl
    << "      --stdin           read password from stdin instead of arguments" << std::endl
    << "      --help            show this message" << std::endl
    ;
}

std::string passwd(const Options& options, const std::string& password) {
  std::string salt = options.salt;

  if (options.mode == Options::kDefault || options.mode == Options::kSha512) {
    return yu::digest::bcrypt_sha512(salt, password);
  } else if (options.mode == Options::kSha256) {
    return yu::digest::bcrypt_sha256(salt, password);
  } else {
    return "ERROR";
  }
}

int main(int argc, char** argv) {
  Options options;

  while (1) {
    static struct option long_options[] = {
      { "sha256",   no_argument, 0, '5' },
      { "sha512",   no_argument, 0, '6' },
      { "salt",     required_argument, 0, 's' },
      { "stdin",    no_argument, 0, 'S' },
      { "help",     no_argument, 0, '?' },
    };

    int option_index = 0;
    int c = getopt_long(argc, argv, "56s:", long_options, &option_index);
    if (c == -1) break;

    switch (c) {
     case '5':
       options.mode = Options::kSha256;
       break;

     case '6':
       options.mode = Options::kSha512;
       break;

     case 's':
       options.salt = optarg;
       break;

     case 'S':
       options.read_from_stdin = true;
       break;

     case '?':
       usage(std::cout, argc, argv);
       return 0;
    }
  }

  if (options.read_from_stdin) {
    std::string line;
    while (std::getline(std::cin, line)) {
      std::cout << passwd(options, line) << std::endl;
    }
  } else {
    if (optind == argc) {
      usage(std::cerr, argc, argv);
      return 1;
    }
    for(int i = optind; i < argc; ++i) {
      std::cout << passwd(options, argv[i]) << std::endl;
    }
  }
}
