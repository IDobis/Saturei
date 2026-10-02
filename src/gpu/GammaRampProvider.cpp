#include "gpu/GammaRampProvider.h"
#include <windows.h>
#include <algorithm>
#include <cmath>

void GammaRampProvider::apply(int /*saturation*/, int contrast) {
  // contrast 50 = neutro. Curva linear em torno do meio-cinza.
  const double k = 0.5 + contrast / 100.0;  // 0.5..1.5
  WORD ramp[3][256];
  for (int i = 0; i < 256; ++i) {
    double v = std::clamp((i / 255.0 - 0.5) * k + 0.5, 0.0, 1.0);
    WORD w = static_cast<WORD>(std::lround(v * 65535.0));
    ramp[0][i] = ramp[1][i] = ramp[2][i] = w;
  }
  HDC dc = GetDC(nullptr);
  SetDeviceGammaRamp(dc, ramp);
  ReleaseDC(nullptr, dc);
}

void GammaRampProvider::reset() { apply(100, 50); }
