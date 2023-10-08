#include "yu/crypt/blowfish.hpp"

#include <sstream>
#include <iostream>

int main() {
  std::ostringstream oss;
  yu::crypt::blowfish_dec_ostream ds(oss, "key");
  yu::crypt::blowfish_enc_ostream es(ds, "key");
  es << "Hello blowfish!!";
  es.finish();
  ds.finish();

  std::cout << oss.str() << std::endl;
}
