#pragma once
#include "gpu/GpuProvider.h"

namespace saturei {

// Last-resort fallback: SetDeviceGammaRamp. Only contrast is supported (a gamma ramp cannot saturate).
class GammaRampProvider : public IGpuProvider {
 public:
  const char* name() const override { return "gamma-ramp"; }
  bool available() const override { return true; }
  void apply(int saturation, int contrast) override;
  void reset() override;
};

}  // namespace saturei
