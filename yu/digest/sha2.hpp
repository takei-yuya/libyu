// Copyright 2023 TAKEI Yuya (https://github.com/takei-yuya/)
#ifndef YU_DIGEST_SHA2_HPP_
#define YU_DIGEST_SHA2_HPP_

#include <climits>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>

#include "yu/base64.hpp"

namespace yu {
namespace digest {

// TODO: move to numric_util? or something else
template <typename T>
inline T rotr(T n, int s) {
  return (n >> s) | (n << (std::numeric_limits<T>::digits - s));
}

// TODO: move to numric_util? or something else
typedef union u128 {
  u128() : u64s() {}
  char bytes[16];
  struct u64s {
    u64s() : hi(0), lo(0) {}
    uint64_t hi;
    uint64_t lo;
  } u64s;
} u128;
static_assert(sizeof(u128) == 16, "sizeof(u128) == 16");

template <typename T>
u128& operator+=(u128& lhs, T rhs) {
  if (std::numeric_limits<uint64_t>::max() - lhs.u64s.lo < static_cast<uint64_t>(rhs)) {
    ++lhs.u64s.hi;
  }
  lhs.u64s.lo += rhs;
  return lhs;
}

template <typename T>
u128 operator+(const u128& lhs, T rhs) {
  u128 result(lhs);
  result += rhs;
  return result;
}
// TODO: add more operators

// TODO: move to system? or lang? endian_utils?
// buf must have 4 byte capacity
inline char* write_as_bigendian(uint32_t n, char* buf) {
  *(buf + 0) = static_cast<char>((n >> 24) & 0b11111111);
  *(buf + 1) = static_cast<char>((n >> 16) & 0b11111111);
  *(buf + 2) = static_cast<char>((n >>  8) & 0b11111111);
  *(buf + 3) = static_cast<char>((n >>  0) & 0b11111111);
  return buf + 4;
}

// buf must have 8 byte capacity
inline char* write_as_bigendian(uint64_t n, char* buf) {
  *(buf + 0) = static_cast<char>((n >> 56) & 0b11111111);
  *(buf + 1) = static_cast<char>((n >> 48) & 0b11111111);
  *(buf + 2) = static_cast<char>((n >> 40) & 0b11111111);
  *(buf + 3) = static_cast<char>((n >> 32) & 0b11111111);
  *(buf + 4) = static_cast<char>((n >> 24) & 0b11111111);
  *(buf + 5) = static_cast<char>((n >> 16) & 0b11111111);
  *(buf + 6) = static_cast<char>((n >>  8) & 0b11111111);
  *(buf + 7) = static_cast<char>((n >>  0) & 0b11111111);
  return buf + 8;
}

// buf must have 16 byte capacity
inline char* write_as_bigendian(const u128 n, char* buf) {
  buf = write_as_bigendian(n.u64s.hi, buf);
  buf = write_as_bigendian(n.u64s.lo, buf);
  return buf;
}

// buf must have 4 byte data
inline const char* read_as_bigendian(uint32_t& n, const char* buf) {
  n =
    ((static_cast<uint32_t>(*(buf + 0)) << 24) & 0b11111111000000000000000000000000) |
    ((static_cast<uint32_t>(*(buf + 1)) << 16) & 0b00000000111111110000000000000000) |
    ((static_cast<uint32_t>(*(buf + 2)) <<  8) & 0b00000000000000001111111100000000) |
    ((static_cast<uint32_t>(*(buf + 3)) <<  0) & 0b00000000000000000000000011111111);
  return buf + 4;
}

// buf must have 8 byte data
inline const char* read_as_bigendian(uint64_t& n, const char* buf) {
  n =
    ((static_cast<uint64_t>(*(buf + 0)) << 56) & 0b1111111100000000000000000000000000000000000000000000000000000000) |
    ((static_cast<uint64_t>(*(buf + 1)) << 48) & 0b0000000011111111000000000000000000000000000000000000000000000000) |
    ((static_cast<uint64_t>(*(buf + 2)) << 40) & 0b0000000000000000111111110000000000000000000000000000000000000000) |
    ((static_cast<uint64_t>(*(buf + 3)) << 32) & 0b0000000000000000000000001111111100000000000000000000000000000000) |
    ((static_cast<uint64_t>(*(buf + 4)) << 24) & 0b0000000000000000000000000000000011111111000000000000000000000000) |
    ((static_cast<uint64_t>(*(buf + 5)) << 16) & 0b0000000000000000000000000000000000000000111111110000000000000000) |
    ((static_cast<uint64_t>(*(buf + 6)) <<  8) & 0b0000000000000000000000000000000000000000000000001111111100000000) |
    ((static_cast<uint64_t>(*(buf + 7)) <<  0) & 0b0000000000000000000000000000000000000000000000000000000011111111);
  return buf + 8;
}

namespace detail {
inline static uint32_t Ch(uint32_t e, uint32_t f, uint32_t g) {
  return (e & f) ^ ((~e) & g);
}

inline static uint32_t Maj(uint32_t a, uint32_t b, uint32_t c) {
  return (a & b) ^ (a & c) ^ (b & c);
}

inline static uint32_t Sigma0(uint32_t a) {
  return rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
}

inline static uint32_t Sigma1(uint32_t e) {
  return rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
}

inline static uint32_t sigma0(uint32_t w) {
  return rotr(w, 7) ^ rotr(w, 18) ^ (w >> 3);
}

inline static uint32_t sigma1(uint32_t w) {
  return rotr(w, 17) ^ rotr(w, 19) ^ (w >> 10);
}

static const std::initializer_list<uint32_t> kSha256InitStatus = {
  0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};
static const std::initializer_list<uint32_t> kSha224InitStatus = {
  0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939, 0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4
};
static const std::initializer_list<uint32_t> kSha256RoundConstants = {
  0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
  0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
  0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
  0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
  0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
  0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
  0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
  0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

inline static uint64_t Ch(uint64_t e, uint64_t f, uint64_t g) {
  return (e & f) ^ ((~e) & g);
}

inline static uint64_t Maj(uint64_t a, uint64_t b, uint64_t c) {
  return (a & b) ^ (a & c) ^ (b & c);
}

inline static uint64_t Sigma0(uint64_t a) {
  return rotr(a, 28) ^ rotr(a, 34) ^ rotr(a, 39);
}

inline static uint64_t Sigma1(uint64_t e) {
  return rotr(e, 14) ^ rotr(e, 18) ^ rotr(e, 41);
}

inline static uint64_t sigma0(uint64_t w) {
  return rotr(w, 1) ^ rotr(w, 8) ^ (w >> 7);
}

inline static uint64_t sigma1(uint64_t w) {
  return rotr(w, 19) ^ rotr(w, 61) ^ (w >> 6);
}

static const std::initializer_list<uint64_t> kSha512InitStatus = {
  0x6a09e667f3bcc908, 0xbb67ae8584caa73b, 0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,
  0x510e527fade682d1, 0x9b05688c2b3e6c1f, 0x1f83d9abfb41bd6b, 0x5be0cd19137e2179
};
static const std::initializer_list<uint64_t> kSha384InitStatus = {
  0xcbbb9d5dc1059ed8, 0x629a292a367cd507, 0x9159015a3070dd17, 0x152fecd8f70e5939,
  0x67332667ffc00b31, 0x8eb44a8768581511, 0xdb0c2e0d64f98fa7, 0x47b5481dbefa4fa4
};
static const std::initializer_list<uint64_t> kSha512_224InitStatus = {
  0x8C3D37C819544DA2, 0x73E1996689DCD4D6, 0x1DFAB7AE32FF9C82, 0x679DD514582F9FCF,
  0x0F6D2B697BD44DA8, 0x77E36F7304C48942, 0x3F9D85A86A1D36C8, 0x1112E6AD91D692A1
};
static const std::initializer_list<uint64_t> kSha512_256InitStatus = {
  0x22312194FC2BF72C, 0x9F555FA3C84C64C2, 0x2393B86B6F53B151, 0x963877195940EABD,
  0x96283EE2A88EFFE3, 0xBE5E1E2553863992, 0x2B0199FC2C85B8AA, 0x0EB72DDC81C52CA2
};
static const std::initializer_list<uint64_t> kSha512RoundConstants = {
  0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc,
  0x3956c25bf348b538, 0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118,
  0xd807aa98a3030242, 0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
  0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 0xc19bf174cf692694,
  0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
  0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
  0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4,
  0xc6e00bf33da88fc2, 0xd5a79147930aa725, 0x06ca6351e003826f, 0x142929670a0e6e70,
  0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
  0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
  0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30,
  0xd192e819d6ef5218, 0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8,
  0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8,
  0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3,
  0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
  0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b,
  0xca273eceea26619c, 0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178,
  0x06f067aa72176fba, 0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b,
  0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c,
  0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817
};


template <typename word_type, typename message_length_type, size_t kHashBit, size_t kChunkSizeInBit, size_t kRound>
class sha2_base_streambuf : public std::streambuf {
 public:
  const static size_t kChunkSize = kChunkSizeInBit / 8;

  sha2_base_streambuf(std::initializer_list<word_type> init_status, std::initializer_list<word_type> round_constants)
    : hs_(init_status), original_hs_(hs_), k_(round_constants), buffer_(kChunkSize), message_size_(), finished_(false) {
    setp(buffer_.data(), buffer_.data() + buffer_.size());
  }

  std::string hash_hex() {
    finish();
    int width = std::numeric_limits<word_type>::digits / 4;
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (word_type h : hs_) {
      oss << std::setw(width) << h;
    }
    return oss.str().substr(0, kHashBit / 4);
  }

  std::string hash_bin() {
    finish();
    std::vector<char> result(sizeof(word_type) * hs_.size());
    for (size_t i = 0; i < hs_.size(); ++i) {
      write_as_bigendian(hs_[i], &result[i * sizeof(word_type)]);
    }
    result.resize(kHashBit / CHAR_BIT);
    return std::string(result.data(), result.data() + result.size());
  }

  std::string hash_base64() {
    return yu::base64::encode(hash_bin());
  }

  void reset() {
    hs_ = original_hs_;
    setp(buffer_.data(), buffer_.data() + buffer_.size());
    message_size_ = {};
    finished_ = false;
  }

 private:
  void finish() {
    if (finished_) return;

    size_t n = static_cast<size_t>(pptr() - pbase());
    message_length_type message_size = message_size_ + static_cast<size_t>(n * CHAR_BIT);
    size_t free_byte_in_chunk = kChunkSize - n;

    // tarminal '1' bit and paddings
    overflow(0b10000000);
    if (free_byte_in_chunk < static_cast<ssize_t>(1 + sizeof(message_length_type))) {
      // no space for message length, process current chunk and use next chunk to store message length
      for (size_t i = 0; i < free_byte_in_chunk; ++i) {  // ensure process current chunk, +1 for loop
        overflow(0b00000000);
      }
    }
    memset(pptr(), 0, static_cast<size_t>(epptr() - pptr()));
    write_as_bigendian(message_size, &buffer_[kChunkSize - sizeof(message_length_type)]);
    process();
    finished_ = true;
  }

  int overflow(int ch = traits_type::eof()) override {
    if (finished_) return traits_type::eof();

    if (pptr() < epptr()) {
      *pptr() = static_cast<char>(ch);
      pbump(1);
      // FIXME: invoke process when pptr() == epptr()?
      return traits_type::not_eof(ch);
    }

    process();

    size_t n = static_cast<size_t>(pptr() - pbase());
    pbump(static_cast<int>(-n));

    if (ch != traits_type::eof()) {
      *pbase() = static_cast<char>(ch);
      pbump(1);
    }
    return traits_type::not_eof(ch);
  };

  void process() {
    std::vector<word_type> w(kRound);
    for (size_t i = 0; i < 16; ++i) {
      read_as_bigendian(w[i], &buffer_[i * sizeof(word_type)]);
    }
    for (size_t i = 16; i < kRound; ++i) {
      w[i] = w[i-16] + sigma0(w[i-15]) + w[i-7] + sigma1(w[i-2]);
    }
    message_size_ += kChunkSizeInBit;

    word_type a = hs_[0], b = hs_[1], c = hs_[2], d = hs_[3], e = hs_[4], f = hs_[5], g = hs_[6], h = hs_[7];
    for (size_t i = 0; i < kRound; ++i) {
      word_type temp1 = h + Sigma1(e) + Ch(e, f, g) + k_[i] + w[i];
      word_type temp2 = Sigma0(a) + Maj(a, b, c);
      h = g;
      g = f;
      f = e;
      e = d + temp1;
      d = c;
      c = b;
      b = a;
      a = temp1 + temp2;
    }
    hs_[0] += a; hs_[1] += b; hs_[2] += c; hs_[3] += d; hs_[4] += e; hs_[5] += f; hs_[6] += g; hs_[7] += h;
  }

 private:
  std::vector<word_type> hs_;
  const std::vector<word_type> original_hs_;
  const std::vector<word_type> k_;
  std::vector<char> buffer_;
  message_length_type message_size_;
  bool finished_;
};

class sha256_streambuf : public sha2_base_streambuf<uint32_t, uint64_t, 256, 512, 64> {
 public:
  sha256_streambuf() : sha2_base_streambuf(kSha256InitStatus, kSha256RoundConstants) {}
};

class sha224_streambuf : public sha2_base_streambuf<uint32_t, uint64_t, 224, 512, 64> {
 public:
  sha224_streambuf() : sha2_base_streambuf(kSha224InitStatus, kSha256RoundConstants) {}
};

class sha512_streambuf : public sha2_base_streambuf<uint64_t, u128, 512, 1024, 80> {
 public:
  sha512_streambuf() : sha2_base_streambuf(kSha512InitStatus, kSha512RoundConstants) {}
};

class sha384_streambuf : public sha2_base_streambuf<uint64_t, u128, 384, 1024, 80> {
 public:
  sha384_streambuf() : sha2_base_streambuf(kSha384InitStatus, kSha512RoundConstants) {}
};

class sha512_224_streambuf : public sha2_base_streambuf<uint64_t, u128, 224, 1024, 80> {
 public:
  sha512_224_streambuf() : sha2_base_streambuf(kSha512_224InitStatus, kSha512RoundConstants) {}
};

class sha512_256_streambuf : public sha2_base_streambuf<uint64_t, u128, 256, 1024, 80> {
 public:
  sha512_256_streambuf() : sha2_base_streambuf(kSha512_256InitStatus, kSha512RoundConstants) {}
};
}  // namespace detail

#define DEFINE_SHA_INTERFACES(bit) \
  class sha##bit##_stream : public std::ostream { \
   public: \
    sha##bit##_stream() : std::ostream(&buf_), buf_() {} \
    std::string hash_hex() { return buf_.hash_hex(); } \
    std::string hash_bin() { return buf_.hash_bin(); } \
    std::string hash_base64() { return buf_.hash_base64(); } \
    void reset() { buf_.reset(); } \
   private: \
    detail::sha##bit##_streambuf buf_; \
  }; \
  std::string sha##bit##_hex(const std::string& str)    { sha##bit##_stream s; s.write(str.data(), static_cast<std::streamsize>(str.size())); return s.hash_hex(); } \
  std::string sha##bit##_bin(const std::string& str)    { sha##bit##_stream s; s.write(str.data(), static_cast<std::streamsize>(str.size())); return s.hash_bin(); } \
  std::string sha##bit##_base64(const std::string& str) { sha##bit##_stream s; s.write(str.data(), static_cast<std::streamsize>(str.size())); return s.hash_base64(); }

DEFINE_SHA_INTERFACES(256)
DEFINE_SHA_INTERFACES(224)
DEFINE_SHA_INTERFACES(512)
DEFINE_SHA_INTERFACES(384)
DEFINE_SHA_INTERFACES(512_224)
DEFINE_SHA_INTERFACES(512_256)

#undef DEFINE_SHA_INTERFACES

}  // namespace digest
}  // namespace yu

#endif  // YU_DIGEST_SHA2_HPP_
