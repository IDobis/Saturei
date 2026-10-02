#pragma once
#include <memory>
#include <vector>
#include "gpu/GpuProvider.h"

// Detecta GPUs (DXGI) e escolhe o melhor provedor disponivel.
// TODO: NvApiProvider (NVIDIA), AdlProvider (AMD), IgclProvider (Intel Iris Xe).
class GpuManager {
 public:
  GpuManager();
  ~GpuManager();
  const std::vector<GpuAdapter>& adapters() const { return adapters_; }
  const char* method() const { return provider_ ? provider_->name() : "none"; }
  bool trueSaturation() const { return provider_ && provider_->trueSaturation(); }
  void apply(int saturation, int contrast) { if (provider_) provider_->apply(saturation, contrast); }
  void reset() { if (provider_) provider_->reset(); }

 private:
  std::vector<GpuAdapter> adapters_;
  std::unique_ptr<IGpuProvider> provider_;
};
