#pragma once
#include <string>
#include <string_view>

namespace saturei::text {

// UTF-8 <-> UTF-16 conversions for Win32 / WebView2 boundaries.
std::wstring widen(std::string_view utf8);
std::string narrow(std::wstring_view utf16);

}  // namespace saturei::text
