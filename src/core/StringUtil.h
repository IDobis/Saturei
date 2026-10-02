#pragma once
#include <algorithm>
#include <cctype>
#include <string>
#include <string_view>

namespace saturei {

inline std::string toLowerAscii(std::string_view s) {
  std::string out(s);
  std::transform(out.begin(), out.end(), out.begin(),
                 [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
  return out;
}

}  // namespace saturei
