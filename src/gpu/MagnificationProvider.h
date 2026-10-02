#pragma once
#include "gpu/GpuProvider.h"

// Saturacao real (0..200%) + contraste via Magnification API do Windows:
// uma matriz de cor 5x5 aplicada ao desktop inteiro (DWM). Funciona em qualquer GPU.
// Limite: jogos em fullscreen exclusivo "puro" (fora do DWM) nao sao afetados.
class MagnificationProvider : public IGpuProvider {
 public:
  MagnificationProvider();
  ~MagnificationProvider() override;
  const char* name() const override { return "magnification"; }
  bool available() const override { return ok_; }
  bool trueSaturation() const override { return true; }
  void apply(int saturation, int contrast) override;
  void reset() override;

 private:
  bool ok_ = false;
};
