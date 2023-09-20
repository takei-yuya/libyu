#ifndef YU_DIGEST_SHA2_HPP_
#define YU_DIGEST_SHA2_HPP_

#include <cstdint>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>
#include <iomanip>
#include <bitset>

#include "yu/lang/endian.hpp"

namespace yu {
namespace digest {

inline uint32_t rotr(uint32_t n, int s) {
  return (n >> s) | (n << (32 - s));
}

class sha256_streambuf : public std::streambuf {
 public:
  sha256_streambuf() : buffer_(64), message_size_(0), finished_(false) {
    initialize();
    setp(buffer_.data(), buffer_.data() + buffer_.size());
  }

  virtual std::string hash() {
    finish();
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (uint32_t h : hs_) {
      oss << std::setw(8) << h;
    }
    return oss.str();
  }

 protected:
  virtual void initialize() {
    hs_ = { 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19, };
  }

 private:
  void finish() {
    if (finished_) return;
    finished_ = true;

    std::streamsize n = pptr() - pbase();
    uint64_t message_size = (message_size_ + n) * 8;

    overflow(0b10000000);
    if (64 - n < 9) {
      // no space for message length
      for (int i = 0; i < 64 - n; ++i) {
        overflow(0b00000000);
      }
    }
    memset(pptr(), 0, epptr() - pptr());
    buffer_[56] = (message_size >> 56) & 0b11111111;
    buffer_[57] = (message_size >> 48) & 0b11111111;
    buffer_[58] = (message_size >> 40) & 0b11111111;
    buffer_[59] = (message_size >> 32) & 0b11111111;
    buffer_[60] = (message_size >> 24) & 0b11111111;
    buffer_[61] = (message_size >> 16) & 0b11111111;
    buffer_[62] = (message_size >>  8) & 0b11111111;
    buffer_[63] = (message_size >>  0) & 0b11111111;
    process();
  }

  virtual int overflow(int ch = traits_type::eof()) {
    std::streamsize n = pptr() - pbase();
    if (n != 64) {
      *pptr() = ch;
      pbump(1);
      return ch;
    }

    process();

    pbump(-n);
    message_size_ += n;

    if (ch != traits_type::eof()) {
      *pbase() = ch;
      pbump(1);
    }
    return ch;
  };

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

  void process() {
    std::vector<uint32_t> w(64);
    for (int i = 0; i < 16; ++i) {
      w[i]
        = ((buffer_[i*4+0] << 24) & 0b11111111000000000000000000000000)
        | ((buffer_[i*4+1] << 16) & 0b00000000111111110000000000000000)
        | ((buffer_[i*4+2] <<  8) & 0b00000000000000001111111100000000)
        | ((buffer_[i*4+3] <<  0) & 0b00000000000000000000000011111111);
    }
    for (int i = 16; i < 64; ++i) {
      w[i] = w[i-16] + sigma0(w[i-15]) + w[i-7] + sigma1(w[i-2]);
    }

    uint32_t a = hs_[0];
    uint32_t b = hs_[1];
    uint32_t c = hs_[2];
    uint32_t d = hs_[3];
    uint32_t e = hs_[4];
    uint32_t f = hs_[5];
    uint32_t g = hs_[6];
    uint32_t h = hs_[7];

    for (int i = 0; i < 64; ++i) {
      uint32_t temp1 = h + Sigma1(e) + Ch(e, f, g) + k[i] + w[i];
      uint32_t temp2 = Sigma0(a) + Maj(a, b, c);
      h = g;
      g = f;
      f = e;
      e = d + temp1;
      d = c;
      c = b;
      b = a;
      a = temp1 + temp2;
    }

    hs_[0] += a;
    hs_[1] += b;
    hs_[2] += c;
    hs_[3] += d;
    hs_[4] += e;
    hs_[5] += f;
    hs_[6] += g;
    hs_[7] += h;
  }


 protected:
  std::vector<uint32_t> hs_;

 private:
  std::vector<char> buffer_;  // 8 * 64 = 512 bit chunk
  uint64_t message_size_;
  bool finished_;

  const uint32_t k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
  };
};

class sha256_stream : public std::ostream {
 public:
  sha256_stream() : std::ostream(&buf_), buf_() {}
  ~sha256_stream() {}
  std::string hash() { return buf_.hash(); }
 private:
  sha256_streambuf buf_;
};

std::string sha256(const std::string& str) {
  sha256_stream sha256s;
  sha256s.write(str.c_str(), str.size());
  return sha256s.hash();
}

class sha224_streambuf : public sha256_streambuf {
 public:
  sha224_streambuf() {
    std::cout << "sha224_streambuf::sha224_streambuf" << std::endl;
    sha256_streambuf();
    initialize();
  }

  virtual std::string hash() {
    return sha256_streambuf::hash().substr(0, 56);
  }

 private:
  virtual void initialize() {
    std::cout << "sha224::initialize" << std::endl;
    hs_ = { 0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939, 0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4 };
  }
};

class sha224_stream : public std::ostream {
 public:
  sha224_stream() : std::ostream(&buf_), buf_() {}
  ~sha224_stream() {}
  std::string hash() { return buf_.hash(); }
 private:
  sha224_streambuf buf_;
};

std::string sha224(const std::string& str) {
  sha224_stream sha224s;
  sha224s.write(str.c_str(), str.size());
  return sha224s.hash();
}

}  // namespace digest
}  // namespace yu

#endif  // YU_DIGEST_SHA2_HPP_
