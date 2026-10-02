#pragma once
#include <windows.h>

namespace saturei::theme {

// Dark palette shared by the native window and the WebView so there is no white flash or seam.
inline constexpr BYTE kBackgroundR = 10, kBackgroundG = 10, kBackgroundB = 10;
inline constexpr COLORREF kBackground = RGB(kBackgroundR, kBackgroundG, kBackgroundB);
inline constexpr COLORREF kBorder = RGB(38, 38, 38);

}  // namespace saturei::theme
