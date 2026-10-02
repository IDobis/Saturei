#include "gpu/MagnificationProvider.h"
#include <windows.h>
#include <magnification.h>
#include "core/Limits.h"
#include "gpu/ColorMath.h"

namespace saturei {

MagnificationProvider::MagnificationProvider() { initialized_ = MagInitialize() != FALSE; }

MagnificationProvider::~MagnificationProvider() {
  if (!initialized_) return;
  reset();
  MagUninitialize();
}

void MagnificationProvider::apply(int saturation, int contrast) {
  if (!initialized_) return;
  const color::Matrix5 matrix = color::saturationContrastMatrix(saturation, contrast);
  MAGCOLOREFFECT effect{};
  for (int i = 0; i < 5; ++i)
    for (int j = 0; j < 5; ++j) effect.transform[i][j] = matrix[i][j];
  MagSetFullscreenColorEffect(&effect);
}

void MagnificationProvider::reset() { apply(kNeutralSaturation, kNeutralContrast); }  // identity matrix

}  // namespace saturei
