// Copyright 2025 TAKEI Yuya (https://github.com/takei-yuya/)
#ifndef YU_MATH_RATIONAL_HPP_
#define YU_MATH_RATIONAL_HPP_

#include <type_traits>
#include <cstdint>
#include <iostream>
#include <limits>

#include "yu/math/utils.hpp"

namespace yu {
namespace math {

template <typename Uint>
class RationalBase {
 private:
  template <typename T>
  using enable_if_is_unsigned_integral_t
    = typename std::enable_if<std::is_unsigned<T>::value && std::is_integral<T>::value, T>::type;

  template <typename T>
  using enable_if_is_signed_integral_t
    = typename std::enable_if<std::is_signed<T>::value && std::is_integral<T>::value, T>::type;

 public:
  // Constructors
  constexpr RationalBase() noexcept : positive_(true), numerator_(0), denominator_(1), reduced_(true) {}

  template <typename T>
  explicit RationalBase(T numerator, enable_if_is_unsigned_integral_t<T> denominator = 1) noexcept
      : positive_(true), numerator_(numerator), denominator_(denominator), reduced_(denominator == 1) {
    Normalize();
  }

  template <typename T>
  explicit RationalBase(T numerator, enable_if_is_signed_integral_t<T> denominator = 1) noexcept
      : positive_((numerator > 0) == (denominator > 0))
      , numerator_(static_cast<Uint>(std::abs(numerator)))
      , denominator_(static_cast<Uint>(std::abs(denominator)))
      , reduced_(denominator == 1) {
    Normalize();
  }

  RationalBase(bool positive, Uint numerator, Uint denominator) noexcept
      : positive_(positive), numerator_(numerator), denominator_(denominator), reduced_(denominator == 1) {
    Normalize();
  }

 private:
  // Constructor for already reduced rational numbers
  RationalBase(bool positive, Uint numerator, Uint denominator, bool reduced) noexcept
      : positive_(positive), numerator_(numerator), denominator_(denominator), reduced_(reduced) {
    Normalize();
  }

 public:
  // Named constructors
  static const RationalBase& Zero() noexcept    { static const RationalBase r(true, 0, 1, true); return r; }
  static const RationalBase& NaN() noexcept     { static const RationalBase r(true, 0, 0, true); return r; }
  static const RationalBase& PosInf() noexcept  { static const RationalBase r(true, 1, 0, true); return r; }
  static const RationalBase& NegInf() noexcept  { static const RationalBase r(false, 1, 0, true); return r; }

  // Accessors
  constexpr bool is_positive() const noexcept { return positive_; }
  constexpr Uint numerator() const noexcept { return numerator_; }
  constexpr Uint denominator() const noexcept { return denominator_; }

  constexpr bool is_zero() const noexcept { return numerator_ == 0 && denominator_ == 1; }
  constexpr bool is_valid() const noexcept { return denominator_ != 0; }
  constexpr bool is_nan() const noexcept { return numerator_ == 0 && denominator_ == 0; }
  constexpr bool is_inf() const noexcept { return numerator_ != 0 && denominator_ == 0; }
  constexpr bool is_pos_inf() const noexcept { return is_inf() && positive_; }
  constexpr bool is_neg_inf() const noexcept { return is_inf() && !positive_; }

  // Conversion operators
  explicit operator long double() const noexcept {
    if (is_nan()) return std::numeric_limits<long double>::quiet_NaN();
    if (is_pos_inf()) return std::numeric_limits<long double>::infinity();
    if (is_neg_inf()) return -std::numeric_limits<long double>::infinity();
    return (positive_ ? 1 : -1) * static_cast<long double>(numerator_) / static_cast<long double>(denominator_);
  }
  explicit operator double() const noexcept { return static_cast<double>(static_cast<long double>(*this)); }
  explicit operator float() const noexcept { return static_cast<float>(static_cast<long double>(*this)); }

  // Comparison operators
  bool operator==(const RationalBase& other) const noexcept {
    if (is_nan() || other.is_nan()) return false;
    return positive_ == other.positive_ && numerator_ == other.numerator_ && denominator_ == other.denominator_;
  }
  bool operator!=(const RationalBase& other) const noexcept {
    if (is_nan() || other.is_nan()) return false;
    return !(*this == other);
  }
  bool operator<(const RationalBase& other) const noexcept {
    if (is_nan() || other.is_nan()) return false;
    return !((*this - other).is_positive());
  }
  bool operator>(const RationalBase& other) const noexcept {
    if (is_nan() || other.is_nan()) return false;
    return other < *this;
  }
  bool operator<=(const RationalBase& other) const noexcept {
    if (is_nan() || other.is_nan()) return false;
    return !(*this > other);
  }
  bool operator>=(const RationalBase& other) const noexcept {
    if (is_nan() || other.is_nan()) return false;
    return !(*this < other);
  }

  // strict equality operator ((NaN == NaN) == false, but NaN.equals(NaN) == true)
  constexpr bool equals(const RationalBase& other) const noexcept {
    return positive_ == other.positive_ && numerator_ == other.numerator_ && denominator_ == other.denominator_;
  }

  // Unary Arithmetic operators
  constexpr RationalBase operator+() const noexcept { return *this; }
  constexpr RationalBase operator-() const noexcept {
    return RationalBase(!positive_, numerator_, denominator_, true);
  }
  constexpr RationalBase invert() const noexcept {
    return RationalBase(positive_, denominator_, numerator_, true);
  }

  // Binary Arithmetic operators
  RationalBase operator+(const RationalBase& other) const noexcept {
    if (is_nan() || other.is_nan()) return NaN();  // NaN + x / x + NaN
    if (is_pos_inf() && other.is_neg_inf()) return NaN();  // +Inf + -Inf
    if (is_neg_inf() && other.is_pos_inf()) return NaN();  // -Inf + +Inf
    if (is_pos_inf() || other.is_pos_inf()) return PosInf();  // +Inf + x / x + +Inf
    if (is_neg_inf() || other.is_neg_inf()) return NegInf();  // -Inf + x / x + -Inf

    Uint gcd = yu::math::gcd(denominator_, other.denominator_);
    Uint n1 = numerator_ * (other.denominator_ / gcd);
    Uint n2 = other.numerator_ * (denominator_ / gcd);
    Uint d = denominator_ * (other.denominator_ / gcd);
    if (is_positive() == other.is_positive()) {
      return RationalBase(positive_, n1 + n2, d);
    } else {
      if (n1 > n2) {
        return RationalBase(positive_, n1 - n2, d);
      } else {
        return RationalBase(!positive_, n2 - n1, d);
      }
    }
  }
  RationalBase operator-(const RationalBase& other) const noexcept {
    return *this + (-other);
  }

  RationalBase operator*(const RationalBase& other) const noexcept {
    if (is_nan() || other.is_nan()) return NaN();  // NaN * x / x * NaN
    if (is_inf() && other.is_zero()) return NaN();  // +Inf/-Inf * 0
    if (is_zero() && other.is_inf()) return NaN();  // 0 * +Inf/-Inf
    if (is_inf()) return (other.is_positive() ? +(*this) : -(*this));  // +Inf/-Inf * x
    if (other.is_inf()) return (is_positive() ? +other : -other);  // x * +Inf/-Inf

    Uint gcd1 = yu::math::gcd(numerator_, other.denominator_);
    Uint gcd2 = yu::math::gcd(other.numerator_, denominator_);
    Uint n = (numerator_ / gcd1) * (other.numerator_ / gcd2);
    Uint d = (denominator_ / gcd2) * (other.denominator_ / gcd1);
    return RationalBase(positive_ == other.positive_, n, d, true);
  }
  RationalBase operator/(const RationalBase& other) const noexcept {
    return *this * other.invert();
  }

  // Binary Arithmetic operators with integer (Rational op int)
  template <typename T> RationalBase operator+(T other) const noexcept { return *this + RationalBase(other); }
  template <typename T> RationalBase operator-(T other) const noexcept { return *this - RationalBase(other); }
  template <typename T> RationalBase operator*(T other) const noexcept { return *this * RationalBase(other); }
  template <typename T> RationalBase operator/(T other) const noexcept { return *this / RationalBase(other); }

  // Binary Arithmetic operators with integer (int op Rational)
  template <typename T> friend RationalBase operator+(T other, const RationalBase& r) noexcept { return RationalBase(other) + r; }
  template <typename T> friend RationalBase operator-(T other, const RationalBase& r) noexcept { return RationalBase(other) - r; }
  template <typename T> friend RationalBase operator*(T other, const RationalBase& r) noexcept { return RationalBase(other) * r; }
  template <typename T> friend RationalBase operator/(T other, const RationalBase& r) noexcept { return RationalBase(other) / r; }

  // Binary Arithmetic assignment operators
  RationalBase& operator+=(const RationalBase& other) noexcept { *this = *this + other; return *this; }
  RationalBase& operator-=(const RationalBase& other) noexcept { *this = *this - other; return *this; }
  RationalBase& operator*=(const RationalBase& other) noexcept { *this = *this * other; return *this; }
  RationalBase& operator/=(const RationalBase& other) noexcept { *this = *this / other; return *this; }

  // Binary Arithmetic assignment operators with integer
  template <typename T> RationalBase& operator+=(T other) noexcept { return *this += RationalBase(other); }
  template <typename T> RationalBase& operator-=(T other) noexcept { return *this -= RationalBase(other); }
  template <typename T> RationalBase& operator*=(T other) noexcept { return *this *= RationalBase(other); }
  template <typename T> RationalBase& operator/=(T other) noexcept { return *this /= RationalBase(other); }

 private:
  void Normalize() noexcept {
    if (denominator_ == 0) {
      if (numerator_ == 0) {
        positive_ = true;  // NaN
      } else {
        numerator_ = 1;  // +Inf/-Inf
      }
      return;
    }
    if (numerator_ == 0) { // 0 -> +0/1
      positive_ = true;
      denominator_ = 1;
      return;
    }
    if (reduced_) return;  // already reduced
    Uint gcd = yu::math::gcd(numerator_, denominator_);
    numerator_ /= gcd;
    denominator_ /= gcd;
    reduced_ = true;
  }

  // NaN: +0/0
  // +Inf: +1/0
  // -Inf: -1/0
  bool positive_;
  Uint numerator_;
  Uint denominator_;
  bool reduced_;  // already reduced
};
using Rational = RationalBase<uint64_t>;

template <typename Uint>
std::ostream& operator<<(std::ostream& os, const RationalBase<Uint>& r) {
  if (r.is_nan()) {
    os << "NaN";
  } else if (r.is_pos_inf()) {
    os << "+Inf";
  } else if (r.is_neg_inf()) {
    os << "-Inf";
  } else {
    if (!r.is_positive()) {
      os << "-";
    }
    os << r.numerator();
    if (r.denominator() != 1) {
      os << "/" << r.denominator();
    }
  }
  return os;
}

}  // namespace math
}  // namespace yu

#endif  // YU_MATH_RATIONAL_HPP_
