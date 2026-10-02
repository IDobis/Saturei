#include "TestFramework.h"
#include "gpu/ColorMath.h"

using namespace saturei;
using namespace saturei::color;

TEST(neutral_settings_are_the_identity) {
  const Matrix5 m = saturationContrastMatrix(kNeutralSaturation, kNeutralContrast);
  for (int i = 0; i < 5; ++i)
    for (int j = 0; j < 5; ++j) CHECK_NEAR(m[i][j], i == j ? 1.0 : 0.0, 1e-6);
}

TEST(neutral_settings_leave_pixels_untouched) {
  const auto out = transform(saturationContrastMatrix(100, 50), 0.8f, 0.3f, 0.1f);
  CHECK_NEAR(out[0], 0.8, 1e-5);
  CHECK_NEAR(out[1], 0.3, 1e-5);
  CHECK_NEAR(out[2], 0.1, 1e-5);
}

TEST(zero_saturation_gives_grayscale_of_the_luma) {
  const auto out = transform(saturationContrastMatrix(0, 50), 1.0f, 0.0f, 0.0f);
  CHECK_NEAR(out[0], kLuma[0], 1e-5);  // pure red -> its Rec.709 luma on every channel
  CHECK_NEAR(out[1], kLuma[0], 1e-5);
  CHECK_NEAR(out[2], kLuma[0], 1e-5);
}

TEST(saturation_never_tints_gray) {
  for (int pct : {0, 50, 100, 200, 300}) {
    const auto out = transform(saturationContrastMatrix(pct, 50), 0.4f, 0.4f, 0.4f);
    CHECK_NEAR(out[0], 0.4, 1e-5);
    CHECK_NEAR(out[1], 0.4, 1e-5);
    CHECK_NEAR(out[2], 0.4, 1e-5);
  }
}

TEST(saturation_above_100_increases_color_distance_from_gray) {
  auto spread = [](int pct) {
    const auto out = transform(saturationContrastMatrix(pct, 50), 0.6f, 0.4f, 0.4f);
    return out[0] - out[1];
  };
  CHECK(spread(200) > spread(100));
  CHECK(spread(300) > spread(200));
  CHECK(spread(50) < spread(100));
}

TEST(saturation_is_clamped_to_the_valid_range) {
  CHECK_NEAR(saturationScale(-50), 0.0, 1e-6);
  CHECK_NEAR(saturationScale(9999), 3.0, 1e-6);
  const auto a = saturationContrastMatrix(9999, 50);
  const auto b = saturationContrastMatrix(300, 50);
  CHECK_NEAR(a[0][0], b[0][0], 1e-6);
}

TEST(contrast_keeps_mid_gray_fixed) {
  for (int contrast : {0, 25, 50, 75, 100}) {
    const auto out = transform(saturationContrastMatrix(100, contrast), 0.5f, 0.5f, 0.5f);
    CHECK_NEAR(out[0], 0.5, 1e-5);
  }
}

TEST(contrast_pushes_values_away_from_mid_gray) {
  const auto high = transform(saturationContrastMatrix(100, 100), 0.75f, 0.25f, 0.5f);
  CHECK(high[0] > 0.75);
  CHECK(high[1] < 0.25);
  const auto low = transform(saturationContrastMatrix(100, 0), 0.75f, 0.25f, 0.5f);
  CHECK(low[0] < 0.75);
  CHECK(low[1] > 0.25);
}

TEST(vibrance_level_hits_the_driver_range_anchors) {
  CHECK_EQ(saturationToLevel(0, 0, 50, 100), 0);
  CHECK_EQ(saturationToLevel(100, 0, 50, 100), 50);
  CHECK_EQ(saturationToLevel(300, 0, 50, 100), 100);
}

TEST(vibrance_level_interpolates_linearly) {
  CHECK_EQ(saturationToLevel(50, 0, 50, 100), 25);
  CHECK_EQ(saturationToLevel(200, 0, 50, 100), 75);
}

TEST(vibrance_level_works_with_a_nonzero_driver_minimum) {
  CHECK_EQ(saturationToLevel(0, 10, 20, 63), 10);
  CHECK_EQ(saturationToLevel(100, 10, 20, 63), 20);
  CHECK_EQ(saturationToLevel(300, 10, 20, 63), 63);
}

TEST(vibrance_level_is_monotonic_and_clamped) {
  int previous = saturationToLevel(0, 0, 50, 100);
  for (int pct = 1; pct <= 300; ++pct) {
    const int level = saturationToLevel(pct, 0, 50, 100);
    CHECK(level >= previous);
    previous = level;
  }
  CHECK_EQ(saturationToLevel(-10, 0, 50, 100), 0);
  CHECK_EQ(saturationToLevel(5000, 0, 50, 100), 100);
}
