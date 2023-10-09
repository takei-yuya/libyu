#include "yu/crypt/blowfish.hpp"

#include <sstream>
#include <iostream>

int main() {
  {
    std::ostringstream oss;
    yu::crypt::blowfish_ecb_enc_ostream ds(oss, "key");
    yu::crypt::blowfish_ecb_dec_ostream es(ds, "key");
    es << "Hello blowfish!!";
    es.finish();
    ds.finish();
    std::cout << oss.str() << std::endl;
  }
  {
    std::ostringstream oss;
    yu::crypt::blowfish_cbc_enc_ostream ds(oss, "key", "Initial Vector 1");
    yu::crypt::blowfish_cbc_dec_ostream es(ds, "key", "Initial Vector 1");
    es << "Hello blowfish!!";
    es.finish();
    ds.finish();
    std::cout << oss.str() << std::endl;
  }
}
