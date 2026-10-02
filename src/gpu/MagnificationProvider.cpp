#include "gpu/MagnificationProvider.h"
#include <windows.h>
#include <magnification.h>
#include <algorithm>

MagnificationProvider::MagnificationProvider() { ok_ = MagInitialize() != FALSE; }

MagnificationProvider::~MagnificationProvider() {
  if (ok_) {
    reset();
    MagUninitialize();
  }
}

void MagnificationProvider::apply(int saturation, int contrast) {
  if (!ok_) return;
  const float s = std::clamp(saturation, 0, 300) / 100.0f;  // 1.0 = neutro
  const float k = 0.5f + std::clamp(contrast, 0, 100) / 100.0f;  // 0.5..1.5, 1.0 = neutro
  // Pesos de luminancia Rec.709.
  constexpr float lw[3] = {0.2126f, 0.7152f, 0.0722f};

  // Vetor-linha [r g b a 1] * M: saida_j = soma_i(entrada_i * M[i][j]).
  // Saturacao: M[i][j] = (1-s)*lw[i] + s*delta(i,j)  (lw indexado pela ENTRADA i).
  // Contraste: escala k em torno do cinza medio (translacao 0.5*(1-k)).
  MAGCOLOREFFECT m{};
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      m.transform[i][j] = k * ((1.0f - s) * lw[i] + (i == j ? s : 0.0f));
  m.transform[3][3] = 1.0f;
  for (int j = 0; j < 3; ++j) m.transform[4][j] = 0.5f * (1.0f - k);
  m.transform[4][4] = 1.0f;
  MagSetFullscreenColorEffect(&m);
}

void MagnificationProvider::reset() {
  if (ok_) apply(100, 50);  // matriz identidade
}
