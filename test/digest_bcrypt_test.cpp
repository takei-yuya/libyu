#include "yu/digest/bcrypt.hpp"

#include "yu/test.hpp"

class BCryptTest : public yu::Test {
};

TEST(BCryptTest, testSha256) {
  // printf 'password' | openssl passwd -5 -salt 'rounds=1000$salt' -stdin
  EXPECT("$5$rounds=1000$salt$p.wiWs2zrZ7irikO2AL64QDIJo00A3KDq2xWHpLJGgB",
         ==, yu::digest::bcrypt_sha256("rounds=1000$salt", "password"));

  // change rounds
  EXPECT("$5$rounds=10000$salt$z7k5MrpzwRqIAw4S2Qj6c3ryVeZEqq2vxVSIJyS7UuD",
         ==, yu::digest::bcrypt_sha256("rounds=10000$salt", "password"));

  // default rounds
  EXPECT("$5$rounds=5000$salt$Gcm6FsVtF/Qa77ZKD.iwsJlCVPY0XSMgLJL0Hnww/c1",
         ==, yu::digest::bcrypt_sha256("rounds=5000$salt", "password"));
  EXPECT("$5$salt$Gcm6FsVtF/Qa77ZKD.iwsJlCVPY0XSMgLJL0Hnww/c1",
         ==, yu::digest::bcrypt_sha256("salt", "password"));

  // bin password
  EXPECT("$5$rounds=1000$salt$efY5sspsB5/gVjNHhKGPaSGUZTuv/YYWBIxMInXRsA.",
         ==, yu::digest::bcrypt_sha256("rounds=1000$salt", "pass\x00word"));

  // long password
  EXPECT("$5$rounds=1000$salt$RIbQGYDburSMMo67uL6Cr7F1c/fFfJ36aixEM7SZh7D",
         ==, yu::digest::bcrypt_sha256("rounds=1000$salt", "long_long_long_long_long_long_long_long_long_long_long_long_password"));

  // long salt
  EXPECT("$5$rounds=1000$0123456789abcdef$Om.m8JkEhXG/FobQKhKpDycrfZz8BC/1xrTDM6T4yn7",
         ==, yu::digest::bcrypt_sha256("rounds=1000$0123456789abcdef", "password"));

  // long salt will be truncate
  EXPECT("$5$rounds=1000$saltlongerthan16$eh0ATKUYyFzAw9UwJEd9lD.sRhTHjjPJ6M73gZYP3gC",
         ==, yu::digest::bcrypt_sha256("rounds=1000$saltlongerthan16bytes", "password"));
  EXPECT("$5$rounds=1000$saltlongerthan16$eh0ATKUYyFzAw9UwJEd9lD.sRhTHjjPJ6M73gZYP3gC",
         ==, yu::digest::bcrypt_sha256("rounds=1000$saltlongerthan16bytesanother", "password"));
}

TEST(BCryptTest, testSha512) {
  EXPECT("$6$rounds=1000$salt$O0nhCLTaysO7YRx7RhswA9LUudDbe8n/29Fg0mCv48MlTSygeX82/gn5a5m7.zIERei.MTm0LX6AbQbQLYGS5.",
         ==, yu::digest::bcrypt_sha512("rounds=1000$salt", "password"));

  // change rounds
  EXPECT("$6$rounds=10000$salt$dE5fLfpn2uXfkz.eouwYK/BjrHRu.piovQPjwlE06fDJHwMlg2l.IqEBUIfWBzf7YPXOAddB3FM7rnXHHKVNt.",
         ==, yu::digest::bcrypt_sha512("rounds=10000$salt", "password"));

  // default rounds
  EXPECT("$6$rounds=5000$salt$IxDD3jeSOb5eB1CX5LBsqZFVkJdido3OUILO5Ifz5iwMuTS4XMS130MTSuDDl3aCI6WouIL9AjRbLCelDCy.g.",
         ==, yu::digest::bcrypt_sha512("rounds=5000$salt", "password"));
  EXPECT("$6$salt$IxDD3jeSOb5eB1CX5LBsqZFVkJdido3OUILO5Ifz5iwMuTS4XMS130MTSuDDl3aCI6WouIL9AjRbLCelDCy.g.",
         ==, yu::digest::bcrypt_sha512("salt", "password"));

  // bin password
  EXPECT("$6$rounds=1000$salt$NqhXojlgP5NLvJojBnjQD87i66jhb8s3bZord3hSZoIgbCJqUfJdp7pclsLBBqgn02fAtd/vn4lieLeX5J.h90",
         ==, yu::digest::bcrypt_sha512("rounds=1000$salt", "pass\x00word"));

  // long password
  EXPECT("$6$rounds=1000$salt$MUqswiU4FlaL.9uLnASWSxD2w9L9uQXbkea.3Bd1QqgNV34QWqlQyyKiTLa6HFzxhADkg2XsYd0Muis7P9ma81",
         ==, yu::digest::bcrypt_sha512("rounds=1000$salt", "long_long_long_long_long_long_long_long_long_long_long_long_password"));

  // long salt
  EXPECT("$6$rounds=1000$0123456789abcdef$NrcnzC1Cw31yiSjRZbYLUDHpNvPA2ZSdS38GKwAQzTm125mJjPzlJsqf1cksiGQ5TF/ub5.IhMMBB8LyJ1ip60",
         ==, yu::digest::bcrypt_sha512("rounds=1000$0123456789abcdef", "password"));

  // long salt will be truncate
  EXPECT("$6$rounds=1000$saltlongerthan16$Sk0JxGSFxo6hoCTGtgrKocYq6e8ZaVuUSKZxlXPQyQwac8PXH4V7aLhv9M7xKgxKhYMRDHVf5j.kRvvb8mDNn0",
         ==, yu::digest::bcrypt_sha512("rounds=1000$saltlongerthan16bytes", "password"));
  EXPECT("$6$rounds=1000$saltlongerthan16$Sk0JxGSFxo6hoCTGtgrKocYq6e8ZaVuUSKZxlXPQyQwac8PXH4V7aLhv9M7xKgxKhYMRDHVf5j.kRvvb8mDNn0",
         ==, yu::digest::bcrypt_sha512("rounds=1000$saltlongerthan16bytesanother", "password"));
}

TEST(BCryptTest, testCheck) {
  std::vector<std::string> passwords = {
    "password", { "pass\0word", 9 }, "Password", "long_long_long_long_long_long_long_long_long_long_long_long_password",
  };
  std::vector<std::string> salts = {
    "salt", "another_salt", "",
  };
  for (const auto& password1 : passwords) {
    for (const auto& password2 : passwords) {
      for (const auto& salt : salts) {
        {
          std::string digest = yu::digest::bcrypt_sha256(salt, password1);
          EXPECT((password1 == password2), ==, yu::digest::bcrypt_check(digest, password2));
        }
        {
          std::string digest = yu::digest::bcrypt_sha512(salt, password1);
          EXPECT((password1 == password2), ==, yu::digest::bcrypt_check(digest, password2));
        }
      }
    }
  }
}
