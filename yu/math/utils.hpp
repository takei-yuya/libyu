// Copyright 2025 TAKEI Yuya (https://github.com/takei-yuya/)
#ifndef YU_MATH_UTILS_HPP_
#define YU_MATH_UTILS_HPP_

#include <cstdlib>

namespace yu {
namespace math {

#if __cpp_lib_gcd_lcm >= 201606L
#include <numeric>
using std::gcd;
using std::lcm;

#else
template <typename T>
auto gcd(T lhs, T rhs) -> typename std::enable_if<std::is_integral<T>::value && std::is_signed<T>::value, T>::type {
  lhs = std::abs(lhs);
  rhs = std::abs(rhs);
  while (lhs != 0 && rhs != 0) {
    if (lhs > rhs) {
      lhs %= rhs;
    } else {
      rhs %= lhs;
    }
  }
  return lhs + rhs;
}

template <typename T>
auto gcd(T lhs, T rhs) -> typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value, T>::type {
  while (lhs != 0 && rhs != 0) {
    if (lhs > rhs) {
      lhs %= rhs;
    } else {
      rhs %= lhs;
    }
  }
  return lhs + rhs;
}

template <typename T>
auto lcm(T lhs, T rhs) -> typename std::enable_if<std::is_integral<T>::value && std::is_signed<T>::value, T>::type {
  if (lhs == 0 || rhs == 0) return 0;
  return std::abs(lhs) / gcd(lhs, rhs) * std::abs(rhs);
}

template <typename T>
auto lcm(T lhs, T rhs) -> typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value, T>::type {
  if (lhs == 0 || rhs == 0) return 0;
  return lhs / gcd(lhs, rhs) * rhs;
}
#endif

}  // namespace math
}  // namespace yu

#endif  // YU_MATH_UTILS_HPP_
