#ifndef YU_STRING_UTILS_HPP_
#define YU_STRING_UTILS_HPP_

#include <string>
#include <vector>

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

inline bool starts_with(const std::string& str, const std::string& prefix) {
  return str.size() >= prefix.size() && str.substr(0, prefix.size()) == prefix;
}

inline bool ends_with(const std::string& str, const std::string& suffix) {
  return str.size() >= suffix.size() && str.substr(str.size() - suffix.size()) == suffix;
}

}  // namespace string
}  // namespace yu

#endif  // YU_STRING_UTILS_HPP_
