#pragma once
#include "gpu/GpuProvider.h"

// Fallback: SetDeviceGammaRamp (so contraste, sem saturacao).
class GammaRampProvider : public IGpuProvider {
 public:
  const char* name() const override { return "gamma-ramp"; }
  bool available() const override { return true; }
  bool trueSaturation() const override { return false; }
  void apply(int saturation, int contrast) override;
  void reset() override;
};
