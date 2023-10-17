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

TEST(BlowFishTest, test_zero_padding) {
  std::vector<char> buf = { '0', '1', '2', '3', '4', '5', '6', '7' };

  EXPECT(0, ==, yu::crypt::ZeroPadding::padding(buf.data(), buf.data() + 8, buf.data() + 8));
  EXPECT(std::string("01234567", 8), ==, std::string(buf.data(), buf.size()));
  EXPECT(0, ==, yu::crypt::ZeroPadding::unpadding(buf.data(), buf.data() + 8, buf.data() + 8));

  EXPECT(1, ==, yu::crypt::ZeroPadding::padding(buf.data(), buf.data() + 7, buf.data() + 8));
  EXPECT(std::string("0123456\0", 8), ==, std::string(buf.data(), buf.size()));
  EXPECT(1, ==, yu::crypt::ZeroPadding::unpadding(buf.data(), buf.data() + 8, buf.data() + 8));

  EXPECT(2, ==, yu::crypt::ZeroPadding::padding(buf.data(), buf.data() + 6, buf.data() + 8));
  EXPECT(std::string("012345\0\0", 8), ==, std::string(buf.data(), buf.size()));
  EXPECT(2, ==, yu::crypt::ZeroPadding::unpadding(buf.data(), buf.data() + 8, buf.data() + 8));

  EXPECT(3, ==, yu::crypt::ZeroPadding::padding(buf.data(), buf.data() + 5, buf.data() + 8));
  EXPECT(std::string("01234\0\0\0", 8), ==, std::string(buf.data(), buf.size()));
  EXPECT(3, ==, yu::crypt::ZeroPadding::unpadding(buf.data(), buf.data() + 8, buf.data() + 8));

  EXPECT(4, ==, yu::crypt::ZeroPadding::padding(buf.data(), buf.data() + 4, buf.data() + 8));
  EXPECT(std::string("0123\0\0\0\0", 8), ==, std::string(buf.data(), buf.size()));
  EXPECT(4, ==, yu::crypt::ZeroPadding::unpadding(buf.data(), buf.data() + 8, buf.data() + 8));

  EXPECT(5, ==, yu::crypt::ZeroPadding::padding(buf.data(), buf.data() + 3, buf.data() + 8));
  EXPECT(std::string("012\0\0\0\0\0", 8), ==, std::string(buf.data(), buf.size()));
  EXPECT(5, ==, yu::crypt::ZeroPadding::unpadding(buf.data(), buf.data() + 8, buf.data() + 8));

  EXPECT(6, ==, yu::crypt::ZeroPadding::padding(buf.data(), buf.data() + 2, buf.data() + 8));
  EXPECT(std::string("01\0\0\0\0\0\0", 8), ==, std::string(buf.data(), buf.size()));
  EXPECT(6, ==, yu::crypt::ZeroPadding::unpadding(buf.data(), buf.data() + 8, buf.data() + 8));

  EXPECT(7, ==, yu::crypt::ZeroPadding::padding(buf.data(), buf.data() + 1, buf.data() + 8));
  EXPECT(std::string("0\0\0\0\0\0\0\0", 8), ==, std::string(buf.data(), buf.size()));
  EXPECT(7, ==, yu::crypt::ZeroPadding::unpadding(buf.data(), buf.data() + 8, buf.data() + 8));

  EXPECT(8, ==, yu::crypt::ZeroPadding::padding(buf.data(), buf.data() + 0, buf.data() + 8));
  EXPECT(std::string("\0\0\0\0\0\0\0\0", 8), ==, std::string(buf.data(), buf.size()));
  EXPECT(8, ==, yu::crypt::ZeroPadding::unpadding(buf.data(), buf.data() + 8, buf.data() + 8));
}

TEST(BlowFishTest, test_pksc7_padding) {
  std::vector<char> buf = { '0', '1', '2', '3', '4', '5', '6', '7' };

  EXPECT(1, ==, yu::crypt::PKCS7Padding::padding(buf.data(), buf.data() + 7, buf.data() + 8));
  EXPECT(std::string("0123456\x01", 8), ==, std::string(buf.data(), buf.size()));
  EXPECT(1, ==, yu::crypt::PKCS7Padding::unpadding(buf.data(), buf.data() + 8, buf.data() + 8));

  EXPECT(2, ==, yu::crypt::PKCS7Padding::padding(buf.data(), buf.data() + 6, buf.data() + 8));
  EXPECT(std::string("012345\x02\x02", 8), ==, std::string(buf.data(), buf.size()));
  EXPECT(2, ==, yu::crypt::PKCS7Padding::unpadding(buf.data(), buf.data() + 8, buf.data() + 8));

  EXPECT(3, ==, yu::crypt::PKCS7Padding::padding(buf.data(), buf.data() + 5, buf.data() + 8));
  EXPECT(std::string("01234\x03\x03\x03", 8), ==, std::string(buf.data(), buf.size()));
  EXPECT(3, ==, yu::crypt::PKCS7Padding::unpadding(buf.data(), buf.data() + 8, buf.data() + 8));

  EXPECT(4, ==, yu::crypt::PKCS7Padding::padding(buf.data(), buf.data() + 4, buf.data() + 8));
  EXPECT(std::string("0123\x04\x04\x04\x04", 8), ==, std::string(buf.data(), buf.size()));
  EXPECT(4, ==, yu::crypt::PKCS7Padding::unpadding(buf.data(), buf.data() + 8, buf.data() + 8));

  EXPECT(5, ==, yu::crypt::PKCS7Padding::padding(buf.data(), buf.data() + 3, buf.data() + 8));
  EXPECT(std::string("012\x05\x05\x05\x05\x05", 8), ==, std::string(buf.data(), buf.size()));
  EXPECT(5, ==, yu::crypt::PKCS7Padding::unpadding(buf.data(), buf.data() + 8, buf.data() + 8));

  EXPECT(6, ==, yu::crypt::PKCS7Padding::padding(buf.data(), buf.data() + 2, buf.data() + 8));
  EXPECT(std::string("01\x06\x06\x06\x06\x06\x06", 8), ==, std::string(buf.data(), buf.size()));
  EXPECT(6, ==, yu::crypt::PKCS7Padding::unpadding(buf.data(), buf.data() + 8, buf.data() + 8));

  EXPECT(7, ==, yu::crypt::PKCS7Padding::padding(buf.data(), buf.data() + 1, buf.data() + 8));
  EXPECT(std::string("0\x07\x07\x07\x07\x07\x07\x07", 8), ==, std::string(buf.data(), buf.size()));
  EXPECT(7, ==, yu::crypt::PKCS7Padding::unpadding(buf.data(), buf.data() + 8, buf.data() + 8));

  EXPECT(8, ==, yu::crypt::PKCS7Padding::padding(buf.data(), buf.data() + 0, buf.data() + 8));
  EXPECT(std::string("\x08\x08\x08\x08\x08\x08\x08\x08", 8), ==, std::string(buf.data(), buf.size()));
  EXPECT(8, ==, yu::crypt::PKCS7Padding::unpadding(buf.data(), buf.data() + 8, buf.data() + 8));
}


TEST(BlowFishTest, test_ecb_enc_dec) {
  std::ostringstream oss;
  yu::crypt::blowfish_ecb_dec_ostream ds(oss, "key");
  yu::crypt::blowfish_ecb_enc_ostream es(ds, "key");
  es << "Hello blowfish!!";
  es.finish();
  ds.finish();

  EXPECT("Hello blowfish!!", ==, oss.str());
}

TEST(BlowFishTest, testECBTestCases_nopadding) {
  // echo '0000000000000000' | xxd -ps -r | openssl bf-ecb -provider legacy -K 00000000000000000000000000000000 -nopad | xxd -ps
  std::vector<std::pair<std::string, std::pair<std::string, std::string>>> test_cases = {
    { "00000000000000000000000000000000", { "0000000000000000", "4ef997456198dd78" } },
    { "00000000000000000000000000000000", { "ffffffffffffffff", "014933e0cdaff6e4" } },
    { "00000000000000000000000000000000", { "0123456789abcdef", "19f40a0d847f51c3" } },
    { "00000000000000000000000000000000", { "fedcba9876543210", "09a5aa8371843981" } },
    { "ffffffffffffffffffffffffffffffff", { "0000000000000000", "f21e9a77b71c49bc" } },
    { "ffffffffffffffffffffffffffffffff", { "ffffffffffffffff", "51866fd5b85ecb8a" } },
    { "ffffffffffffffffffffffffffffffff", { "0123456789abcdef", "0b13debc7b8635cd" } },
    { "ffffffffffffffffffffffffffffffff", { "fedcba9876543210", "0d7b00c01a21e5c1" } },
    { "00112233445566778899aabbccddeeff", { "0000000000000000", "36d4e2502b003630" } },
    { "00112233445566778899aabbccddeeff", { "ffffffffffffffff", "77c465ae7a9a2077" } },
    { "00112233445566778899aabbccddeeff", { "0123456789abcdef", "dcec940a9cf3faa7" } },
    { "00112233445566778899aabbccddeeff", { "fedcba9876543210", "7d5c6b95237c00ca" } },
    { "ffeeddccbbaa99887766554433221100", { "0000000000000000", "2b04a56528a8498d" } },
    { "ffeeddccbbaa99887766554433221100", { "ffffffffffffffff", "d3f5d17de0e1b07b" } },
    { "ffeeddccbbaa99887766554433221100", { "0123456789abcdef", "bd16bc4918c27135" } },
    { "ffeeddccbbaa99887766554433221100", { "fedcba9876543210", "df7e18415a64fd86" } },
    { "0123456789abcdef0123456789abcdef", { "0000000000000000", "245946885754369a" } },
    { "0123456789abcdef0123456789abcdef", { "ffffffffffffffff", "1c6cd49835c7b012" } },
    { "0123456789abcdef0123456789abcdef", { "0123456789abcdef", "008a8314ee2b27a3" } },
    { "0123456789abcdef0123456789abcdef", { "fedcba9876543210", "9f5fac53492e0761" } },
    { "fedcba9876543210fedcba9876543210", { "0000000000000000", "2324986da676719c" } },
    { "fedcba9876543210fedcba9876543210", { "ffffffffffffffff", "6b5c5a9c5d9e0a5a" } },
    { "fedcba9876543210fedcba9876543210", { "0123456789abcdef", "0aceab0fc6a0a28d" } },
    { "fedcba9876543210fedcba9876543210", { "fedcba9876543210", "40c7ccd7789805f5" } },
  };

  for (size_t i = 0; i < test_cases.size(); ++i) {
    const std::string& key = hex2bin(test_cases[i].first);
    const std::string& plain = hex2bin(test_cases[i].second.first);
    const std::string& enc = hex2bin(test_cases[i].second.second);
    {
      std::ostringstream oss;
      yu::crypt::cipher_enc_ostream<yu::crypt::Blowfish_ECB, yu::crypt::NoPadding> es(oss, key);
      es.write(plain.data(), static_cast<std::streamsize>(plain.size()));
      es.finish();
      EXPECT(enc, ==, oss.str());
    }
    {
      std::ostringstream oss;
      yu::crypt::cipher_dec_ostream<yu::crypt::Blowfish_ECB, yu::crypt::NoPadding> es(oss, key);
      es.write(enc.data(), static_cast<std::streamsize>(enc.size()));
      es.finish();
      EXPECT(plain, ==, oss.str());
    }
  }
}

TEST(BlowFishTest, testCBCTestCases_nopadding) {
  // echo '0000000000000000' | xxd -ps -r | openssl bf-cbc -provider legacy -K 00000000000000000000000000000000 -iv 0000000000000000 -nopad | xxd -ps
  std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>> test_cases = {
    { { "00000000000000000000000000000000", "0000000000000000" }, { "00000000000000000000000000000000", "4ef997456198dd78e1c030e74c14d261" } },
    { { "00000000000000000000000000000000", "0000000000000000" }, { "ffffffffffffffffffffffffffffffff", "014933e0cdaff6e4cb173d866b305e4d" } },
    { { "00000000000000000000000000000000", "0000000000000000" }, { "00112233445566778899aabbccddeeff", "6b650d96ecd76d01be1a23624fd60793" } },
    { { "00000000000000000000000000000000", "0000000000000000" }, { "ffeeddccbbaa99887766554433221100", "f440f377e21c8cd7a574a48732b3cf6d" } },
    { { "00000000000000000000000000000000", "0000000000000000" }, { "0123456789abcdef0123456789abcdef", "19f40a0d847f51c3f2a36ba0f11cf534" } },
    { { "00000000000000000000000000000000", "0000000000000000" }, { "fedcba9876543210fedcba9876543210", "09a5aa8371843981df2cbea2b5545265" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "00000000000000000000000000000000", "014933e0cdaff6e42016cc63b245656a" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffffffff", "4ef997456198dd78f5341f960dacbf14" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "00112233445566778899aabbccddeeff", "f440f377e21c8cd71ea81c19c203e964" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "ffeeddccbbaa99887766554433221100", "6b650d96ecd76d01bb6fe147cb4f50fb" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "0123456789abcdef0123456789abcdef", "09a5aa837184398159f6002470c23881" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "fedcba9876543210fedcba9876543210", "19f40a0d847f51c349d2c9ab7a0400f8" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "00000000000000000000000000000000", "19f40a0d847f51c3bb0eec7223ce2b0b" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "ffffffffffffffffffffffffffffffff", "09a5aa8371843981a78e1832f48502db" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "00112233445566778899aabbccddeeff", "cb9190acb4dae35f8045e31a166fde9c" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "ffeeddccbbaa99887766554433221100", "5820552bb46271757984b58c3f62d1ed" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "0123456789abcdef0123456789abcdef", "4ef997456198dd78f6cd0605bcc5bb40" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "fedcba9876543210fedcba9876543210", "014933e0cdaff6e45e98f580cbce6de3" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "00000000000000000000000000000000", "09a5aa83718439813df63409c28151d3" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "ffffffffffffffffffffffffffffffff", "19f40a0d847f51c3ce05e99f55850ca6" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "00112233445566778899aabbccddeeff", "5820552bb46271751954520bfd96748d" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "ffeeddccbbaa99887766554433221100", "cb9190acb4dae35f92e047aeccd20dc5" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "0123456789abcdef0123456789abcdef", "014933e0cdaff6e4369b81770cd63403" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "fedcba9876543210fedcba9876543210", "4ef997456198dd7855d424be06fe723e" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "00000000000000000000000000000000", "f21e9a77b71c49bc92fad911711bd626" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "ffffffffffffffffffffffffffffffff", "51866fd5b85ecb8a5e299e3f053d25d8" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "00112233445566778899aabbccddeeff", "e74f522178fb590c9369acf7a5fd7e4d" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "ffeeddccbbaa99887766554433221100", "fd9972778079905a04a8bd443717b29d" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "0123456789abcdef0123456789abcdef", "0b13debc7b8635cd1dd175945d9ee56b" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "fedcba9876543210fedcba9876543210", "0d7b00c01a21e5c1cf1c6c907455976a" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "00000000000000000000000000000000", "51866fd5b85ecb8aba8a39a94193adc1" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffffffff", "f21e9a77b71c49bcb4109f5f31379421" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "00112233445566778899aabbccddeeff", "fd9972778079905a30374cbc0ac1080b" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "ffeeddccbbaa99887766554433221100", "e74f522178fb590c8813be3259e566ff" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "0123456789abcdef0123456789abcdef", "0d7b00c01a21e5c189a147ee2c75c6c8" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "fedcba9876543210fedcba9876543210", "0b13debc7b8635cd730bb1e8d7b82c22" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "00000000000000000000000000000000", "0b13debc7b8635cd1e0c1b4356a1f270" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "ffffffffffffffffffffffffffffffff", "0d7b00c01a21e5c158776e9ee73bb00b" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "00112233445566778899aabbccddeeff", "f721b4d93d85f82ab7318733755c07c4" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "ffeeddccbbaa99887766554433221100", "8cf1e5cc5d20417629a282190874d63d" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "0123456789abcdef0123456789abcdef", "f21e9a77b71c49bce226b0eae7c8bdf5" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "fedcba9876543210fedcba9876543210", "51866fd5b85ecb8a437fa3ff482665e8" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "00000000000000000000000000000000", "0d7b00c01a21e5c1b5478ddacd1a04dc" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "ffffffffffffffffffffffffffffffff", "0b13debc7b8635cdf8d281331a4752ca" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "00112233445566778899aabbccddeeff", "8cf1e5cc5d2041768a42f3be82c50fd4" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "ffeeddccbbaa99887766554433221100", "f721b4d93d85f82aa8ffa4c45e4fb57c" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "0123456789abcdef0123456789abcdef", "51866fd5b85ecb8a3db137756488eb6b" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "fedcba9876543210fedcba9876543210", "f21e9a77b71c49bcea70722ce1578e2c" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "00000000000000000000000000000000", "36d4e2502b0036308a0981b1c05ecdd6" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "ffffffffffffffffffffffffffffffff", "77c465ae7a9a20775c738a742e397d11" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "00112233445566778899aabbccddeeff", "4cf55a7a713b308bb58780466518463c" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "ffeeddccbbaa99887766554433221100", "1de0d1ce6c37b8a98e3d7bba1b19915a" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "0123456789abcdef0123456789abcdef", "dcec940a9cf3faa7069d0167ab15db85" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "fedcba9876543210fedcba9876543210", "7d5c6b95237c00ca257b3dac68c4ee44" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "00000000000000000000000000000000", "77c465ae7a9a20777cedb21611b54f35" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffffffff", "36d4e2502b00363038fa155b4b01fcfc" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "00112233445566778899aabbccddeeff", "1de0d1ce6c37b8a99f2884127a062e24" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "ffeeddccbbaa99887766554433221100", "4cf55a7a713b308b21f0de9f19e1f1b2" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "0123456789abcdef0123456789abcdef", "7d5c6b95237c00caaa0722f50a024b5d" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "fedcba9876543210fedcba9876543210", "dcec940a9cf3faa77e59e102cc4c3cbe" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "00000000000000000000000000000000", "dcec940a9cf3faa7918f4de01b24446d" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "ffffffffffffffffffffffffffffffff", "7d5c6b95237c00ca98ab8571795f4ef6" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "00112233445566778899aabbccddeeff", "74fc25ef7b4cfc297716f70aea63a3a2" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "ffeeddccbbaa99887766554433221100", "492eb75dc5ab66d800e74d02a36ecf09" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "0123456789abcdef0123456789abcdef", "36d4e2502b00363078edbc38e84bdb6f" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "fedcba9876543210fedcba9876543210", "77c465ae7a9a20771f749c5816ff4eca" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "00000000000000000000000000000000", "7d5c6b95237c00ca4a01a183b625c11a" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "ffffffffffffffffffffffffffffffff", "dcec940a9cf3faa7c263ff138d3539e4" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "00112233445566778899aabbccddeeff", "492eb75dc5ab66d8a7798fe67130ab3e" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "ffeeddccbbaa99887766554433221100", "74fc25ef7b4cfc293d65b6c2307f5e8f" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "0123456789abcdef0123456789abcdef", "77c465ae7a9a20778d67b4d96b25d3b5" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "fedcba9876543210fedcba9876543210", "36d4e2502b003630acc7b7b03529cffd" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "00000000000000000000000000000000", "2b04a56528a8498dd818a3babd355e37" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "ffffffffffffffffffffffffffffffff", "d3f5d17de0e1b07bc08870f17f6b981d" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "00112233445566778899aabbccddeeff", "df1a5086867e5b0c9dfbca1f62fdb0de" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "ffeeddccbbaa99887766554433221100", "35335dd9640b344d4bde3439e667f4b9" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "0123456789abcdef0123456789abcdef", "bd16bc4918c27135c3dd97340e205c3c" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "fedcba9876543210fedcba9876543210", "df7e18415a64fd86b50bd9a5f3b5de5e" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "00000000000000000000000000000000", "d3f5d17de0e1b07ba5d1db3d384876d0" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffffffff", "2b04a56528a8498d6c3c1b7ff55b72b8" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "00112233445566778899aabbccddeeff", "35335dd9640b344de11fdf61a81bdcc9" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "ffeeddccbbaa99887766554433221100", "df1a5086867e5b0cbe9265b4d4d061c0" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "0123456789abcdef0123456789abcdef", "df7e18415a64fd864bdf8c021f66f9d5" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "fedcba9876543210fedcba9876543210", "bd16bc4918c2713589d854bb16f38dad" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "00000000000000000000000000000000", "bd16bc4918c271359e32141a6b7c40bc" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "ffffffffffffffffffffffffffffffff", "df7e18415a64fd8646be064a09997e16" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "00112233445566778899aabbccddeeff", "ba1eea8d8c9fb632e3c208130df5f8bf" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "ffeeddccbbaa99887766554433221100", "f9cf3e7c86a8979309438e065b5a9820" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "0123456789abcdef0123456789abcdef", "2b04a56528a8498dd7a88357c95ecd07" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "fedcba9876543210fedcba9876543210", "d3f5d17de0e1b07b02407480ea58458d" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "00000000000000000000000000000000", "df7e18415a64fd8664ea660209a3c64c" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "ffffffffffffffffffffffffffffffff", "bd16bc4918c2713566e87c44bd065d64" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "00112233445566778899aabbccddeeff", "f9cf3e7c86a897931ee0c5e4af1b787f" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "ffeeddccbbaa99887766554433221100", "ba1eea8d8c9fb632122864dfae1f2aed" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "0123456789abcdef0123456789abcdef", "d3f5d17de0e1b07bf3b336d0dfd8b782" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "fedcba9876543210fedcba9876543210", "2b04a56528a8498d4be889bcf7f36f56" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "00000000000000000000000000000000", "245946885754369ae74f215b96c7b86d" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "ffffffffffffffffffffffffffffffff", "1c6cd49835c7b012df26f8d20476edfe" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "00112233445566778899aabbccddeeff", "362d146aded351e9d0b1ec61707c81dd" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "ffeeddccbbaa99887766554433221100", "dbabb7427fe7e97def1f6658bf0e3e3e" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "0123456789abcdef0123456789abcdef", "008a8314ee2b27a3de6089eeed84b92a" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "fedcba9876543210fedcba9876543210", "9f5fac53492e0761d55b0cd821f59f4e" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "00000000000000000000000000000000", "1c6cd49835c7b0128a12a5e4a947fa24" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffffffff", "245946885754369ac6d2ea47119fafd5" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "00112233445566778899aabbccddeeff", "dbabb7427fe7e97d020a5b9a0882ef76" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "ffeeddccbbaa99887766554433221100", "362d146aded351e92940c10e0c0517ff" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "0123456789abcdef0123456789abcdef", "9f5fac53492e0761f65e643451485123" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "fedcba9876543210fedcba9876543210", "008a8314ee2b27a366352e35ec14cc11" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "00000000000000000000000000000000", "008a8314ee2b27a3cb94e0b9d32ecbba" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "ffffffffffffffffffffffffffffffff", "9f5fac53492e0761df21b3f50798629a" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "00112233445566778899aabbccddeeff", "933587b8aecdf10bf20eef92fb83597f" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "ffeeddccbbaa99887766554433221100", "d14b03b57cc84443c86974b0c1b3fd2f" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "0123456789abcdef0123456789abcdef", "245946885754369a260ae5e0324579dc" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "fedcba9876543210fedcba9876543210", "1c6cd49835c7b0126c0c34d59706c46b" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "00000000000000000000000000000000", "9f5fac53492e076126ab6bfbb5c07df1" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "ffffffffffffffffffffffffffffffff", "008a8314ee2b27a33718d4866a4cda3d" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "00112233445566778899aabbccddeeff", "d14b03b57cc84443ec43ec5e4562ea60" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "ffeeddccbbaa99887766554433221100", "933587b8aecdf10b1d2136d397cc7d8c" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "0123456789abcdef0123456789abcdef", "1c6cd49835c7b012cedeee816d326399" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "fedcba9876543210fedcba9876543210", "245946885754369af438cf749ca0cac4" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "00000000000000000000000000000000", "2324986da676719cb7989842a8e194b1" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "ffffffffffffffffffffffffffffffff", "6b5c5a9c5d9e0a5a064a8418e8afd117" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "00112233445566778899aabbccddeeff", "8efefc92aa7d6abf705f9f7eefc03a7b" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "ffeeddccbbaa99887766554433221100", "dda249674c06a3ae1deb559db5fd407b" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "0123456789abcdef0123456789abcdef", "0aceab0fc6a0a28dd1921d4d06fd895d" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "fedcba9876543210fedcba9876543210", "40c7ccd7789805f5d279bcc56be8cd4c" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "00000000000000000000000000000000", "6b5c5a9c5d9e0a5a5f4abe4c38e2fe8b" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffffffff", "2324986da676719c6dc9c4401ac957fa" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "00112233445566778899aabbccddeeff", "dda249674c06a3ae2f67c25575494b14" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "ffeeddccbbaa99887766554433221100", "8efefc92aa7d6abfb7736aaa75ebdbfc" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "0123456789abcdef0123456789abcdef", "40c7ccd7789805f5775e9f745a5902b9" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "fedcba9876543210fedcba9876543210", "0aceab0fc6a0a28d313e4538dd6bd5e1" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "00000000000000000000000000000000", "0aceab0fc6a0a28db6553c45fe0025f6" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "ffffffffffffffffffffffffffffffff", "40c7ccd7789805f5752e65694d0ebdaf" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "00112233445566778899aabbccddeeff", "fab26baf08a902f6bc6c450f172f0f89" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "ffeeddccbbaa99887766554433221100", "fe2ea97cdf387b696a011f0ccc9d1879" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "0123456789abcdef0123456789abcdef", "2324986da676719cf7b1872be772c450" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "fedcba9876543210fedcba9876543210", "6b5c5a9c5d9e0a5a206a79a306cd0072" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "00000000000000000000000000000000", "40c7ccd7789805f52b464880e6259f02" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "ffffffffffffffffffffffffffffffff", "0aceab0fc6a0a28d2d8ac043d18e002c" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "00112233445566778899aabbccddeeff", "fe2ea97cdf387b697421725425c9d850" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "ffeeddccbbaa99887766554433221100", "fab26baf08a902f6ca1f205039befc6d" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "0123456789abcdef0123456789abcdef", "6b5c5a9c5d9e0a5a2bc3e113e54d892a" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "fedcba9876543210fedcba9876543210", "2324986da676719cfe7786f5822cb689" } },
  };

  for (size_t i = 0; i < test_cases.size(); ++i) {
    const std::string& key = hex2bin(test_cases[i].first.first);
    const std::string& iv = hex2bin(test_cases[i].first.second);
    const std::string& plain = hex2bin(test_cases[i].second.first);
    const std::string& enc = hex2bin(test_cases[i].second.second);
    {
      std::ostringstream oss;
      yu::crypt::cipher_enc_ostream<yu::crypt::Blowfish_CBC, yu::crypt::NoPadding> es(oss, key, iv);
      es.write(plain.data(), static_cast<std::streamsize>(plain.size()));
      es.finish();
      EXPECT(enc, ==, oss.str());
    }
    {
      std::ostringstream oss;
      yu::crypt::cipher_dec_ostream<yu::crypt::Blowfish_CBC, yu::crypt::NoPadding> es(oss, key, iv);
      es.write(enc.data(), static_cast<std::streamsize>(enc.size()));
      es.finish();
      EXPECT(plain, ==, oss.str());
    }
  }
}

TEST(BlowFishTest, testCFBTestCases_nopadding) {
  // echo '0000000000000000' | xxd -ps -r | openssl bf-cfb -provider legacy -K 00000000000000000000000000000000 -iv 0000000000000000 -nopad | xxd -ps
  std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>> test_cases = {
    { { "00000000000000000000000000000000", "0000000000000000" }, { "00000000000000000000000000000000", "4ef997456198dd78e1c030e74c14d261" } },
    { { "00000000000000000000000000000000", "0000000000000000" }, { "ffffffffffffffffffffffffffffffff", "b10668ba9e6722870acbe069f25340eb" } },
    { { "00000000000000000000000000000000", "0000000000000000" }, { "00112233445566778899aabbccddeeff", "4ee8b57625cdbb0fd50f32179f6629c8" } },
    { { "00000000000000000000000000000000", "0000000000000000" }, { "ffeeddccbbaa99887766554433221100", "b1174a89da3244f0b285b2ed335d4318" } },
    { { "00000000000000000000000000000000", "0000000000000000" }, { "0123456789abcdef0123456789abcdef", "4fdad222e8331097f7ee4362356e76af" } },
    { { "00000000000000000000000000000000", "0000000000000000" }, { "fedcba9876543210fedcba9876543210", "b0252ddd17ccef68ab089e2670aa402e" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "00000000000000000000000000000000", "014933e0cdaff6e42016cc63b245656a" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffffffff", "feb6cc1f3250091b34e8c27994cfa1b2" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "00112233445566778899aabbccddeeff", "015811d389fa90938448fa19cfb5e321" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "ffeeddccbbaa99887766554433221100", "fea7ee2c76056f6cd43e0a8dd008e2dc" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "0123456789abcdef0123456789abcdef", "006a768744043b0b37b8c410857df9ec" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "fedcba9876543210fedcba9876543210", "ff958978bbfbc4f4a0444f18bd9a5ff3" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "00000000000000000000000000000000", "19f40a0d847f51c3bb0eec7223ce2b0b" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "ffffffffffffffffffffffffffffffff", "e60bf5f27b80ae3c31fa1660aa7af359" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "00112233445566778899aabbccddeeff", "19e5283ec02a37b4b549671573227d64" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "ffeeddccbbaa99887766554433221100", "e61ad7c13fd5c84b116248c6ccf3eb31" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "0123456789abcdef0123456789abcdef", "18d74f6a0dd49c2cf3802ec778b738db" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "fedcba9876543210fedcba9876543210", "e728b095f22b63d3b70e73330c5032e8" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "00000000000000000000000000000000", "09a5aa83718439813df63409c28151d3" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "ffffffffffffffffffffffffffffffff", "f65a557c8e7bc67e5871e7cd0b7afd24" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "00112233445566778899aabbccddeeff", "09b488b035d15ff60699f9c4f50a3e59" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "ffeeddccbbaa99887766554433221100", "f64b774fca2ea009b0561330e410bd4e" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "0123456789abcdef0123456789abcdef", "0886efe4f82ff46e58d54543f969f56e" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "fedcba9876543210fedcba9876543210", "f779101b07d00b9121f0043ac3006075" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "00000000000000000000000000000000", "f21e9a77b71c49bc92fad911711bd626" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "ffffffffffffffffffffffffffffffff", "0de1658848e3b6434bef60a0cec86bde" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "00112233445566778899aabbccddeeff", "f20fb844f3492fcb10df9b1ddeaf326f" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "ffeeddccbbaa99887766554433221100", "0df047bb0cb6d03472dd24cf1dd6209a" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "0123456789abcdef0123456789abcdef", "f33ddf103eb78453e305f58d6e63701a" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "fedcba9876543210fedcba9876543210", "0cc220efc1487bac14acc8b49703bc3c" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "00000000000000000000000000000000", "51866fd5b85ecb8aba8a39a94193adc1" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffffffff", "ae79902a47a13475a1d661c0fac2da27" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "00112233445566778899aabbccddeeff", "51974de6fc0badfd01943c651b7f676e" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "ffeeddccbbaa99887766554433221100", "ae68b21903f45202a8a5970457e57112" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "0123456789abcdef0123456789abcdef", "50a52ab231f506653c927212ed232684" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "fedcba9876543210fedcba9876543210", "af5ad54dce0af99abda319673e7257f8" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "00000000000000000000000000000000", "0b13debc7b8635cd1e0c1b4356a1f270" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "ffffffffffffffffffffffffffffffff", "f4ec21438479ca32072d7ecce5b8ad35" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "00112233445566778899aabbccddeeff", "0b02fc8f3fd353bab3d4da98d93ee2b0" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "ffeeddccbbaa99887766554433221100", "f4fd0370c02cac4571f937bc8f3a4cd0" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "0123456789abcdef0123456789abcdef", "0a309bdbf22df8221cf230f3d4352884" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "fedcba9876543210fedcba9876543210", "f5cf64240dd207dd8dd70b70a1ec1e32" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "00000000000000000000000000000000", "0d7b00c01a21e5c1b5478ddacd1a04dc" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "ffffffffffffffffffffffffffffffff", "f284ff3fe5de1a3ea788916118c44ff4" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "00112233445566778899aabbccddeeff", "0d6a22f35e7483b63d3793a591dcf174" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "ffeeddccbbaa99887766554433221100", "f295dd0ca18b7c49984b1bbe8201e0af" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "0123456789abcdef0123456789abcdef", "0c5845a7938a282e88820289a5de0b27" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "fedcba9876543210fedcba9876543210", "f3a7ba586c75d7d131c0d6080201a57a" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "00000000000000000000000000000000", "36d4e2502b0036308a0981b1c05ecdd6" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "ffffffffffffffffffffffffffffffff", "c92b1dafd4ffc9cfc705eaa4b4fe0303" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "00112233445566778899aabbccddeeff", "36c5c0636f5550470d44993de507a787" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "ffeeddccbbaa99887766554433221100", "c93a3f9c90aaafb8aff2c5cc31f6e480" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "0123456789abcdef0123456789abcdef", "37f7a737a2abfbdf79cef95f61e01680" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "fedcba9876543210fedcba9876543210", "c80858c85d540420521b0d28437dfded" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "00000000000000000000000000000000", "77c465ae7a9a20777cedb21611b54f35" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffffffff", "883b9a518565df88a38c758bd1c682ee" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "00112233445566778899aabbccddeeff", "77d5479d3ecf4600ce4a5ea8fcb291f3" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "ffeeddccbbaa99887766554433221100", "882ab862c130b9fffd2cc024118eb555" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "0123456789abcdef0123456789abcdef", "76e720c9f331ed988c44f1bee28e1e5a" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "fedcba9876543210fedcba9876543210", "8918df360cce1267e1a826c060ab7cda" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "00000000000000000000000000000000", "dcec940a9cf3faa7918f4de01b24446d" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "ffffffffffffffffffffffffffffffff", "23136bf5630c05583d9c00ec72cac61b" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "00112233445566778899aabbccddeeff", "dcfdb639d8a69cd03042ea06a137a08d" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "ffeeddccbbaa99887766554433221100", "230249c62759632f652781368f204dd2" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "0123456789abcdef0123456789abcdef", "ddcfd16d1558374807be440022be166a" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "fedcba9876543210fedcba9876543210", "22302e92eaa7c8b780855b9aba180eae" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "00000000000000000000000000000000", "7d5c6b95237c00ca4a01a183b625c11a" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "ffffffffffffffffffffffffffffffff", "82a3946adc83ff3567547a8e86a0b109" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "00112233445566778899aabbccddeeff", "7d4d49a6672966bdc5d686f8b069d771" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "ffeeddccbbaa99887766554433221100", "82b2b65998d699422afb43ede209e24d" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "0123456789abcdef0123456789abcdef", "7c7f2ef2aad7cd25ab24679283a986b2" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "fedcba9876543210fedcba9876543210", "8380d10d552832dadba787341e90dc54" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "00000000000000000000000000000000", "2b04a56528a8498dd818a3babd355e37" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "ffffffffffffffffffffffffffffffff", "d4fb5a9ad757b67293c3e4800aa48d47" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "00112233445566778899aabbccddeeff", "2b1587566cfd2ffa971ff61c8719ecfd" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "ffeeddccbbaa99887766554433221100", "d4ea78a99302d0057412fe5075f75307" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "0123456789abcdef0123456789abcdef", "2a27e002a1038462d68bc63040f500e8" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "fedcba9876543210fedcba9876543210", "d5d81ffd5efc7b9db534332481a75d46" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "00000000000000000000000000000000", "d3f5d17de0e1b07ba5d1db3d384876d0" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffffffff", "2c0a2e821f1e4f843f778f0e809467e2" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "00112233445566778899aabbccddeeff", "d3e4f34ea4b4d60cccc970cf3db6e0cc" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "ffeeddccbbaa99887766554433221100", "2c1b0cb15b4b29f380724e3f316fc34b" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "0123456789abcdef0123456789abcdef", "d2d6941a694a7d94f29073b756737a6d" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "fedcba9876543210fedcba9876543210", "2d296be596b5826bfc9cce189c0c779d" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "00000000000000000000000000000000", "bd16bc4918c271359e32141a6b7c40bc" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "ffffffffffffffffffffffffffffffff", "42e943b6e73d8eca991783bb42f9a29b" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "00112233445566778899aabbccddeeff", "bd079e7a5c971742599fdfce1c149fc6" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "ffeeddccbbaa99887766554433221100", "42f86185a368e8bde9c78f922f4c6784" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "0123456789abcdef0123456789abcdef", "bc35f92e9169bcdac2fed253878b91d3" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "fedcba9876543210fedcba9876543210", "43ca06d16e9643257704ee2360a7bfbd" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "00000000000000000000000000000000", "df7e18415a64fd8664ea660209a3c64c" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "ffffffffffffffffffffffffffffffff", "2081e7bea59b0279b941f9b5f66681e9" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "00112233445566778899aabbccddeeff", "df6f3a721e319bf1f8a370f2b8bfa0bf" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "ffeeddccbbaa99887766554433221100", "2090c58de1ce640eb77b8e320e6afb70" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "0123456789abcdef0123456789abcdef", "de5d5d26d3cf30694afcc96596cd343a" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "fedcba9876543210fedcba9876543210", "21a2a2d92c30cf964bd7633d85e1ec4e" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "00000000000000000000000000000000", "245946885754369ae74f215b96c7b86d" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "ffffffffffffffffffffffffffffffff", "dba6b977a8abc965392d15b8ee60502a" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "00112233445566778899aabbccddeeff", "244864bb130150edc1503c63939771f8" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "ffeeddccbbaa99887766554433221100", "dbb79b44ecfeaf12414b0d5635ce1893" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "0123456789abcdef0123456789abcdef", "257a03efdefffb752729a087bbeeb433" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "fedcba9876543210fedcba9876543210", "da85fc102100048a0ae475eceaf4f8d4" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "00000000000000000000000000000000", "1c6cd49835c7b0128a12a5e4a947fa24" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffffffff", "e3932b67ca384fed20d9072dfb891201" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "00112233445566778899aabbccddeeff", "1c7df6ab7192d665af92f45a2708e47e" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "ffeeddccbbaa99887766554433221100", "e38209548e6d299a332cc60d16ba75e0" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "0123456789abcdef0123456789abcdef", "1d4f91ffbc6c7dfdcffdabe6e499ae76" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "fedcba9876543210fedcba9876543210", "e2b06e004393820292d08e4de152f67b" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "00000000000000000000000000000000", "008a8314ee2b27a3cb94e0b9d32ecbba" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "ffffffffffffffffffffffffffffffff", "ff757ceb11d4d85cc8e72b7995b325c2" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "00112233445566778899aabbccddeeff", "009ba127aa7e41d434ef4baaf246208f" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "ffeeddccbbaa99887766554433221100", "ff645ed85581be2b61c83d997ef785c3" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "0123456789abcdef0123456789abcdef", "01a9c6736780ea4cdf43cc89642f74c5" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "fedcba9876543210fedcba9876543210", "fe56398c987f15b398e994ad9a40fe01" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "00000000000000000000000000000000", "9f5fac53492e076126ab6bfbb5c07df1" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "ffffffffffffffffffffffffffffffff", "60a053acb6d1f89e20de4c0af8679d65" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "00112233445566778899aabbccddeeff", "9f4e8e600d7b6116990e847de30f69af" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "ffeeddccbbaa99887766554433221100", "60b1719ff2849ee964a75c7b8d62414a" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "0123456789abcdef0123456789abcdef", "9e7ce934c085ca8ef77d2153d8e39ccc" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "fedcba9876543210fedcba9876543210", "618316cb3f7a35712b87b64057a1ad5e" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "00000000000000000000000000000000", "2324986da676719cb7989842a8e194b1" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "ffffffffffffffffffffffffffffffff", "dcdb679259898e6392363bbfe536a805" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "00112233445566778899aabbccddeeff", "2335ba5ee22317eb4f9cdf4c3f946962" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "ffeeddccbbaa99887766554433221100", "dcca45a11ddce814590ddc9bd67d38c0" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "0123456789abcdef0123456789abcdef", "2207dd0a2fddbc73f692c24c6ed909bf" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "fedcba9876543210fedcba9876543210", "ddf822f5d022438c00ab3c6df4788499" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "00000000000000000000000000000000", "6b5c5a9c5d9e0a5a5f4abe4c38e2fe8b" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffffffff", "94a3a563a261f5a5f9b57be717502ee8" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "00112233445566778899aabbccddeeff", "6b4d78af19cb6c2d10ee802eee04d0fe" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "ffeeddccbbaa99887766554433221100", "94b28750e63493d2727c4d0b1d2ffd7b" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "0123456789abcdef0123456789abcdef", "6a7f1ffbd435c7b52ae0a4746ce644c5" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "fedcba9876543210fedcba9876543210", "9580e0042bca384adeb6c33b70993262" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "00000000000000000000000000000000", "0aceab0fc6a0a28db6553c45fe0025f6" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "ffffffffffffffffffffffffffffffff", "f53154f0395f5d72d2753fbc2e71ffd3" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "00112233445566778899aabbccddeeff", "0adf893c82f5c4fa9aa06985d7b578ba" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "ffeeddccbbaa99887766554433221100", "f52076c37d0a3b05ec6a7d230336708f" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "0123456789abcdef0123456789abcdef", "0bedee684f0b6f62d0b1582a8f5644b2" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "fedcba9876543210fedcba9876543210", "f4121197b0f4909dcfe2ffa0ab3fe7f1" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "00000000000000000000000000000000", "40c7ccd7789805f52b464880e6259f02" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "ffffffffffffffffffffffffffffffff", "bf3833288767fa0a8ad19a96b2f14250" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "00112233445566778899aabbccddeeff", "40d6eee43ccd6382357a72c8c437e11e" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "ffeeddccbbaa99887766554433221100", "bf29111bc3329c7d65777db378dd568b" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "0123456789abcdef0123456789abcdef", "41e489b0f133c81a767dda13d3f2cf56" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "fedcba9876543210fedcba9876543210", "be1b764f0ecc37e52ca5065d1dbcff5c" } },
  };

  for (size_t i = 0; i < test_cases.size(); ++i) {
    const std::string& key = hex2bin(test_cases[i].first.first);
    const std::string& iv = hex2bin(test_cases[i].first.second);
    const std::string& plain = hex2bin(test_cases[i].second.first);
    const std::string& enc = hex2bin(test_cases[i].second.second);
    {
      std::ostringstream oss;
      yu::crypt::cipher_enc_ostream<yu::crypt::Blowfish_CFB, yu::crypt::NoPadding> es(oss, key, iv);
      es.write(plain.data(), static_cast<std::streamsize>(plain.size()));
      es.finish();
      EXPECT(enc, ==, oss.str());
    }
    {
      std::ostringstream oss;
      yu::crypt::cipher_dec_ostream<yu::crypt::Blowfish_CFB, yu::crypt::NoPadding> es(oss, key, iv);
      es.write(enc.data(), static_cast<std::streamsize>(enc.size()));
      es.finish();
      EXPECT(plain, ==, oss.str());
    }
  }
}

TEST(BlowFishTest, testOFBTestCases_nopadding) {
  // echo '0000000000000000' | xxd -ps -r | openssl bf-ofb -provider legacy -K 00000000000000000000000000000000 -iv 0000000000000000 -nopad | xxd -ps
  std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>> test_cases = {
    { { "00000000000000000000000000000000", "0000000000000000" }, { "00000000000000000000000000000000", "4ef997456198dd78e1c030e74c14d261" } },
    { { "00000000000000000000000000000000", "0000000000000000" }, { "ffffffffffffffffffffffffffffffff", "b10668ba9e6722871e3fcf18b3eb2d9e" } },
    { { "00000000000000000000000000000000", "0000000000000000" }, { "00112233445566778899aabbccddeeff", "4ee8b57625cdbb0f69599a5c80c93c9e" } },
    { { "00000000000000000000000000000000", "0000000000000000" }, { "ffeeddccbbaa99887766554433221100", "b1174a89da3244f096a665a37f36c361" } },
    { { "00000000000000000000000000000000", "0000000000000000" }, { "0123456789abcdef0123456789abcdef", "4fdad222e8331097e0e37580c5bf1f8e" } },
    { { "00000000000000000000000000000000", "0000000000000000" }, { "fedcba9876543210fedcba9876543210", "b0252ddd17ccef681f1c8a7f3a40e071" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "00000000000000000000000000000000", "014933e0cdaff6e42016cc63b245656a" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffffffff", "feb6cc1f3250091bdfe9339c4dba9a95" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "00112233445566778899aabbccddeeff", "015811d389fa9093a88f66d87e988b95" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "ffeeddccbbaa99887766554433221100", "fea7ee2c76056f6c577099278167746a" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "0123456789abcdef0123456789abcdef", "006a768744043b0b213589043beea885" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "fedcba9876543210fedcba9876543210", "ff958978bbfbc4f4deca76fbc411577a" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "00000000000000000000000000000000", "19f40a0d847f51c3bb0eec7223ce2b0b" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "ffffffffffffffffffffffffffffffff", "e60bf5f27b80ae3c44f1138ddc31d4f4" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "00112233445566778899aabbccddeeff", "19e5283ec02a37b4339746c9ef13c5f4" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "ffeeddccbbaa99887766554433221100", "e61ad7c13fd5c84bcc68b93610ec3a0b" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "0123456789abcdef0123456789abcdef", "18d74f6a0dd49c2cba2da915aa65e6e4" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "fedcba9876543210fedcba9876543210", "e728b095f22b63d345d256ea559a191b" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "00000000000000000000000000000000", "09a5aa83718439813df63409c28151d3" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "ffffffffffffffffffffffffffffffff", "f65a557c8e7bc67ec209cbf63d7eae2c" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "00112233445566778899aabbccddeeff", "09b488b035d15ff6b56f9eb20e5cbf2c" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "ffeeddccbbaa99887766554433221100", "f64b774fca2ea0094a90614df1a340d3" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "0123456789abcdef0123456789abcdef", "0886efe4f82ff46e3cd5716e4b2a9c3c" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "fedcba9876543210fedcba9876543210", "f779101b07d00b91c32a8e91b4d563c3" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "00000000000000000000000000000000", "f21e9a77b71c49bc92fad911711bd626" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "ffffffffffffffffffffffffffffffff", "0de1658848e3b6436d0526ee8ee429d9" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "00112233445566778899aabbccddeeff", "f20fb844f3492fcb1a6373aabdc638d9" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "ffeeddccbbaa99887766554433221100", "0df047bb0cb6d034e59c8c554239c726" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "0123456789abcdef0123456789abcdef", "f33ddf103eb7845393d99c76f8b01bc9" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "fedcba9876543210fedcba9876543210", "0cc220efc1487bac6c266389074fe436" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "00000000000000000000000000000000", "51866fd5b85ecb8aba8a39a94193adc1" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffffffff", "ae79902a47a134754575c656be6c523e" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "00112233445566778899aabbccddeeff", "51974de6fc0badfd321393128d4e433e" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "ffeeddccbbaa99887766554433221100", "ae68b21903f45202cdec6ced72b1bcc1" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "0123456789abcdef0123456789abcdef", "50a52ab231f50665bba97ccec838602e" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "fedcba9876543210fedcba9876543210", "af5ad54dce0af99a4456833137c79fd1" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "00000000000000000000000000000000", "0b13debc7b8635cd1e0c1b4356a1f270" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "ffffffffffffffffffffffffffffffff", "f4ec21438479ca32e1f3e4bca95e0d8f" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "00112233445566778899aabbccddeeff", "0b02fc8f3fd353ba9695b1f89a7c1c8f" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "ffeeddccbbaa99887766554433221100", "f4fd0370c02cac45696a4e076583e370" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "0123456789abcdef0123456789abcdef", "0a309bdbf22df8221f2f5e24df0a3f9f" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "fedcba9876543210fedcba9876543210", "f5cf64240dd207dde0d0a1db20f5c060" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "00000000000000000000000000000000", "0d7b00c01a21e5c1b5478ddacd1a04dc" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "ffffffffffffffffffffffffffffffff", "f284ff3fe5de1a3e4ab8722532e5fb23" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "00112233445566778899aabbccddeeff", "0d6a22f35e7483b63dde276101c7ea23" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "ffeeddccbbaa99887766554433221100", "f295dd0ca18b7c49c221d89efe3815dc" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "0123456789abcdef0123456789abcdef", "0c5845a7938a282eb464c8bd44b1c933" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "fedcba9876543210fedcba9876543210", "f3a7ba586c75d7d14b9b3742bb4e36cc" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "00000000000000000000000000000000", "36d4e2502b0036308a0981b1c05ecdd6" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "ffffffffffffffffffffffffffffffff", "c92b1dafd4ffc9cf75f67e4e3fa13229" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "00112233445566778899aabbccddeeff", "36c5c0636f55504702902b0a0c832329" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "ffeeddccbbaa99887766554433221100", "c93a3f9c90aaafb8fd6fd4f5f37cdcd6" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "0123456789abcdef0123456789abcdef", "37f7a737a2abfbdf8b2ac4d649f50039" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "fedcba9876543210fedcba9876543210", "c80858c85d54042074d53b29b60affc6" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "00000000000000000000000000000000", "77c465ae7a9a20777cedb21611b54f35" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffffffff", "883b9a518565df8883124de9ee4ab0ca" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "00112233445566778899aabbccddeeff", "77d5479d3ecf4600f47418addd68a1ca" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "ffeeddccbbaa99887766554433221100", "882ab862c130b9ff0b8be75222975e35" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "0123456789abcdef0123456789abcdef", "76e720c9f331ed987dcef771981e82da" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "fedcba9876543210fedcba9876543210", "8918df360cce12678231088e67e17d25" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "00000000000000000000000000000000", "dcec940a9cf3faa7918f4de01b24446d" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "ffffffffffffffffffffffffffffffff", "23136bf5630c05586e70b21fe4dbbb92" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "00112233445566778899aabbccddeeff", "dcfdb639d8a69cd01916e75bd7f9aa92" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "ffeeddccbbaa99887766554433221100", "230249c62759632fe6e918a42806556d" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "0123456789abcdef0123456789abcdef", "ddcfd16d1558374890ac0887928f8982" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "fedcba9876543210fedcba9876543210", "22302e92eaa7c8b76f53f7786d70767d" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "00000000000000000000000000000000", "7d5c6b95237c00ca4a01a183b625c11a" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "ffffffffffffffffffffffffffffffff", "82a3946adc83ff35b5fe5e7c49da3ee5" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "00112233445566778899aabbccddeeff", "7d4d49a6672966bdc2980b387af82fe5" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "ffeeddccbbaa99887766554433221100", "82b2b65998d699423d67f4c78507d01a" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "0123456789abcdef0123456789abcdef", "7c7f2ef2aad7cd254b22e4e43f8e0cf5" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "fedcba9876543210fedcba9876543210", "8380d10d552832dab4dd1b1bc071f30a" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "00000000000000000000000000000000", "2b04a56528a8498dd818a3babd355e37" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "ffffffffffffffffffffffffffffffff", "d4fb5a9ad757b67227e75c4542caa1c8" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "00112233445566778899aabbccddeeff", "2b1587566cfd2ffa5081090171e8b0c8" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "ffeeddccbbaa99887766554433221100", "d4ea78a99302d005af7ef6fe8e174f37" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "0123456789abcdef0123456789abcdef", "2a27e002a1038462d93be6dd349e93d8" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "fedcba9876543210fedcba9876543210", "d5d81ffd5efc7b9d26c41922cb616c27" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "00000000000000000000000000000000", "d3f5d17de0e1b07ba5d1db3d384876d0" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffffffff", "2c0a2e821f1e4f845a2e24c2c7b7892f" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "00112233445566778899aabbccddeeff", "d3e4f34ea4b4d60c2d487186f495982f" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "ffeeddccbbaa99887766554433221100", "2c1b0cb15b4b29f3d2b78e790b6a67d0" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "0123456789abcdef0123456789abcdef", "d2d6941a694a7d94a4f29e5ab1e3bb3f" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "fedcba9876543210fedcba9876543210", "2d296be596b5826b5b0d61a54e1c44c0" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "00000000000000000000000000000000", "bd16bc4918c271359e32141a6b7c40bc" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "ffffffffffffffffffffffffffffffff", "42e943b6e73d8eca61cdebe59483bf43" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "00112233445566778899aabbccddeeff", "bd079e7a5c97174216abbea1a7a1ae43" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "ffeeddccbbaa99887766554433221100", "42f86185a368e8bde954415e585e51bc" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "0123456789abcdef0123456789abcdef", "bc35f92e9169bcda9f11517de2d78d53" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "fedcba9876543210fedcba9876543210", "43ca06d16e96432560eeae821d2872ac" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "00000000000000000000000000000000", "df7e18415a64fd8664ea660209a3c64c" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "ffffffffffffffffffffffffffffffff", "2081e7bea59b02799b1599fdf65c39b3" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "00112233445566778899aabbccddeeff", "df6f3a721e319bf1ec73ccb9c57e28b3" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "ffeeddccbbaa99887766554433221100", "2090c58de1ce640e138c33463a81d74c" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "0123456789abcdef0123456789abcdef", "de5d5d26d3cf306965c9236580080ba3" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "fedcba9876543210fedcba9876543210", "21a2a2d92c30cf969a36dc9a7ff7f45c" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "00000000000000000000000000000000", "245946885754369ae74f215b96c7b86d" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "ffffffffffffffffffffffffffffffff", "dba6b977a8abc96518b0dea469384792" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "00112233445566778899aabbccddeeff", "244864bb130150ed6fd68be05a1a5692" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "ffeeddccbbaa99887766554433221100", "dbb79b44ecfeaf129029741fa5e5a96d" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "0123456789abcdef0123456789abcdef", "257a03efdefffb75e66c643c1f6c7582" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "fedcba9876543210fedcba9876543210", "da85fc102100048a19939bc3e0938a7d" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "00000000000000000000000000000000", "1c6cd49835c7b0128a12a5e4a947fa24" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffffffff", "e3932b67ca384fed75ed5a1b56b805db" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "00112233445566778899aabbccddeeff", "1c7df6ab7192d665028b0f5f659a14db" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "ffeeddccbbaa99887766554433221100", "e38209548e6d299afd74f0a09a65eb24" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "0123456789abcdef0123456789abcdef", "1d4f91ffbc6c7dfd8b31e08320ec37cb" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "fedcba9876543210fedcba9876543210", "e2b06e004393820274ce1f7cdf13c834" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "00000000000000000000000000000000", "008a8314ee2b27a3cb94e0b9d32ecbba" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "ffffffffffffffffffffffffffffffff", "ff757ceb11d4d85c346b1f462cd13445" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "00112233445566778899aabbccddeeff", "009ba127aa7e41d4430d4a021ff32545" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "ffeeddccbbaa99887766554433221100", "ff645ed85581be2bbcf2b5fde00cdaba" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "0123456789abcdef0123456789abcdef", "01a9c6736780ea4ccab7a5de5a850655" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "fedcba9876543210fedcba9876543210", "fe56398c987f15b335485a21a57af9aa" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "00000000000000000000000000000000", "9f5fac53492e076126ab6bfbb5c07df1" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "ffffffffffffffffffffffffffffffff", "60a053acb6d1f89ed95494044a3f820e" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "00112233445566778899aabbccddeeff", "9f4e8e600d7b6116ae32c140791d930e" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "ffeeddccbbaa99887766554433221100", "60b1719ff2849ee951cd3ebf86e26cf1" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "0123456789abcdef0123456789abcdef", "9e7ce934c085ca8e27882e9c3c6bb01e" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "fedcba9876543210fedcba9876543210", "618316cb3f7a3571d877d163c3944fe1" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "00000000000000000000000000000000", "2324986da676719cb7989842a8e194b1" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "ffffffffffffffffffffffffffffffff", "dcdb679259898e63486767bd571e6b4e" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "00112233445566778899aabbccddeeff", "2335ba5ee22317eb3f0132f9643c7a4e" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "ffeeddccbbaa99887766554433221100", "dcca45a11ddce814c0fecd069bc385b1" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "0123456789abcdef0123456789abcdef", "2207dd0a2fddbc73b6bbdd25214a595e" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "fedcba9876543210fedcba9876543210", "ddf822f5d022438c494422dadeb5a6a1" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "00000000000000000000000000000000", "6b5c5a9c5d9e0a5a5f4abe4c38e2fe8b" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffffffff", "94a3a563a261f5a5a0b541b3c71d0174" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "00112233445566778899aabbccddeeff", "6b4d78af19cb6c2dd7d314f7f43f1074" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "ffeeddccbbaa99887766554433221100", "94b28750e63493d2282ceb080bc0ef8b" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "0123456789abcdef0123456789abcdef", "6a7f1ffbd435c7b55e69fb2bb1493364" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "fedcba9876543210fedcba9876543210", "9580e0042bca384aa19604d44eb6cc9b" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "00000000000000000000000000000000", "0aceab0fc6a0a28db6553c45fe0025f6" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "ffffffffffffffffffffffffffffffff", "f53154f0395f5d7249aac3ba01ffda09" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "00112233445566778899aabbccddeeff", "0adf893c82f5c4fa3ecc96fe32ddcb09" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "ffeeddccbbaa99887766554433221100", "f52076c37d0a3b05c1336901cd2234f6" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "0123456789abcdef0123456789abcdef", "0bedee684f0b6f62b776792277abe819" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "fedcba9876543210fedcba9876543210", "f4121197b0f4909d488986dd885417e6" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "00000000000000000000000000000000", "40c7ccd7789805f52b464880e6259f02" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "ffffffffffffffffffffffffffffffff", "bf3833288767fa0ad4b9b77f19da60fd" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "00112233445566778899aabbccddeeff", "40d6eee43ccd6382a3dfe23b2af871fd" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "ffeeddccbbaa99887766554433221100", "bf29111bc3329c7d5c201dc4d5078e02" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "0123456789abcdef0123456789abcdef", "41e489b0f133c81a2a650de76f8e52ed" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "fedcba9876543210fedcba9876543210", "be1b764f0ecc37e5d59af2189071ad12" } },
  };

  for (size_t i = 0; i < test_cases.size(); ++i) {
    const std::string& key = hex2bin(test_cases[i].first.first);
    const std::string& iv = hex2bin(test_cases[i].first.second);
    const std::string& plain = hex2bin(test_cases[i].second.first);
    const std::string& enc = hex2bin(test_cases[i].second.second);
    {
      std::ostringstream oss;
      yu::crypt::cipher_enc_ostream<yu::crypt::Blowfish_OFB, yu::crypt::NoPadding> es(oss, key, iv);
      es.write(plain.data(), static_cast<std::streamsize>(plain.size()));
      es.finish();
      EXPECT(enc, ==, oss.str());
    }
    {
      std::ostringstream oss;
      yu::crypt::cipher_dec_ostream<yu::crypt::Blowfish_OFB, yu::crypt::NoPadding> es(oss, key, iv);
      es.write(enc.data(), static_cast<std::streamsize>(enc.size()));
      es.finish();
      EXPECT(plain, ==, oss.str());
    }
    // OFB encrypt and decrypt is same
    {
      std::ostringstream oss;
      yu::crypt::cipher_dec_ostream<yu::crypt::Blowfish_OFB, yu::crypt::NoPadding> es(oss, key, iv);
      es.write(plain.data(), static_cast<std::streamsize>(plain.size()));
      es.finish();
      EXPECT(enc, ==, oss.str());
    }
    {
      std::ostringstream oss;
      yu::crypt::cipher_enc_ostream<yu::crypt::Blowfish_OFB, yu::crypt::NoPadding> es(oss, key, iv);
      es.write(enc.data(), static_cast<std::streamsize>(enc.size()));
      es.finish();
      EXPECT(plain, ==, oss.str());
    }
  }
}

TEST(BlowFishTest, testECBTestCases_with_padding) {
  // echo '0000000000000000' | xxd -ps -r | openssl bf-ecb -provider legacy -K 00000000000000000000000000000000 | xxd -ps
  std::vector<std::pair<std::string, std::pair<std::string, std::string>>> test_cases = {
    { "00000000000000000000000000000000", { "0000000000000000000000000000", "4ef997456198dd782b885e6900141caf" } },
    { "00000000000000000000000000000000", { "ffffffffffffffffffffffffffff", "014933e0cdaff6e48f852be9142dff26" } },
    { "00000000000000000000000000000000", { "00112233445566778899aabbccdd", "6b650d96ecd76d014ab6c1c5035de4d4" } },
    { "00000000000000000000000000000000", { "ffeeddccbbaa9988776655443322", "f440f377e21c8cd74932308990bcb9c4" } },
    { "00000000000000000000000000000000", { "0123456789abcdef0123456789ab", "19f40a0d847f51c3b5b764f844c9fb89" } },
    { "00000000000000000000000000000000", { "fedcba9876543210fedcba987654", "09a5aa8371843981577b5d37120d2a55" } },
    { "ffffffffffffffffffffffffffffffff", { "0000000000000000000000000000", "f21e9a77b71c49bc45af22f50b2d07c0" } },
    { "ffffffffffffffffffffffffffffffff", { "ffffffffffffffffffffffffffff", "51866fd5b85ecb8aba572f152b5bc413" } },
    { "ffffffffffffffffffffffffffffffff", { "00112233445566778899aabbccdd", "e74f522178fb590c7b46c0b6bd1ed58e" } },
    { "ffffffffffffffffffffffffffffffff", { "ffeeddccbbaa9988776655443322", "fd9972778079905aa8deeca8e3e1ef2a" } },
    { "ffffffffffffffffffffffffffffffff", { "0123456789abcdef0123456789ab", "0b13debc7b8635cdf5fe6446b37a3e9b" } },
    { "ffffffffffffffffffffffffffffffff", { "fedcba9876543210fedcba987654", "0d7b00c01a21e5c1d4cf072e0f3b7f85" } },
    { "00112233445566778899aabbccddeeff", { "0000000000000000000000000000", "36d4e2502b0036304422186c589651a5" } },
    { "00112233445566778899aabbccddeeff", { "ffffffffffffffffffffffffffff", "77c465ae7a9a2077ddefbb5def49b759" } },
    { "00112233445566778899aabbccddeeff", { "00112233445566778899aabbccdd", "4cf55a7a713b308b5ef40c66297cec41" } },
    { "00112233445566778899aabbccddeeff", { "ffeeddccbbaa9988776655443322", "1de0d1ce6c37b8a98b2f94162c0c1f14" } },
    { "00112233445566778899aabbccddeeff", { "0123456789abcdef0123456789ab", "dcec940a9cf3faa7f9200d0e497ee71e" } },
    { "00112233445566778899aabbccddeeff", { "fedcba9876543210fedcba987654", "7d5c6b95237c00ca3e3c3f241ca94ec9" } },
    { "ffeeddccbbaa99887766554433221100", { "0000000000000000000000000000", "2b04a56528a8498de81ceb497e88ec09" } },
    { "ffeeddccbbaa99887766554433221100", { "ffffffffffffffffffffffffffff", "d3f5d17de0e1b07bbebbdd0cc9d068f4" } },
    { "ffeeddccbbaa99887766554433221100", { "00112233445566778899aabbccdd", "df1a5086867e5b0c803cdd31ef043386" } },
    { "ffeeddccbbaa99887766554433221100", { "ffeeddccbbaa9988776655443322", "35335dd9640b344d154dcd9b81625de3" } },
    { "ffeeddccbbaa99887766554433221100", { "0123456789abcdef0123456789ab", "bd16bc4918c271354b64ec8686729009" } },
    { "ffeeddccbbaa99887766554433221100", { "fedcba9876543210fedcba987654", "df7e18415a64fd86acb9ba5e84137407" } },
    { "0123456789abcdef0123456789abcdef", { "0000000000000000000000000000", "245946885754369a0dee68c86afbb54d" } },
    { "0123456789abcdef0123456789abcdef", { "ffffffffffffffffffffffffffff", "1c6cd49835c7b012b2a097f87dfebfdf" } },
    { "0123456789abcdef0123456789abcdef", { "00112233445566778899aabbccdd", "362d146aded351e9d39efbdb20bed7e7" } },
    { "0123456789abcdef0123456789abcdef", { "ffeeddccbbaa9988776655443322", "dbabb7427fe7e97d9a35c7e5d2c6de40" } },
    { "0123456789abcdef0123456789abcdef", { "0123456789abcdef0123456789ab", "008a8314ee2b27a3cf91f0b37fe690a7" } },
    { "0123456789abcdef0123456789abcdef", { "fedcba9876543210fedcba987654", "9f5fac53492e0761ea85fbf486ffdddc" } },
    { "fedcba9876543210fedcba9876543210", { "0000000000000000000000000000", "2324986da676719c4c87a1a7a8120bdd" } },
    { "fedcba9876543210fedcba9876543210", { "ffffffffffffffffffffffffffff", "6b5c5a9c5d9e0a5a50d9abc1c145ab9d" } },
    { "fedcba9876543210fedcba9876543210", { "00112233445566778899aabbccdd", "8efefc92aa7d6abf982d55a94fd034f9" } },
    { "fedcba9876543210fedcba9876543210", { "ffeeddccbbaa9988776655443322", "dda249674c06a3ae35ce102e56f92904" } },
    { "fedcba9876543210fedcba9876543210", { "0123456789abcdef0123456789ab", "0aceab0fc6a0a28dbb91ede2a9b372d7" } },
    { "fedcba9876543210fedcba9876543210", { "fedcba9876543210fedcba987654", "40c7ccd7789805f54f6453477c04a1e1" } },
  };

  for (size_t i = 0; i < test_cases.size(); ++i) {
    const std::string& key = hex2bin(test_cases[i].first);
    const std::string& plain = hex2bin(test_cases[i].second.first);
    const std::string& enc = hex2bin(test_cases[i].second.second);
    {
      std::ostringstream oss;
      yu::crypt::blowfish_ecb_enc_ostream es(oss, key);
      es.write(plain.data(), static_cast<std::streamsize>(plain.size()));
      es.finish();
      EXPECT(enc, ==, oss.str());
    }
    {
      std::ostringstream oss;
      yu::crypt::blowfish_ecb_dec_ostream es(oss, key);
      es.write(enc.data(), static_cast<std::streamsize>(enc.size()));
      es.finish();
      EXPECT(plain, ==, oss.str());
    }
  }
}

TEST(BlowFishTest, testCBCTestCases_with_padding) {
  // echo '0000000000000000' | xxd -ps -r | openssl bf-cbc -provider legacy -K 00000000000000000000000000000000 -iv 0000000000000000 | xxd -ps
  std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>> test_cases = {
    { { "00000000000000000000000000000000", "0000000000000000" }, { "0000000000000000000000000000", "4ef997456198dd78ba44a85624708949" } },
    { { "00000000000000000000000000000000", "0000000000000000" }, { "ffffffffffffffffffffffffffff", "014933e0cdaff6e469f794ccefb137a2" } },
    { { "00000000000000000000000000000000", "0000000000000000" }, { "00112233445566778899aabbccdd", "6b650d96ecd76d01ae58db88cf9ceb56" } },
    { { "00000000000000000000000000000000", "0000000000000000" }, { "ffeeddccbbaa9988776655443322", "f440f377e21c8cd7258160c209ea50ee" } },
    { { "00000000000000000000000000000000", "0000000000000000" }, { "0123456789abcdef0123456789ab", "19f40a0d847f51c3bff0234c2bbe967e" } },
    { { "00000000000000000000000000000000", "0000000000000000" }, { "fedcba9876543210fedcba987654", "09a5aa83718439815426b34d17a69e7a" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "0000000000000000000000000000", "014933e0cdaff6e44ccba3cc33a96495" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffff", "4ef997456198dd78929d0fd6abb28d90" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "00112233445566778899aabbccdd", "f440f377e21c8cd71777021ecc34bf8b" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "ffeeddccbbaa9988776655443322", "6b650d96ecd76d01849375c780e0fdfc" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "0123456789abcdef0123456789ab", "09a5aa83718439814c478dfb3108cbf9" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "fedcba9876543210fedcba987654", "19f40a0d847f51c316148cf3ec241da4" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "0000000000000000000000000000", "19f40a0d847f51c3a4e524e05a4e24b3" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "ffffffffffffffffffffffffffff", "09a5aa8371843981e2a6b556a1917091" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "00112233445566778899aabbccdd", "cb9190acb4dae35f036dcb67d2b90d96" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "ffeeddccbbaa9988776655443322", "5820552bb4627175565dfa049c265aff" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "0123456789abcdef0123456789ab", "4ef997456198dd78ca4778bef386a071" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "fedcba9876543210fedcba987654", "014933e0cdaff6e4576e02d8cfb06454" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "0000000000000000000000000000", "09a5aa83718439811924aa36aa68ce81" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "ffffffffffffffffffffffffffff", "19f40a0d847f51c31a9e97da8053c89d" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "00112233445566778899aabbccdd", "5820552bb4627175feaa03df96abd83e" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "ffeeddccbbaa9988776655443322", "cb9190acb4dae35f1d813e3ee410bd98" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "0123456789abcdef0123456789ab", "014933e0cdaff6e49b92dd58bac400dd" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "fedcba9876543210fedcba987654", "4ef997456198dd789f77e7cffdedbeee" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "0000000000000000000000000000", "f21e9a77b71c49bcdbe68ecdc17ee6f4" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "ffffffffffffffffffffffffffff", "51866fd5b85ecb8ae53bb8e60cf705f2" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "00112233445566778899aabbccdd", "e74f522178fb590c24d7a30fb6a42be6" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "ffeeddccbbaa9988776655443322", "fd9972778079905ac7fb522c91963e7a" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "0123456789abcdef0123456789ab", "0b13debc7b8635cd7b1e04d497e27c36" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "fedcba9876543210fedcba987654", "0d7b00c01a21e5c1b7bc487c8a72c998" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "0000000000000000000000000000", "51866fd5b85ecb8a3b2b7ec33a39b9ae" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffff", "f21e9a77b71c49bc44f2636ec8e54d19" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "00112233445566778899aabbccdd", "fd9972778079905ac4b12d93baa308f0" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "ffeeddccbbaa9988776655443322", "e74f522178fb590c6302df7d0cebfb3f" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "0123456789abcdef0123456789ab", "0d7b00c01a21e5c1f9b1ceb97134c955" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "fedcba9876543210fedcba987654", "0b13debc7b8635cd8de50eb51eeab7e4" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "0000000000000000000000000000", "0b13debc7b8635cdee6bd4df9556d28c" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "ffffffffffffffffffffffffffff", "0d7b00c01a21e5c161d72859f232736a" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "00112233445566778899aabbccdd", "f721b4d93d85f82aaec1ec13f267966c" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "ffeeddccbbaa9988776655443322", "8cf1e5cc5d204176d9cb209c26c216f2" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "0123456789abcdef0123456789ab", "f21e9a77b71c49bc71217f9cfd6cd6f2" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "fedcba9876543210fedcba987654", "51866fd5b85ecb8a61f4b6bb280bb022" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "0000000000000000000000000000", "0d7b00c01a21e5c15b9f9c53d1604bfa" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "ffffffffffffffffffffffffffff", "0b13debc7b8635cd1e36e9ab095d7122" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "00112233445566778899aabbccdd", "8cf1e5cc5d204176d9cfc11331972b82" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "ffeeddccbbaa9988776655443322", "f721b4d93d85f82a7ba3d6d9ab1797fd" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "0123456789abcdef0123456789ab", "51866fd5b85ecb8ae7185ab052bf7222" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "fedcba9876543210fedcba987654", "f21e9a77b71c49bc1324332258096675" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "0000000000000000000000000000", "36d4e2502b0036301024cdfd61ca5208" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "ffffffffffffffffffffffffffff", "77c465ae7a9a207791fcabe7260f6194" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "00112233445566778899aabbccdd", "4cf55a7a713b308b504d84f0f1f719eb" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "ffeeddccbbaa9988776655443322", "1de0d1ce6c37b8a9cb5735712f048ddb" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "0123456789abcdef0123456789ab", "dcec940a9cf3faa7acd2b21a677b16bf" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "fedcba9876543210fedcba987654", "7d5c6b95237c00caa21cd0a2a3671299" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "0000000000000000000000000000", "77c465ae7a9a2077e1df48e55657e9bb" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffff", "36d4e2502b003630b28bdb2e36e49a14" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "00112233445566778899aabbccdd", "1de0d1ce6c37b8a9c6422dd53de28bc0" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "ffeeddccbbaa9988776655443322", "4cf55a7a713b308bcbe5ff0cd9d5df15" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "0123456789abcdef0123456789ab", "7d5c6b95237c00ca1785bbae67ed5c62" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "fedcba9876543210fedcba987654", "dcec940a9cf3faa7b667503fa415c448" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "0000000000000000000000000000", "dcec940a9cf3faa7467bf05e96d35263" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "ffffffffffffffffffffffffffff", "7d5c6b95237c00ca8895097a0f998f89" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "00112233445566778899aabbccdd", "74fc25ef7b4cfc297f4016274ef5b770" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "ffeeddccbbaa9988776655443322", "492eb75dc5ab66d80a2555eb6a708e83" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "0123456789abcdef0123456789ab", "36d4e2502b00363093ded72c2dc9cb02" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "fedcba9876543210fedcba987654", "77c465ae7a9a2077b7e5c88514b42c50" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "0000000000000000000000000000", "7d5c6b95237c00ca3b80a74ffb97cc82" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "ffffffffffffffffffffffffffff", "dcec940a9cf3faa703b61058a6bf464d" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "00112233445566778899aabbccdd", "492eb75dc5ab66d8622a36ef68869663" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "ffeeddccbbaa9988776655443322", "74fc25ef7b4cfc298444317b88463ac8" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "0123456789abcdef0123456789ab", "77c465ae7a9a20779cc261533e45d6fb" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "fedcba9876543210fedcba987654", "36d4e2502b003630c842b3892662c92c" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "0000000000000000000000000000", "2b04a56528a8498dc6c57a0330c5f33e" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "ffffffffffffffffffffffffffff", "d3f5d17de0e1b07ba5462df4cf1ecfa8" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "00112233445566778899aabbccdd", "df1a5086867e5b0cb676513ad8e05304" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "ffeeddccbbaa9988776655443322", "35335dd9640b344db0379a2f4619c4d5" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "0123456789abcdef0123456789ab", "bd16bc4918c27135806197f54b5651ef" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "fedcba9876543210fedcba987654", "df7e18415a64fd86762e41bb3e1901b7" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "0000000000000000000000000000", "d3f5d17de0e1b07b6bb204533aae21d9" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffff", "2b04a56528a8498da9aebc5f2d84cd04" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "00112233445566778899aabbccdd", "35335dd9640b344d3e8cc34ae6df7e6f" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "ffeeddccbbaa9988776655443322", "df1a5086867e5b0c63eca0209c5fef08" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "0123456789abcdef0123456789ab", "df7e18415a64fd86a3bb274cac4096cd" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "fedcba9876543210fedcba987654", "bd16bc4918c27135cde47e842919ba5b" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "0000000000000000000000000000", "bd16bc4918c271354062c0d728da5ae1" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "ffffffffffffffffffffffffffff", "df7e18415a64fd86ca0656d66ebe178d" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "00112233445566778899aabbccdd", "ba1eea8d8c9fb632b310372395fd6c8e" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "ffeeddccbbaa9988776655443322", "f9cf3e7c86a8979330594d8d08a4a473" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "0123456789abcdef0123456789ab", "2b04a56528a8498df39cca72adc0394f" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "fedcba9876543210fedcba987654", "d3f5d17de0e1b07b46ae30fd02318e60" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "0000000000000000000000000000", "df7e18415a64fd86e834a2c5a7527d0b" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "ffffffffffffffffffffffffffff", "bd16bc4918c27135db9de7102687fe7a" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "00112233445566778899aabbccdd", "f9cf3e7c86a89793c2d86dfb7aecc1c0" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "ffeeddccbbaa9988776655443322", "ba1eea8d8c9fb632bec7347d95bbd262" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "0123456789abcdef0123456789ab", "d3f5d17de0e1b07bff87fe84c01657e6" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "fedcba9876543210fedcba987654", "2b04a56528a8498d9cd712163bfa43df" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "0000000000000000000000000000", "245946885754369af44f0ab9a197d169" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "ffffffffffffffffffffffffffff", "1c6cd49835c7b012958fea88c8899c2a" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "00112233445566778899aabbccdd", "362d146aded351e99f35691eb06d1e06" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "ffeeddccbbaa9988776655443322", "dbabb7427fe7e97d6c985a55332ce2bb" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "0123456789abcdef0123456789ab", "008a8314ee2b27a383d2b2bf6136a8fa" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "fedcba9876543210fedcba987654", "9f5fac53492e076174da889d7c1f3c2f" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "0000000000000000000000000000", "1c6cd49835c7b012955817fa7d8bb9dc" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffff", "245946885754369a1e5f02d8960bd24b" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "00112233445566778899aabbccdd", "dbabb7427fe7e97d2ee6f5856ca7c40d" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "ffeeddccbbaa9988776655443322", "362d146aded351e90e181eb4ce397335" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "0123456789abcdef0123456789ab", "9f5fac53492e07613e9b01952ba4ea65" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "fedcba9876543210fedcba987654", "008a8314ee2b27a331de67ac7e441673" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "0000000000000000000000000000", "008a8314ee2b27a38944ff763b6d54ea" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "ffffffffffffffffffffffffffff", "9f5fac53492e0761fd69eb8e22c2ada8" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "00112233445566778899aabbccdd", "933587b8aecdf10b6e096b07df5407da" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "ffeeddccbbaa9988776655443322", "d14b03b57cc84443ecedf19043915b05" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "0123456789abcdef0123456789ab", "245946885754369a642618710a89d567" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "fedcba9876543210fedcba987654", "1c6cd49835c7b01260c54cebba13fc7a" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "0000000000000000000000000000", "9f5fac53492e07610df6b9835d182b1b" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "ffffffffffffffffffffffffffff", "008a8314ee2b27a36afaca4e173ad59c" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "00112233445566778899aabbccdd", "d14b03b57cc8444324830245bc7082ca" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "ffeeddccbbaa9988776655443322", "933587b8aecdf10b85cff423fcee6a39" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "0123456789abcdef0123456789ab", "1c6cd49835c7b012919ea23501965892" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "fedcba9876543210fedcba987654", "245946885754369aee82ae27dd1430d1" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "0000000000000000000000000000", "2324986da676719caf4832fb66939d86" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "ffffffffffffffffffffffffffff", "6b5c5a9c5d9e0a5a990b98660cc69d4d" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "00112233445566778899aabbccdd", "8efefc92aa7d6abfc29a83c633ecc8aa" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "ffeeddccbbaa9988776655443322", "dda249674c06a3ae0e59d98d02631db4" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "0123456789abcdef0123456789ab", "0aceab0fc6a0a28d67acaaa611e3d1ce" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "fedcba9876543210fedcba987654", "40c7ccd7789805f559bc117fa93466a8" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "0000000000000000000000000000", "6b5c5a9c5d9e0a5af6a71f788a9944ae" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffff", "2324986da676719c9355149cecddc294" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "00112233445566778899aabbccdd", "dda249674c06a3ae0bdb15d4453af2bc" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "ffeeddccbbaa9988776655443322", "8efefc92aa7d6abff6270ab08483ffe3" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "0123456789abcdef0123456789ab", "40c7ccd7789805f52a26abaa921c1879" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "fedcba9876543210fedcba987654", "0aceab0fc6a0a28d51c6cb8148251b7e" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "0000000000000000000000000000", "0aceab0fc6a0a28da6f65d4eb437d730" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "ffffffffffffffffffffffffffff", "40c7ccd7789805f5b2608a6a19a90662" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "00112233445566778899aabbccdd", "fab26baf08a902f6a67e036762586f5c" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "ffeeddccbbaa9988776655443322", "fe2ea97cdf387b6984681ccdccb22542" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "0123456789abcdef0123456789ab", "2324986da676719c9e8ba6555f779fa6" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "fedcba9876543210fedcba987654", "6b5c5a9c5d9e0a5aed54c56fecf78864" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "0000000000000000000000000000", "40c7ccd7789805f58b290053c26d6c3d" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "ffffffffffffffffffffffffffff", "0aceab0fc6a0a28de874abaa36717d81" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "00112233445566778899aabbccdd", "fe2ea97cdf387b693d64632da16899e8" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "ffeeddccbbaa9988776655443322", "fab26baf08a902f6a286f8969c02ee22" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "0123456789abcdef0123456789ab", "6b5c5a9c5d9e0a5a49694b12b750f181" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "fedcba9876543210fedcba987654", "2324986da676719c44dd54e756f62e66" } },
  };

  for (size_t i = 0; i < test_cases.size(); ++i) {
    const std::string& key = hex2bin(test_cases[i].first.first);
    const std::string& iv = hex2bin(test_cases[i].first.second);
    const std::string& plain = hex2bin(test_cases[i].second.first);
    const std::string& enc = hex2bin(test_cases[i].second.second);
    {
      std::ostringstream oss;
      yu::crypt::blowfish_cbc_enc_ostream es(oss, key, iv);
      es.write(plain.data(), static_cast<std::streamsize>(plain.size()));
      es.finish();
      EXPECT(enc, ==, oss.str());
    }
    {
      std::ostringstream oss;
      yu::crypt::blowfish_cbc_dec_ostream es(oss, key, iv);
      es.write(enc.data(), static_cast<std::streamsize>(enc.size()));
      es.finish();
      EXPECT(plain, ==, oss.str());
    }
  }
}

TEST(BlowFishTest, testCFBTestCases_stream) {
  // echo '0000000000000000' | xxd -ps -r | openssl bf-cfb -provider legacy -K 00000000000000000000000000000000 -iv 0000000000000000 | xxd -ps
  std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>> test_cases = {
    { { "00000000000000000000000000000000", "0000000000000000" }, { "0000000000000000000000000000", "4ef997456198dd78e1c030e74c14" } },
    { { "00000000000000000000000000000000", "0000000000000000" }, { "ffffffffffffffffffffffffffff", "b10668ba9e6722870acbe069f253" } },
    { { "00000000000000000000000000000000", "0000000000000000" }, { "00112233445566778899aabbccdd", "4ee8b57625cdbb0fd50f32179f66" } },
    { { "00000000000000000000000000000000", "0000000000000000" }, { "ffeeddccbbaa9988776655443322", "b1174a89da3244f0b285b2ed335d" } },
    { { "00000000000000000000000000000000", "0000000000000000" }, { "0123456789abcdef0123456789ab", "4fdad222e8331097f7ee4362356e" } },
    { { "00000000000000000000000000000000", "0000000000000000" }, { "fedcba9876543210fedcba987654", "b0252ddd17ccef68ab089e2670aa" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "0000000000000000000000000000", "014933e0cdaff6e42016cc63b245" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffff", "feb6cc1f3250091b34e8c27994cf" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "00112233445566778899aabbccdd", "015811d389fa90938448fa19cfb5" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "ffeeddccbbaa9988776655443322", "fea7ee2c76056f6cd43e0a8dd008" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "0123456789abcdef0123456789ab", "006a768744043b0b37b8c410857d" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "fedcba9876543210fedcba987654", "ff958978bbfbc4f4a0444f18bd9a" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "0000000000000000000000000000", "19f40a0d847f51c3bb0eec7223ce" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "ffffffffffffffffffffffffffff", "e60bf5f27b80ae3c31fa1660aa7a" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "00112233445566778899aabbccdd", "19e5283ec02a37b4b54967157322" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "ffeeddccbbaa9988776655443322", "e61ad7c13fd5c84b116248c6ccf3" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "0123456789abcdef0123456789ab", "18d74f6a0dd49c2cf3802ec778b7" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "fedcba9876543210fedcba987654", "e728b095f22b63d3b70e73330c50" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "0000000000000000000000000000", "09a5aa83718439813df63409c281" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "ffffffffffffffffffffffffffff", "f65a557c8e7bc67e5871e7cd0b7a" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "00112233445566778899aabbccdd", "09b488b035d15ff60699f9c4f50a" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "ffeeddccbbaa9988776655443322", "f64b774fca2ea009b0561330e410" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "0123456789abcdef0123456789ab", "0886efe4f82ff46e58d54543f969" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "fedcba9876543210fedcba987654", "f779101b07d00b9121f0043ac300" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "0000000000000000000000000000", "f21e9a77b71c49bc92fad911711b" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "ffffffffffffffffffffffffffff", "0de1658848e3b6434bef60a0cec8" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "00112233445566778899aabbccdd", "f20fb844f3492fcb10df9b1ddeaf" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "ffeeddccbbaa9988776655443322", "0df047bb0cb6d03472dd24cf1dd6" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "0123456789abcdef0123456789ab", "f33ddf103eb78453e305f58d6e63" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "fedcba9876543210fedcba987654", "0cc220efc1487bac14acc8b49703" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "0000000000000000000000000000", "51866fd5b85ecb8aba8a39a94193" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffff", "ae79902a47a13475a1d661c0fac2" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "00112233445566778899aabbccdd", "51974de6fc0badfd01943c651b7f" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "ffeeddccbbaa9988776655443322", "ae68b21903f45202a8a5970457e5" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "0123456789abcdef0123456789ab", "50a52ab231f506653c927212ed23" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "fedcba9876543210fedcba987654", "af5ad54dce0af99abda319673e72" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "0000000000000000000000000000", "0b13debc7b8635cd1e0c1b4356a1" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "ffffffffffffffffffffffffffff", "f4ec21438479ca32072d7ecce5b8" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "00112233445566778899aabbccdd", "0b02fc8f3fd353bab3d4da98d93e" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "ffeeddccbbaa9988776655443322", "f4fd0370c02cac4571f937bc8f3a" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "0123456789abcdef0123456789ab", "0a309bdbf22df8221cf230f3d435" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "fedcba9876543210fedcba987654", "f5cf64240dd207dd8dd70b70a1ec" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "0000000000000000000000000000", "0d7b00c01a21e5c1b5478ddacd1a" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "ffffffffffffffffffffffffffff", "f284ff3fe5de1a3ea788916118c4" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "00112233445566778899aabbccdd", "0d6a22f35e7483b63d3793a591dc" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "ffeeddccbbaa9988776655443322", "f295dd0ca18b7c49984b1bbe8201" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "0123456789abcdef0123456789ab", "0c5845a7938a282e88820289a5de" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "fedcba9876543210fedcba987654", "f3a7ba586c75d7d131c0d6080201" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "0000000000000000000000000000", "36d4e2502b0036308a0981b1c05e" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "ffffffffffffffffffffffffffff", "c92b1dafd4ffc9cfc705eaa4b4fe" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "00112233445566778899aabbccdd", "36c5c0636f5550470d44993de507" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "ffeeddccbbaa9988776655443322", "c93a3f9c90aaafb8aff2c5cc31f6" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "0123456789abcdef0123456789ab", "37f7a737a2abfbdf79cef95f61e0" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "fedcba9876543210fedcba987654", "c80858c85d540420521b0d28437d" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "0000000000000000000000000000", "77c465ae7a9a20777cedb21611b5" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffff", "883b9a518565df88a38c758bd1c6" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "00112233445566778899aabbccdd", "77d5479d3ecf4600ce4a5ea8fcb2" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "ffeeddccbbaa9988776655443322", "882ab862c130b9fffd2cc024118e" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "0123456789abcdef0123456789ab", "76e720c9f331ed988c44f1bee28e" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "fedcba9876543210fedcba987654", "8918df360cce1267e1a826c060ab" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "0000000000000000000000000000", "dcec940a9cf3faa7918f4de01b24" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "ffffffffffffffffffffffffffff", "23136bf5630c05583d9c00ec72ca" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "00112233445566778899aabbccdd", "dcfdb639d8a69cd03042ea06a137" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "ffeeddccbbaa9988776655443322", "230249c62759632f652781368f20" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "0123456789abcdef0123456789ab", "ddcfd16d1558374807be440022be" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "fedcba9876543210fedcba987654", "22302e92eaa7c8b780855b9aba18" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "0000000000000000000000000000", "7d5c6b95237c00ca4a01a183b625" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "ffffffffffffffffffffffffffff", "82a3946adc83ff3567547a8e86a0" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "00112233445566778899aabbccdd", "7d4d49a6672966bdc5d686f8b069" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "ffeeddccbbaa9988776655443322", "82b2b65998d699422afb43ede209" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "0123456789abcdef0123456789ab", "7c7f2ef2aad7cd25ab24679283a9" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "fedcba9876543210fedcba987654", "8380d10d552832dadba787341e90" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "0000000000000000000000000000", "2b04a56528a8498dd818a3babd35" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "ffffffffffffffffffffffffffff", "d4fb5a9ad757b67293c3e4800aa4" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "00112233445566778899aabbccdd", "2b1587566cfd2ffa971ff61c8719" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "ffeeddccbbaa9988776655443322", "d4ea78a99302d0057412fe5075f7" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "0123456789abcdef0123456789ab", "2a27e002a1038462d68bc63040f5" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "fedcba9876543210fedcba987654", "d5d81ffd5efc7b9db534332481a7" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "0000000000000000000000000000", "d3f5d17de0e1b07ba5d1db3d3848" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffff", "2c0a2e821f1e4f843f778f0e8094" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "00112233445566778899aabbccdd", "d3e4f34ea4b4d60cccc970cf3db6" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "ffeeddccbbaa9988776655443322", "2c1b0cb15b4b29f380724e3f316f" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "0123456789abcdef0123456789ab", "d2d6941a694a7d94f29073b75673" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "fedcba9876543210fedcba987654", "2d296be596b5826bfc9cce189c0c" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "0000000000000000000000000000", "bd16bc4918c271359e32141a6b7c" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "ffffffffffffffffffffffffffff", "42e943b6e73d8eca991783bb42f9" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "00112233445566778899aabbccdd", "bd079e7a5c971742599fdfce1c14" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "ffeeddccbbaa9988776655443322", "42f86185a368e8bde9c78f922f4c" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "0123456789abcdef0123456789ab", "bc35f92e9169bcdac2fed253878b" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "fedcba9876543210fedcba987654", "43ca06d16e9643257704ee2360a7" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "0000000000000000000000000000", "df7e18415a64fd8664ea660209a3" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "ffffffffffffffffffffffffffff", "2081e7bea59b0279b941f9b5f666" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "00112233445566778899aabbccdd", "df6f3a721e319bf1f8a370f2b8bf" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "ffeeddccbbaa9988776655443322", "2090c58de1ce640eb77b8e320e6a" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "0123456789abcdef0123456789ab", "de5d5d26d3cf30694afcc96596cd" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "fedcba9876543210fedcba987654", "21a2a2d92c30cf964bd7633d85e1" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "0000000000000000000000000000", "245946885754369ae74f215b96c7" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "ffffffffffffffffffffffffffff", "dba6b977a8abc965392d15b8ee60" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "00112233445566778899aabbccdd", "244864bb130150edc1503c639397" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "ffeeddccbbaa9988776655443322", "dbb79b44ecfeaf12414b0d5635ce" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "0123456789abcdef0123456789ab", "257a03efdefffb752729a087bbee" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "fedcba9876543210fedcba987654", "da85fc102100048a0ae475eceaf4" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "0000000000000000000000000000", "1c6cd49835c7b0128a12a5e4a947" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffff", "e3932b67ca384fed20d9072dfb89" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "00112233445566778899aabbccdd", "1c7df6ab7192d665af92f45a2708" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "ffeeddccbbaa9988776655443322", "e38209548e6d299a332cc60d16ba" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "0123456789abcdef0123456789ab", "1d4f91ffbc6c7dfdcffdabe6e499" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "fedcba9876543210fedcba987654", "e2b06e004393820292d08e4de152" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "0000000000000000000000000000", "008a8314ee2b27a3cb94e0b9d32e" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "ffffffffffffffffffffffffffff", "ff757ceb11d4d85cc8e72b7995b3" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "00112233445566778899aabbccdd", "009ba127aa7e41d434ef4baaf246" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "ffeeddccbbaa9988776655443322", "ff645ed85581be2b61c83d997ef7" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "0123456789abcdef0123456789ab", "01a9c6736780ea4cdf43cc89642f" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "fedcba9876543210fedcba987654", "fe56398c987f15b398e994ad9a40" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "0000000000000000000000000000", "9f5fac53492e076126ab6bfbb5c0" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "ffffffffffffffffffffffffffff", "60a053acb6d1f89e20de4c0af867" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "00112233445566778899aabbccdd", "9f4e8e600d7b6116990e847de30f" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "ffeeddccbbaa9988776655443322", "60b1719ff2849ee964a75c7b8d62" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "0123456789abcdef0123456789ab", "9e7ce934c085ca8ef77d2153d8e3" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "fedcba9876543210fedcba987654", "618316cb3f7a35712b87b64057a1" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "0000000000000000000000000000", "2324986da676719cb7989842a8e1" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "ffffffffffffffffffffffffffff", "dcdb679259898e6392363bbfe536" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "00112233445566778899aabbccdd", "2335ba5ee22317eb4f9cdf4c3f94" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "ffeeddccbbaa9988776655443322", "dcca45a11ddce814590ddc9bd67d" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "0123456789abcdef0123456789ab", "2207dd0a2fddbc73f692c24c6ed9" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "fedcba9876543210fedcba987654", "ddf822f5d022438c00ab3c6df478" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "0000000000000000000000000000", "6b5c5a9c5d9e0a5a5f4abe4c38e2" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffff", "94a3a563a261f5a5f9b57be71750" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "00112233445566778899aabbccdd", "6b4d78af19cb6c2d10ee802eee04" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "ffeeddccbbaa9988776655443322", "94b28750e63493d2727c4d0b1d2f" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "0123456789abcdef0123456789ab", "6a7f1ffbd435c7b52ae0a4746ce6" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "fedcba9876543210fedcba987654", "9580e0042bca384adeb6c33b7099" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "0000000000000000000000000000", "0aceab0fc6a0a28db6553c45fe00" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "ffffffffffffffffffffffffffff", "f53154f0395f5d72d2753fbc2e71" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "00112233445566778899aabbccdd", "0adf893c82f5c4fa9aa06985d7b5" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "ffeeddccbbaa9988776655443322", "f52076c37d0a3b05ec6a7d230336" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "0123456789abcdef0123456789ab", "0bedee684f0b6f62d0b1582a8f56" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "fedcba9876543210fedcba987654", "f4121197b0f4909dcfe2ffa0ab3f" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "0000000000000000000000000000", "40c7ccd7789805f52b464880e625" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "ffffffffffffffffffffffffffff", "bf3833288767fa0a8ad19a96b2f1" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "00112233445566778899aabbccdd", "40d6eee43ccd6382357a72c8c437" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "ffeeddccbbaa9988776655443322", "bf29111bc3329c7d65777db378dd" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "0123456789abcdef0123456789ab", "41e489b0f133c81a767dda13d3f2" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "fedcba9876543210fedcba987654", "be1b764f0ecc37e52ca5065d1dbc" } },
  };

  for (size_t i = 0; i < test_cases.size(); ++i) {
    const std::string& key = hex2bin(test_cases[i].first.first);
    const std::string& iv = hex2bin(test_cases[i].first.second);
    const std::string& plain = hex2bin(test_cases[i].second.first);
    const std::string& enc = hex2bin(test_cases[i].second.second);
    {
      std::ostringstream oss;
      yu::crypt::blowfish_cfb_enc_ostream es(oss, key, iv);
      es.write(plain.data(), static_cast<std::streamsize>(plain.size()));
      es.finish();
      EXPECT(enc, ==, oss.str());
    }
    {
      std::ostringstream oss;
      yu::crypt::blowfish_cfb_dec_ostream es(oss, key, iv);
      es.write(enc.data(), static_cast<std::streamsize>(enc.size()));
      es.finish();
      EXPECT(plain, ==, oss.str());
    }
  }
}

TEST(BlowFishTest, testOFBTestCases_stream) {
  // echo '0000000000000000' | xxd -ps -r | openssl bf-ofb -provider legacy -K 00000000000000000000000000000000 -iv 0000000000000000 | xxd -ps
  std::vector<std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>> test_cases = {
    { { "00000000000000000000000000000000", "0000000000000000" }, { "0000000000000000000000000000", "4ef997456198dd78e1c030e74c14" } },
    { { "00000000000000000000000000000000", "0000000000000000" }, { "ffffffffffffffffffffffffffff", "b10668ba9e6722871e3fcf18b3eb" } },
    { { "00000000000000000000000000000000", "0000000000000000" }, { "00112233445566778899aabbccdd", "4ee8b57625cdbb0f69599a5c80c9" } },
    { { "00000000000000000000000000000000", "0000000000000000" }, { "ffeeddccbbaa9988776655443322", "b1174a89da3244f096a665a37f36" } },
    { { "00000000000000000000000000000000", "0000000000000000" }, { "0123456789abcdef0123456789ab", "4fdad222e8331097e0e37580c5bf" } },
    { { "00000000000000000000000000000000", "0000000000000000" }, { "fedcba9876543210fedcba987654", "b0252ddd17ccef681f1c8a7f3a40" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "0000000000000000000000000000", "014933e0cdaff6e42016cc63b245" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffff", "feb6cc1f3250091bdfe9339c4dba" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "00112233445566778899aabbccdd", "015811d389fa9093a88f66d87e98" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "ffeeddccbbaa9988776655443322", "fea7ee2c76056f6c577099278167" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "0123456789abcdef0123456789ab", "006a768744043b0b213589043bee" } },
    { { "00000000000000000000000000000000", "ffffffffffffffff" }, { "fedcba9876543210fedcba987654", "ff958978bbfbc4f4deca76fbc411" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "0000000000000000000000000000", "19f40a0d847f51c3bb0eec7223ce" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "ffffffffffffffffffffffffffff", "e60bf5f27b80ae3c44f1138ddc31" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "00112233445566778899aabbccdd", "19e5283ec02a37b4339746c9ef13" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "ffeeddccbbaa9988776655443322", "e61ad7c13fd5c84bcc68b93610ec" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "0123456789abcdef0123456789ab", "18d74f6a0dd49c2cba2da915aa65" } },
    { { "00000000000000000000000000000000", "0123456789abcdef" }, { "fedcba9876543210fedcba987654", "e728b095f22b63d345d256ea559a" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "0000000000000000000000000000", "09a5aa83718439813df63409c281" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "ffffffffffffffffffffffffffff", "f65a557c8e7bc67ec209cbf63d7e" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "00112233445566778899aabbccdd", "09b488b035d15ff6b56f9eb20e5c" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "ffeeddccbbaa9988776655443322", "f64b774fca2ea0094a90614df1a3" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "0123456789abcdef0123456789ab", "0886efe4f82ff46e3cd5716e4b2a" } },
    { { "00000000000000000000000000000000", "fedcba9876543210" }, { "fedcba9876543210fedcba987654", "f779101b07d00b91c32a8e91b4d5" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "0000000000000000000000000000", "f21e9a77b71c49bc92fad911711b" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "ffffffffffffffffffffffffffff", "0de1658848e3b6436d0526ee8ee4" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "00112233445566778899aabbccdd", "f20fb844f3492fcb1a6373aabdc6" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "ffeeddccbbaa9988776655443322", "0df047bb0cb6d034e59c8c554239" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "0123456789abcdef0123456789ab", "f33ddf103eb7845393d99c76f8b0" } },
    { { "ffffffffffffffffffffffffffffffff", "0000000000000000" }, { "fedcba9876543210fedcba987654", "0cc220efc1487bac6c266389074f" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "0000000000000000000000000000", "51866fd5b85ecb8aba8a39a94193" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffff", "ae79902a47a134754575c656be6c" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "00112233445566778899aabbccdd", "51974de6fc0badfd321393128d4e" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "ffeeddccbbaa9988776655443322", "ae68b21903f45202cdec6ced72b1" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "0123456789abcdef0123456789ab", "50a52ab231f50665bba97ccec838" } },
    { { "ffffffffffffffffffffffffffffffff", "ffffffffffffffff" }, { "fedcba9876543210fedcba987654", "af5ad54dce0af99a4456833137c7" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "0000000000000000000000000000", "0b13debc7b8635cd1e0c1b4356a1" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "ffffffffffffffffffffffffffff", "f4ec21438479ca32e1f3e4bca95e" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "00112233445566778899aabbccdd", "0b02fc8f3fd353ba9695b1f89a7c" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "ffeeddccbbaa9988776655443322", "f4fd0370c02cac45696a4e076583" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "0123456789abcdef0123456789ab", "0a309bdbf22df8221f2f5e24df0a" } },
    { { "ffffffffffffffffffffffffffffffff", "0123456789abcdef" }, { "fedcba9876543210fedcba987654", "f5cf64240dd207dde0d0a1db20f5" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "0000000000000000000000000000", "0d7b00c01a21e5c1b5478ddacd1a" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "ffffffffffffffffffffffffffff", "f284ff3fe5de1a3e4ab8722532e5" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "00112233445566778899aabbccdd", "0d6a22f35e7483b63dde276101c7" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "ffeeddccbbaa9988776655443322", "f295dd0ca18b7c49c221d89efe38" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "0123456789abcdef0123456789ab", "0c5845a7938a282eb464c8bd44b1" } },
    { { "ffffffffffffffffffffffffffffffff", "fedcba9876543210" }, { "fedcba9876543210fedcba987654", "f3a7ba586c75d7d14b9b3742bb4e" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "0000000000000000000000000000", "36d4e2502b0036308a0981b1c05e" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "ffffffffffffffffffffffffffff", "c92b1dafd4ffc9cf75f67e4e3fa1" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "00112233445566778899aabbccdd", "36c5c0636f55504702902b0a0c83" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "ffeeddccbbaa9988776655443322", "c93a3f9c90aaafb8fd6fd4f5f37c" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "0123456789abcdef0123456789ab", "37f7a737a2abfbdf8b2ac4d649f5" } },
    { { "00112233445566778899aabbccddeeff", "0000000000000000" }, { "fedcba9876543210fedcba987654", "c80858c85d54042074d53b29b60a" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "0000000000000000000000000000", "77c465ae7a9a20777cedb21611b5" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffff", "883b9a518565df8883124de9ee4a" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "00112233445566778899aabbccdd", "77d5479d3ecf4600f47418addd68" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "ffeeddccbbaa9988776655443322", "882ab862c130b9ff0b8be7522297" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "0123456789abcdef0123456789ab", "76e720c9f331ed987dcef771981e" } },
    { { "00112233445566778899aabbccddeeff", "ffffffffffffffff" }, { "fedcba9876543210fedcba987654", "8918df360cce12678231088e67e1" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "0000000000000000000000000000", "dcec940a9cf3faa7918f4de01b24" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "ffffffffffffffffffffffffffff", "23136bf5630c05586e70b21fe4db" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "00112233445566778899aabbccdd", "dcfdb639d8a69cd01916e75bd7f9" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "ffeeddccbbaa9988776655443322", "230249c62759632fe6e918a42806" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "0123456789abcdef0123456789ab", "ddcfd16d1558374890ac0887928f" } },
    { { "00112233445566778899aabbccddeeff", "0123456789abcdef" }, { "fedcba9876543210fedcba987654", "22302e92eaa7c8b76f53f7786d70" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "0000000000000000000000000000", "7d5c6b95237c00ca4a01a183b625" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "ffffffffffffffffffffffffffff", "82a3946adc83ff35b5fe5e7c49da" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "00112233445566778899aabbccdd", "7d4d49a6672966bdc2980b387af8" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "ffeeddccbbaa9988776655443322", "82b2b65998d699423d67f4c78507" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "0123456789abcdef0123456789ab", "7c7f2ef2aad7cd254b22e4e43f8e" } },
    { { "00112233445566778899aabbccddeeff", "fedcba9876543210" }, { "fedcba9876543210fedcba987654", "8380d10d552832dab4dd1b1bc071" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "0000000000000000000000000000", "2b04a56528a8498dd818a3babd35" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "ffffffffffffffffffffffffffff", "d4fb5a9ad757b67227e75c4542ca" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "00112233445566778899aabbccdd", "2b1587566cfd2ffa5081090171e8" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "ffeeddccbbaa9988776655443322", "d4ea78a99302d005af7ef6fe8e17" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "0123456789abcdef0123456789ab", "2a27e002a1038462d93be6dd349e" } },
    { { "ffeeddccbbaa99887766554433221100", "0000000000000000" }, { "fedcba9876543210fedcba987654", "d5d81ffd5efc7b9d26c41922cb61" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "0000000000000000000000000000", "d3f5d17de0e1b07ba5d1db3d3848" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffff", "2c0a2e821f1e4f845a2e24c2c7b7" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "00112233445566778899aabbccdd", "d3e4f34ea4b4d60c2d487186f495" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "ffeeddccbbaa9988776655443322", "2c1b0cb15b4b29f3d2b78e790b6a" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "0123456789abcdef0123456789ab", "d2d6941a694a7d94a4f29e5ab1e3" } },
    { { "ffeeddccbbaa99887766554433221100", "ffffffffffffffff" }, { "fedcba9876543210fedcba987654", "2d296be596b5826b5b0d61a54e1c" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "0000000000000000000000000000", "bd16bc4918c271359e32141a6b7c" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "ffffffffffffffffffffffffffff", "42e943b6e73d8eca61cdebe59483" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "00112233445566778899aabbccdd", "bd079e7a5c97174216abbea1a7a1" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "ffeeddccbbaa9988776655443322", "42f86185a368e8bde954415e585e" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "0123456789abcdef0123456789ab", "bc35f92e9169bcda9f11517de2d7" } },
    { { "ffeeddccbbaa99887766554433221100", "0123456789abcdef" }, { "fedcba9876543210fedcba987654", "43ca06d16e96432560eeae821d28" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "0000000000000000000000000000", "df7e18415a64fd8664ea660209a3" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "ffffffffffffffffffffffffffff", "2081e7bea59b02799b1599fdf65c" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "00112233445566778899aabbccdd", "df6f3a721e319bf1ec73ccb9c57e" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "ffeeddccbbaa9988776655443322", "2090c58de1ce640e138c33463a81" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "0123456789abcdef0123456789ab", "de5d5d26d3cf306965c923658008" } },
    { { "ffeeddccbbaa99887766554433221100", "fedcba9876543210" }, { "fedcba9876543210fedcba987654", "21a2a2d92c30cf969a36dc9a7ff7" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "0000000000000000000000000000", "245946885754369ae74f215b96c7" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "ffffffffffffffffffffffffffff", "dba6b977a8abc96518b0dea46938" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "00112233445566778899aabbccdd", "244864bb130150ed6fd68be05a1a" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "ffeeddccbbaa9988776655443322", "dbb79b44ecfeaf129029741fa5e5" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "0123456789abcdef0123456789ab", "257a03efdefffb75e66c643c1f6c" } },
    { { "0123456789abcdef0123456789abcdef", "0000000000000000" }, { "fedcba9876543210fedcba987654", "da85fc102100048a19939bc3e093" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "0000000000000000000000000000", "1c6cd49835c7b0128a12a5e4a947" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffff", "e3932b67ca384fed75ed5a1b56b8" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "00112233445566778899aabbccdd", "1c7df6ab7192d665028b0f5f659a" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "ffeeddccbbaa9988776655443322", "e38209548e6d299afd74f0a09a65" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "0123456789abcdef0123456789ab", "1d4f91ffbc6c7dfd8b31e08320ec" } },
    { { "0123456789abcdef0123456789abcdef", "ffffffffffffffff" }, { "fedcba9876543210fedcba987654", "e2b06e004393820274ce1f7cdf13" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "0000000000000000000000000000", "008a8314ee2b27a3cb94e0b9d32e" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "ffffffffffffffffffffffffffff", "ff757ceb11d4d85c346b1f462cd1" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "00112233445566778899aabbccdd", "009ba127aa7e41d4430d4a021ff3" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "ffeeddccbbaa9988776655443322", "ff645ed85581be2bbcf2b5fde00c" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "0123456789abcdef0123456789ab", "01a9c6736780ea4ccab7a5de5a85" } },
    { { "0123456789abcdef0123456789abcdef", "0123456789abcdef" }, { "fedcba9876543210fedcba987654", "fe56398c987f15b335485a21a57a" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "0000000000000000000000000000", "9f5fac53492e076126ab6bfbb5c0" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "ffffffffffffffffffffffffffff", "60a053acb6d1f89ed95494044a3f" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "00112233445566778899aabbccdd", "9f4e8e600d7b6116ae32c140791d" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "ffeeddccbbaa9988776655443322", "60b1719ff2849ee951cd3ebf86e2" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "0123456789abcdef0123456789ab", "9e7ce934c085ca8e27882e9c3c6b" } },
    { { "0123456789abcdef0123456789abcdef", "fedcba9876543210" }, { "fedcba9876543210fedcba987654", "618316cb3f7a3571d877d163c394" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "0000000000000000000000000000", "2324986da676719cb7989842a8e1" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "ffffffffffffffffffffffffffff", "dcdb679259898e63486767bd571e" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "00112233445566778899aabbccdd", "2335ba5ee22317eb3f0132f9643c" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "ffeeddccbbaa9988776655443322", "dcca45a11ddce814c0fecd069bc3" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "0123456789abcdef0123456789ab", "2207dd0a2fddbc73b6bbdd25214a" } },
    { { "fedcba9876543210fedcba9876543210", "0000000000000000" }, { "fedcba9876543210fedcba987654", "ddf822f5d022438c494422dadeb5" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "0000000000000000000000000000", "6b5c5a9c5d9e0a5a5f4abe4c38e2" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "ffffffffffffffffffffffffffff", "94a3a563a261f5a5a0b541b3c71d" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "00112233445566778899aabbccdd", "6b4d78af19cb6c2dd7d314f7f43f" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "ffeeddccbbaa9988776655443322", "94b28750e63493d2282ceb080bc0" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "0123456789abcdef0123456789ab", "6a7f1ffbd435c7b55e69fb2bb149" } },
    { { "fedcba9876543210fedcba9876543210", "ffffffffffffffff" }, { "fedcba9876543210fedcba987654", "9580e0042bca384aa19604d44eb6" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "0000000000000000000000000000", "0aceab0fc6a0a28db6553c45fe00" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "ffffffffffffffffffffffffffff", "f53154f0395f5d7249aac3ba01ff" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "00112233445566778899aabbccdd", "0adf893c82f5c4fa3ecc96fe32dd" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "ffeeddccbbaa9988776655443322", "f52076c37d0a3b05c1336901cd22" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "0123456789abcdef0123456789ab", "0bedee684f0b6f62b776792277ab" } },
    { { "fedcba9876543210fedcba9876543210", "0123456789abcdef" }, { "fedcba9876543210fedcba987654", "f4121197b0f4909d488986dd8854" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "0000000000000000000000000000", "40c7ccd7789805f52b464880e625" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "ffffffffffffffffffffffffffff", "bf3833288767fa0ad4b9b77f19da" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "00112233445566778899aabbccdd", "40d6eee43ccd6382a3dfe23b2af8" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "ffeeddccbbaa9988776655443322", "bf29111bc3329c7d5c201dc4d507" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "0123456789abcdef0123456789ab", "41e489b0f133c81a2a650de76f8e" } },
    { { "fedcba9876543210fedcba9876543210", "fedcba9876543210" }, { "fedcba9876543210fedcba987654", "be1b764f0ecc37e5d59af2189071" } },
  };

  for (size_t i = 0; i < test_cases.size(); ++i) {
    const std::string& key = hex2bin(test_cases[i].first.first);
    const std::string& iv = hex2bin(test_cases[i].first.second);
    const std::string& plain = hex2bin(test_cases[i].second.first);
    const std::string& enc = hex2bin(test_cases[i].second.second);
    {
      std::ostringstream oss;
      yu::crypt::blowfish_ofb_enc_ostream es(oss, key, iv);
      es.write(plain.data(), static_cast<std::streamsize>(plain.size()));
      es.finish();
      EXPECT(enc, ==, oss.str());
    }
    {
      std::ostringstream oss;
      yu::crypt::blowfish_ofb_dec_ostream es(oss, key, iv);
      es.write(enc.data(), static_cast<std::streamsize>(enc.size()));
      es.finish();
      EXPECT(plain, ==, oss.str());
    }
    // OFB encrypt and decrypt is same
    {
      std::ostringstream oss;
      yu::crypt::blowfish_ofb_dec_ostream es(oss, key, iv);
      es.write(plain.data(), static_cast<std::streamsize>(plain.size()));
      es.finish();
      EXPECT(enc, ==, oss.str());
    }
    {
      std::ostringstream oss;
      yu::crypt::blowfish_ofb_enc_ostream es(oss, key, iv);
      es.write(enc.data(), static_cast<std::streamsize>(enc.size()));
      es.finish();
      EXPECT(plain, ==, oss.str());
    }
  }
}
