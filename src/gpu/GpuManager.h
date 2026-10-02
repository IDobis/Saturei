#pragma once
#include <memory>
#include "gpu/GpuProvider.h"

namespace saturei {

// Picks the best available provider once, at startup:
//   NVIDIA driver (NvAPI)  ->  Windows color matrix (any GPU)  ->  gamma ramp (contrast only)
// TODO: native AMD (ADL/ADLX) and Intel (IGCL) providers.
class GpuManager {
 public:
  GpuManager();
  ~GpuManager();
  GpuManager(const GpuManager&) = delete;
  GpuManager& operator=(const GpuManager&) = delete;

  void apply(int saturation, int contrast) { provider_->apply(saturation, contrast); }
  void reset() { provider_->reset(); }
  const char* providerName() const { return provider_->name(); }

 private:
  std::unique_ptr<IGpuProvider> provider_;
};

}  // namespace saturei
