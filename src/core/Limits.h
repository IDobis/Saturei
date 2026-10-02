#pragma once

namespace saturei {

// Value ranges shared by the profile model, the color math and the UI
// (the UI mirrors these in ui/src/constants.ts).
inline constexpr int kMinSaturation = 0;
inline constexpr int kNeutralSaturation = 100;  // percent; 100 = unchanged
inline constexpr int kMaxSaturation = 300;

inline constexpr int kMinContrast = 0;
inline constexpr int kNeutralContrast = 50;  // 50 = unchanged
inline constexpr int kMaxContrast = 100;

}  // namespace saturei
