#include "yu/crypt/blowfish.hpp"

#include "yu/test.hpp"

class BlowFishTest : public yu::Test {
 protected:
  char hexchar2bin(char ch) {
    if ('0' <= ch && ch <= '9') return static_cast<char>(ch - '0');
    if ('a' <= ch && ch <= 'f') return static_cast<char>(ch - 'a' + 0xa);
    if ('A' <= ch && ch <= 'F') return static_cast<char>(ch - 'A' + 0xA);
    FAIL("Invalid char for hex");
  }

  std::string hex2bin(const std::string& hex_str) {
    std::string result;
    for (size_t i = 0; i + 1 < hex_str.size(); i += 2) {
      result += static_cast<char>(hexchar2bin(hex_str[i]) << 4 | hexchar2bin(hex_str[i+1]));
    }
    return result;
  }
};

TEST(BlowFishTest, test_enc_dec) {
  std::ostringstream oss;
  yu::crypt::blowfish_dec_ostream ds(oss, "key");
  yu::crypt::blowfish_enc_ostream es(ds, "key");
  es << "Hello blowfish!!";
  es.finish();
  ds.finish();

  EXPECT("Hello blowfish!!", ==, oss.str());
}

TEST(BlowFishTest, testTestCases) {
  // echo '0000000000000000' | xxd -ps -r | openssl bf-ecb -provider legacy -K 00000000000000000000000000000000 -nopad | xxd -ps
  std::vector<std::pair<std::string, std::pair<std::string, std::string>>> test_cases = {
    { hex2bin("00000000000000000000000000000000"), { hex2bin("0000000000000000"), hex2bin("4ef997456198dd78") } },
    { hex2bin("00000000000000000000000000000000"), { hex2bin("ffffffffffffffff"), hex2bin("014933e0cdaff6e4") } },
    { hex2bin("00000000000000000000000000000000"), { hex2bin("0123456789abcdef"), hex2bin("19f40a0d847f51c3") } },
    { hex2bin("00000000000000000000000000000000"), { hex2bin("fedcba9876543210"), hex2bin("09a5aa8371843981") } },
    { hex2bin("ffffffffffffffffffffffffffffffff"), { hex2bin("0000000000000000"), hex2bin("f21e9a77b71c49bc") } },
    { hex2bin("ffffffffffffffffffffffffffffffff"), { hex2bin("ffffffffffffffff"), hex2bin("51866fd5b85ecb8a") } },
    { hex2bin("ffffffffffffffffffffffffffffffff"), { hex2bin("0123456789abcdef"), hex2bin("0b13debc7b8635cd") } },
    { hex2bin("ffffffffffffffffffffffffffffffff"), { hex2bin("fedcba9876543210"), hex2bin("0d7b00c01a21e5c1") } },
    { hex2bin("00112233445566778899aabbccddeeff"), { hex2bin("0000000000000000"), hex2bin("36d4e2502b003630") } },
    { hex2bin("00112233445566778899aabbccddeeff"), { hex2bin("ffffffffffffffff"), hex2bin("77c465ae7a9a2077") } },
    { hex2bin("00112233445566778899aabbccddeeff"), { hex2bin("0123456789abcdef"), hex2bin("dcec940a9cf3faa7") } },
    { hex2bin("00112233445566778899aabbccddeeff"), { hex2bin("fedcba9876543210"), hex2bin("7d5c6b95237c00ca") } },
    { hex2bin("ffeeddccbbaa99887766554433221100"), { hex2bin("0000000000000000"), hex2bin("2b04a56528a8498d") } },
    { hex2bin("ffeeddccbbaa99887766554433221100"), { hex2bin("ffffffffffffffff"), hex2bin("d3f5d17de0e1b07b") } },
    { hex2bin("ffeeddccbbaa99887766554433221100"), { hex2bin("0123456789abcdef"), hex2bin("bd16bc4918c27135") } },
    { hex2bin("ffeeddccbbaa99887766554433221100"), { hex2bin("fedcba9876543210"), hex2bin("df7e18415a64fd86") } },
    { hex2bin("0123456789abcdef0123456789abcdef"), { hex2bin("0000000000000000"), hex2bin("245946885754369a") } },
    { hex2bin("0123456789abcdef0123456789abcdef"), { hex2bin("ffffffffffffffff"), hex2bin("1c6cd49835c7b012") } },
    { hex2bin("0123456789abcdef0123456789abcdef"), { hex2bin("0123456789abcdef"), hex2bin("008a8314ee2b27a3") } },
    { hex2bin("0123456789abcdef0123456789abcdef"), { hex2bin("fedcba9876543210"), hex2bin("9f5fac53492e0761") } },
    { hex2bin("fedcba9876543210fedcba9876543210"), { hex2bin("0000000000000000"), hex2bin("2324986da676719c") } },
    { hex2bin("fedcba9876543210fedcba9876543210"), { hex2bin("ffffffffffffffff"), hex2bin("6b5c5a9c5d9e0a5a") } },
    { hex2bin("fedcba9876543210fedcba9876543210"), { hex2bin("0123456789abcdef"), hex2bin("0aceab0fc6a0a28d") } },
    { hex2bin("fedcba9876543210fedcba9876543210"), { hex2bin("fedcba9876543210"), hex2bin("40c7ccd7789805f5") } },
  };
  for (size_t i = 0; i < test_cases.size(); ++i) {
    const std::string& key = test_cases[i].first;
    const std::string& plain = test_cases[i].second.first;
    const std::string& enc = test_cases[i].second.second;

    {
      std::ostringstream oss;
      yu::crypt::blowfish_enc_ostream es(oss, key);
      es.write(plain.data(), static_cast<std::streamsize>(plain.size()));
      es.finish();
      EXPECT(enc, ==, oss.str());
    }
    {
      std::ostringstream oss;
      yu::crypt::blowfish_dec_ostream es(oss, key);
      es.write(enc.data(), static_cast<std::streamsize>(enc.size()));
      es.finish();
      EXPECT(plain, ==, oss.str());
    }
  }
}
