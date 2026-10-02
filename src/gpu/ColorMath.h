#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include "core/Limits.h"

// Pure color math shared by the GPU providers. No Windows dependencies, so it is unit-tested.
namespace saturei::color {

// Rec.709 luma weights (R, G, B).
inline constexpr std::array<float, 3> kLuma = {0.2126f, 0.7152f, 0.0722f};

// 5x5 color matrix using the row-vector convention of the Windows Magnification API:
// [r g b a 1] * M. The last row is the translation.
using Matrix5 = std::array<std::array<float, 5>, 5>;

// Saturation as a multiplier: 0 = grayscale, 1 = unchanged, 3 = 300%.
inline float saturationScale(int percent) {
  return static_cast<float>(std::clamp(percent, kMinSaturation, kMaxSaturation)) / 100.0f;
}

// Contrast as a multiplier around mid-gray: 0.5 .. 1.5, 1 = unchanged (slider value 50).
inline float contrastScale(int value) {
  return 0.5f + static_cast<float>(std::clamp(value, kMinContrast, kMaxContrast)) / 100.0f;
}

// Saturation blends each color toward its luma (M[i][j] = (1-s)*luma[i] + s*delta(i,j));
// contrast then scales around 0.5 (translation 0.5*(1-k)).
inline Matrix5 saturationContrastMatrix(int saturationPercent, int contrast) {
  const float s = saturationScale(saturationPercent);
  const float k = contrastScale(contrast);

  Matrix5 m{};
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j) m[i][j] = k * ((1.0f - s) * kLuma[i] + (i == j ? s : 0.0f));
  m[3][3] = 1.0f;
  for (int j = 0; j < 3; ++j) m[4][j] = 0.5f * (1.0f - k);
  m[4][4] = 1.0f;
  return m;
}

// Applies the matrix to one pixel (channels in 0..1). Used by the tests to check the math.
inline std::array<float, 3> transform(const Matrix5& m, float r, float g, float b) {
  const float in[3] = {r, g, b};
  std::array<float, 3> out{};
  for (int j = 0; j < 3; ++j) {
    out[j] = m[4][j];
    for (int i = 0; i < 3; ++i) out[j] += in[i] * m[i][j];
  }
  return out;
}

// Maps the saturation percentage onto a driver-reported level range (NVIDIA digital vibrance):
// 0% -> minLevel, 100% -> defaultLevel, 300% -> maxLevel, linear in between.
inline int saturationToLevel(int percent, int minLevel, int defaultLevel, int maxLevel) {
  const int p = std::clamp(percent, kMinSaturation, kMaxSaturation);
  const double level = p <= kNeutralSaturation
      ? minLevel + (defaultLevel - minLevel) * (static_cast<double>(p) / kNeutralSaturation)
      : defaultLevel + (maxLevel - defaultLevel) * (static_cast<double>(p - kNeutralSaturation) / (kMaxSaturation - kNeutralSaturation));
  return static_cast<int>(std::lround(level));
}

}  // namespace saturei::color
