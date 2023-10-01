// Copyright 2023 TAKEI Yuya (https://github.com/takei-yuya/)
#ifndef YU_DIGEST_BCRYPT_HPP_
#define YU_DIGEST_BCRYPT_HPP_

#include "yu/digest/sha2.hpp"
#include "yu/lang/lexical_cast.hpp"
#include "yu/string/utils.hpp"

#include <stdexcept>
#include <iostream>
#include <cassert>
#include <random>

namespace yu {
namespace digest {

namespace detail {
static const std::vector<char> kAlphabets = {
  '.', '/',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
  'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
};

const static std::initializer_list<size_t> kBCryptBase64Indexes32 = {
  20, 10,  0,
  11,  1, 21,
   2, 22, 12,
  23, 13,  3,
  14,  4, 24,
   5, 25, 15,
  26, 16,  6,
  17,  7, 27,
   8, 28, 18,
  29, 19,  9,
  30, 31,
};
const static std::initializer_list<size_t> kBCryptBase64Indexes64 = {
  42, 21,  0,
  1, 43, 22,
  23,  2, 44,
  45, 24,  3,
  4, 46, 25,
  26,  5, 47,
  48, 27,  6,
  7, 49, 28,
  29,  8, 50,
  51, 30,  9,
  10, 52, 31,
  32, 11, 53,
  54, 33, 12,
  13, 55, 34,
  35, 14, 56,
  57, 36, 15,
  16, 58, 37,
  38, 17, 59,
  60, 39, 18,
  19, 61, 40,
  41, 20, 62,
  63
};

std::string bcrypt_base64(const std::string& bin) {
  std::vector<size_t> indexes;
  if (bin.size() == 32) indexes.assign(kBCryptBase64Indexes32);
  else if (bin.size() == 64) indexes.assign(kBCryptBase64Indexes64);
  else assert(false);

  std::string result;
  char buf = 0;
  for (size_t i = 0; i < bin.size(); ++i) {
    char ch = bin[indexes[i]];
    if ((i % 3) == 0) {
      result += kAlphabets[      ((ch << 0) & 0b00111111)];
      buf = (ch >> 6) & 0b00000011;
    } else if ((i % 3) == 1) {
      result += kAlphabets[static_cast<size_t>(buf | ((ch << 2) & 0b00111100))];
      buf = (ch >> 4) & 0b00001111;
    } else {
      result += kAlphabets[static_cast<size_t>(buf | ((ch << 4) & 0b00110000))];
      result += kAlphabets[      ((ch >> 2) & 0b00111111)];
      buf = 0;
    }
  }
  result += kAlphabets[static_cast<size_t>(buf)];
  return result;
}

std::string random_salt(size_t len) {
  std::random_device seed_gen;
  std::default_random_engine engine(seed_gen());
  std::uniform_int_distribution<size_t> dist(0, kAlphabets.size() - 1);

  std::string salt;
  for (size_t i = 0; i < len; ++i) {
    salt += kAlphabets[dist(engine)];
  }
  return salt;
}

template <class SHA, int kBufSize>
std::string bcrypt_sha(const std::string& magic, const std::string& input_salt, const std::string& password) {
  const static size_t kMaxSaltSize = 16;
  const static std::string kRoundsPrefix = "rounds=";
  const static size_t kDefaultRound = 5000;
  const static size_t kMinRound = 1000;
  const static size_t kMaxRound = 999999999;

  std::string salt = input_salt.substr(0, kMaxSaltSize);
  size_t round = kDefaultRound;
  bool custom_round = false;

  if (yu::string::starts_with(input_salt, kRoundsPrefix)) {
    size_t p = input_salt.find_first_not_of("0123456789", kRoundsPrefix.size());
    if (p == std::string::npos) throw std::runtime_error("invalid salt");
    if (input_salt[p] != '$') throw std::runtime_error("invalid salt");

    round = yu::lang::lexical_cast<size_t>(input_salt.substr(kRoundsPrefix.size(), p - kRoundsPrefix.size()));
    if (round < kMinRound) round = kMinRound;
    else if (round > kMaxRound) round = kMaxRound;
    custom_round = true;

    salt = input_salt.substr(p + 1, kMaxSaltSize);
  }
  if (salt.empty()) {
    salt = random_salt(16);
  }

  SHA sha;
  sha << password << salt << password;
  std::string digest_b = sha.hash_bin();

  // pre-digest
  std::string buf = password + salt;
  for (size_t i = password.size(); i > kBufSize; i -= kBufSize) buf += digest_b;
  buf += digest_b.substr(0, password.size() % kBufSize);
  for (size_t n = password.size(); n > 0; n >>= 1) {
    if (n % 2 == 0) buf += password;
    else buf += digest_b;
  }
  sha.reset();
  sha << buf;
  std::string x = sha.hash_bin();

  // P
  std::string p;
  {
    sha.reset();
    for (size_t i = 0; i < password.size(); ++i) {
      sha << password;
    }
    std::string hash = sha.hash_bin();
    while (p.size() < password.size()) p += hash;
    p = p.substr(0, password.size());
  }

  // S
  std::string s;
  {
    sha.reset();
    for (size_t i = 0; i < static_cast<size_t>(16 + static_cast<unsigned char>(x[0])); ++i) {
      sha << salt;
    }
    std::string hash = sha.hash_bin();
    while (s.size() < salt.size()) s += hash;
    s = s.substr(0, salt.size());
  }

  // main round
  std::string round_buf;
  round_buf.reserve(x.size() + s.size() + p.size() + p.size() + 1);
  for (size_t i = 0; i < round; ++i) {
    sha.reset();
    if (i % 2 == 0) {
      sha << x;
    } else {
      sha << p;
    }
    if (i % 3) sha << s;
    if (i % 7) sha << p;

    if (i % 2 == 0) {
      sha << p;
    } else {
      sha << x;
    }
    x = sha.hash_bin();
  }

  std::ostringstream oss;
  if (custom_round) {
    oss << "$" << magic << "$" << kRoundsPrefix << round << "$" << salt << "$" << detail::bcrypt_base64(x);
  } else {
    oss << "$" << magic << "$" << salt << "$" << detail::bcrypt_base64(x);
  }

  return oss.str();
}
}  // namespace detail

std::string bcrypt_sha256(const std::string& salt, const std::string& password) {
  return detail::bcrypt_sha<yu::digest::sha256_stream, 32>("5", salt, password);
}

std::string bcrypt_sha512(const std::string& salt, const std::string& password) {
  return detail::bcrypt_sha<yu::digest::sha512_stream, 64>("6", salt, password);
}

bool bcrypt_check(const std::string& digest, const std::string& password) {
  const static std::string kSha256Prefix = "$5$";
  const static std::string kSha512Prefix = "$6$";
  const static std::string kRoundsPrefix = "rounds=";

  if (yu::string::starts_with(digest, kSha256Prefix)) {
    std::string::size_type p = digest.rfind('$');
    std::string salt = digest.substr(kSha256Prefix.size(), p - kSha256Prefix.size());
    return bcrypt_sha256(salt, password) == digest;
  }
  if (yu::string::starts_with(digest, kSha512Prefix)) {
    std::string::size_type p = digest.rfind('$');
    std::string salt = digest.substr(kSha512Prefix.size(), p - kSha512Prefix.size());
    return bcrypt_sha512(salt, password) == digest;
  }
  throw std::runtime_error("Not impl");
}

}  // namespace digest
}  // namespace yu

#endif  // YU_DIGEST_BCRYPT_HPP_
