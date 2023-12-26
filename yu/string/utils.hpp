// Copyright 2023 TAKEI Yuya (https://github.com/takei-yuya/)
#ifndef YU_STRING_UTILS_HPP_
#define YU_STRING_UTILS_HPP_

#include <string>
#include <vector>
#include <sstream>

namespace yu {
namespace string {

inline std::string lstrip(const std::string& str, const std::string& spaces = "\r\n\t ") {
  size_t l = str.find_first_not_of(spaces);
  if (l == std::string::npos) return "";
  return str.substr(l);
}

inline std::string rstrip(const std::string& str, const std::string& spaces = "\r\n\t ") {
  size_t r = str.find_last_not_of(spaces);
  if (r == std::string::npos) return "";
  return str.substr(0, r + 1);
}

inline std::string strip(const std::string& str, const std::string& spaces = "\r\n\t ") {
  size_t l = str.find_first_not_of(spaces);
  if (l == std::string::npos) return "";
  size_t r = str.find_last_not_of(spaces);
  // r should not be npos
  return str.substr(l, r - l + 1);
}

inline std::vector<std::string> split(const std::string& str, char delim = ',', bool strip_token = false, size_t max_tokens = 0) {
  std::vector<std::string> result;
  if (max_tokens == 0) max_tokens = str.size();
  size_t prev = 0;
  for (size_t i = str.find(delim); i < str.size() && result.size() < max_tokens - 1; i = str.find(delim, prev)) {
    if (strip_token) {
      result.push_back(strip(str.substr(prev, i - prev)));
    } else {
      result.push_back(str.substr(prev, i - prev));
    }
    prev = i + 1;
  }
  if (strip_token) {
    result.push_back(strip(str.substr(prev)));
  } else {
    result.push_back(str.substr(prev));
  }
  return result;
}

inline std::string join(const std::vector<std::string>& strs, const std::string& delim = ",") {
  std::string result;
  for (size_t i = 0; i < strs.size(); ++i) {
    if (i != 0) result += delim;
    result += strs[i];
  }
  return result;
}

template <typename T>
inline std::string join(const std::vector<T>& strs, const std::string& delim = ",") {
  std::ostringstream oss;
  for (size_t i = 0; i < strs.size(); ++i) {
    if (i != 0) oss << delim;
    oss << strs[i];
  }
  return oss.str();
}

inline bool starts_with(const std::string& str, const std::string& prefix) {
  return str.size() >= prefix.size() && str.substr(0, prefix.size()) == prefix;
}

inline bool ends_with(const std::string& str, const std::string& suffix) {
  return str.size() >= suffix.size() && str.substr(str.size() - suffix.size()) == suffix;
}

inline std::string remove_prefix(const std::string& str, const std::string& prefix) {
  if (starts_with(str, prefix)) return str.substr(prefix.size());
  return str;
}

inline std::string remove_suffix(const std::string& str, const std::string& suffix) {
  if (ends_with(str, suffix)) return str.substr(0, str.size() - suffix.size());
  return str;
}

inline int icompare(const std::string& lhs, const std::string& rhs) {
  size_t len = std::min(lhs.size(), rhs.size());
  for (size_t i = 0; i < len; ++i) {
    if (std::toupper(lhs[i]) < std::toupper(rhs[i])) return -1;
    if (std::toupper(lhs[i]) > std::toupper(rhs[i])) return 1;
  }
  if (lhs.size() < rhs.size()) return -1;
  if (lhs.size() > rhs.size()) return 1;
  return 0;
}

inline bool iless(const std::string& lhs, const std::string& rhs) {
  return icompare(lhs, rhs) < 0;
}

class iLess {
 public:
  bool operator()(const std::string& lhs, const std::string& rhs) const { return iless(lhs, rhs); }
  using first_argument_type  = std::string;
  using second_argument_type = std::string;
  using result_type          = bool;
};

inline bool igreater(const std::string& lhs, const std::string& rhs) {
  return icompare(lhs, rhs) > 0;
}

class iGreater {
 public:
  bool operator()(const std::string& lhs, const std::string& rhs) const { return igreater(lhs, rhs); }
  using first_argument_type  = std::string;
  using second_argument_type = std::string;
  using result_type          = bool;
};

}  // namespace string
}  // namespace yu

#endif  // YU_STRING_UTILS_HPP_
