#ifndef YU_DIGEST_SHA2_HPP_
#define YU_DIGEST_SHA2_HPP_

#include <cstdint>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>
#include <iomanip>
#include <limits>
#include <climits>

namespace yu {
namespace digest {

// TODO: move to numric_util? or something else
inline uint32_t rotr(uint32_t n, int s) {
  return (n >> s) | (n << (std::numeric_limits<uint32_t>::digits - s));
}

// TODO: move to system? or lang? endian_utils?
// buf must have 4 byte capacity
inline char* write_as_bigendian(uint32_t n, char* buf) {
  *(buf + 0) = ((n >> 24) & 0b11111111);
  *(buf + 1) = ((n >> 16) & 0b11111111);
  *(buf + 2) = ((n >>  8) & 0b11111111);
  *(buf + 3) = ((n >>  0) & 0b11111111);
  return buf + 4;
}
// buf must have 8 byte capacity
inline char* write_as_bigendian(uint64_t n, char* buf) {
  *(buf + 0) = ((n >> 56) & 0b11111111);
  *(buf + 1) = ((n >> 48) & 0b11111111);
  *(buf + 2) = ((n >> 40) & 0b11111111);
  *(buf + 3) = ((n >> 32) & 0b11111111);
  *(buf + 4) = ((n >> 24) & 0b11111111);
  *(buf + 5) = ((n >> 16) & 0b11111111);
  *(buf + 6) = ((n >>  8) & 0b11111111);
  *(buf + 7) = ((n >>  0) & 0b11111111);
  return buf + 8;
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
static const std::initializer_list<uint32_t> kSha256InitStatus = { 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };
static const std::initializer_list<uint32_t> kSha224InitStatus = { 0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939, 0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4 };

class sha2_base_streambuf : public std::streambuf {
 public:
  static const int kChunkSize = 512;  // bit
  static const int kRound = 64;

  explicit sha2_base_streambuf(std::initializer_list<uint32_t> init_status)
    : hs_(init_status), buffer_(kChunkSize / CHAR_BIT), message_size_(0), finished_(false) {
    setp(buffer_.data(), buffer_.data() + buffer_.size());
  }

  virtual std::string hash() = 0;

 protected:
  void finish() {
    if (finished_) return;
    finished_ = true;

    std::streamsize n = pptr() - pbase();
    uint64_t message_size = message_size_ + n * CHAR_BIT;
    std::streamsize free_byte_in_chunk = kChunkSize / CHAR_BIT - n;

    overflow(0b10000000);
    if (free_byte_in_chunk < 9) {
      // no space for message length
      for (int i = 0; i < free_byte_in_chunk; ++i) {  // ensure process current chunk, +1 for loop
        overflow(0b00000000);
      }
    }
    memset(pptr(), 0, epptr() - pptr());
    write_as_bigendian(message_size, &buffer_[kChunkSize / CHAR_BIT - sizeof(uint64_t)]);
    process();
  }

 private:
  virtual int overflow(int ch = traits_type::eof()) {
    std::streamsize n = pptr() - pbase();
    if (pptr() < epptr()) {
      *pptr() = ch;
      pbump(1);
      return ch;
    }

    process();

    pbump(-n);
    message_size_ += n * CHAR_BIT;

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
    std::vector<uint32_t> w(kRound);
    for (int i = 0; i < 16; ++i) {
      read_as_bigendian(w[i], &buffer_[i*4]);
    }
    for (int i = 16; i < kRound; ++i) {
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

    for (int i = 0; i < kRound; ++i) {
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

  const uint32_t k[kRound] = {
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

class sha256_streambuf : public sha2_base_streambuf {
 public:
  sha256_streambuf() : sha2_base_streambuf(kSha256InitStatus) {}

  virtual std::string hash() {
    finish();
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (uint32_t h : hs_) {
      oss << std::setw(std::numeric_limits<uint32_t>::digits / 4) << h;
    }
    return oss.str();
  }
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

class sha224_streambuf : public sha2_base_streambuf {
 public:
  sha224_streambuf() : sha2_base_streambuf(kSha224InitStatus) {}

  virtual std::string hash() {
    finish();
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (size_t i = 0; i < hs_.size() - 1; ++i) {  // drop h7
      oss << std::setw(std::numeric_limits<uint32_t>::digits / 4) << hs_[i];
    }
    return oss.str();
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
