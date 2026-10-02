#pragma once
#include "gpu/GpuProvider.h"

namespace saturei {

// Real saturation (0..300%) and contrast through the Windows Magnification API: one 5x5 color matrix
// applied to the whole desktop by the compositor (DWM). Works on any GPU.
// Limitation: games in true exclusive fullscreen bypass the compositor and ignore the effect.
class MagnificationProvider : public IGpuProvider {
 public:
  MagnificationProvider();
  ~MagnificationProvider() override;
  MagnificationProvider(const MagnificationProvider&) = delete;
  MagnificationProvider& operator=(const MagnificationProvider&) = delete;

  const char* name() const override { return "magnification"; }
  bool available() const override { return initialized_; }
  void apply(int saturation, int contrast) override;
  void reset() override;

 private:
  bool initialized_ = false;
};

}  // namespace saturei
