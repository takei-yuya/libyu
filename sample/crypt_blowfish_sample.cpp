#include "yu/crypt/blowfish.hpp"

#include <sstream>
#include <iostream>

int main() {
  {
    std::ostringstream oss;
    yu::crypt::blowfish_ecb_dec_ostream ds(oss, "key");
    yu::crypt::blowfish_ecb_enc_ostream es(ds, "key");
    es << "Hello blowfish!!";
    es.finish();
    ds.finish();
    std::cout << oss.str() << std::endl;
  }
  {
    std::ostringstream oss;
    yu::crypt::blowfish_cbc_dec_ostream ds(oss, "key", "Init Vec");
    yu::crypt::blowfish_cbc_enc_ostream es(ds, "key", "Init Vec");
    es << "Hello blowfish!!";
    es.finish();
    ds.finish();
    std::cout << oss.str() << std::endl;
  }
}
