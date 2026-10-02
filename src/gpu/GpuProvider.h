#pragma once
#include <string>

struct GpuAdapter {
  enum class Vendor { Nvidia, Amd, Intel, Unknown } vendor = Vendor::Unknown;
  std::string name;
};

// Um provedor aplica cor na tela.
// saturation: 0..300 (%), 100 = neutro. contrast: 0..100, 50 = neutro.
class IGpuProvider {
 public:
  virtual ~IGpuProvider() = default;
  virtual const char* name() const = 0;
  virtual bool available() const = 0;
  virtual bool trueSaturation() const = 0;  // false = so contraste (fallback gamma)
  virtual void apply(int saturation, int contrast) = 0;
  virtual void reset() = 0;
};
