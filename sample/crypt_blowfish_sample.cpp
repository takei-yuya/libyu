#include "yu/crypt/blowfish.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int main() {
  {  // ECB
    std::ostringstream oss;
    yu::crypt::blowfish_ecb_dec_ostream ds(oss, "key");
    yu::crypt::blowfish_ecb_enc_ostream es(ds, "key");
    es << "Hello blowfish!!";
    es.finish();
    ds.finish();
    std::cout << oss.str() << std::endl;
  }
  { // CBC, CFB and OFB
    std::ostringstream oss;
    yu::crypt::blowfish_cbc_dec_ostream ds(oss, "key", "Init Vec");
    yu::crypt::blowfish_cbc_enc_ostream es(ds, "key", "Init Vec");
    es << "Hello blowfish!!";
    es.finish();
    ds.finish();
    std::cout << oss.str() << std::endl;
  }

  {  // istream and ostream
    std::ostringstream oss;
    {
      yu::crypt::blowfish_cfb_enc_ostream oes(oss, "key", "Init Vec");
      oes << "Hello blowfish!!";
      oes.finish();
    }

    std::istringstream iss(oss.str());
    {
      yu::crypt::blowfish_cfb_dec_istream ids(iss, "key", "Init Vec");
      std::vector<char> buffer(1024);
      ids.read(buffer.data(), buffer.size());
      std::string message(buffer.data(), ids.gcount());
      std::cout << message << std::endl;
    }
  }
}
