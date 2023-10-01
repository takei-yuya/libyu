// Copyright 2023 TAKEI Yuya (https://github.com/takei-yuya/)
#ifndef YU_LEXICAL_CAST_HPP_
#define YU_LEXICAL_CAST_HPP_

#include <sstream>

namespace yu {
namespace lang {

template <typename T1, typename T2>
auto lexical_cast(const T2& val) -> typename std::enable_if<std::is_same<T1, T2>::value, T1>::type {
  return val;
}

template <typename T1, typename T2>
auto lexical_cast(const T2& val) -> typename std::enable_if<!std::is_same<T1, T2>::value, T1>::type {
  T1 result {};
  std::ostringstream oss;
  oss << val;
  std::istringstream iss(oss.str());
  iss >> result;
  return result;
}

template<typename T>
T lexical_cast(const std::string& val) {
  T result {};
  std::istringstream iss(val);
  iss >> result;
  return result;
}

}  // namespace lang
}  // namespace yu

#endif  // YU_LEXICAL_CAST_HPP_
