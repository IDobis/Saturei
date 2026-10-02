#include "gpu/GammaRampProvider.h"
#include <windows.h>
#include <algorithm>
#include <cmath>
#include "core/Limits.h"
#include "gpu/ColorMath.h"

namespace saturei {

void GammaRampProvider::apply(int /*saturation*/, int contrast) {
  const double k = color::contrastScale(contrast);
  WORD ramp[3][256];
  for (int i = 0; i < 256; ++i) {
    const double v = std::clamp((i / 255.0 - 0.5) * k + 0.5, 0.0, 1.0);
    ramp[0][i] = ramp[1][i] = ramp[2][i] = static_cast<WORD>(std::lround(v * 65535.0));
  }
  HDC dc = GetDC(nullptr);
  SetDeviceGammaRamp(dc, ramp);
  ReleaseDC(nullptr, dc);
}

void GammaRampProvider::reset() { apply(kNeutralSaturation, kNeutralContrast); }

}  // namespace saturei
