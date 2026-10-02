#pragma once

namespace saturei {

// Applies saturation/contrast to the screen through one specific mechanism.
//   saturation: 0..300 (%), 100 = unchanged
//   contrast:   0..100,     50  = unchanged
// Implementations restore the original look in reset().
class IGpuProvider {
 public:
  virtual ~IGpuProvider() = default;
  virtual const char* name() const = 0;
  virtual bool available() const = 0;
  virtual void apply(int saturation, int contrast) = 0;
  virtual void reset() = 0;
};

}  // namespace saturei
