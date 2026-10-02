#pragma once
#include <windows.h>
#include <vector>
#include "gpu/GammaRampProvider.h"
#include "gpu/GpuProvider.h"

namespace saturei {

// NVIDIA digital vibrance through nvapi64.dll (shipped with the driver) and nvapi_QueryInterface.
// Works in exclusive fullscreen because it does not go through the Windows compositor.
//
// The DVC functions are not part of the public NvAPI SDK. IDs and signatures were checked against
// community projects (jNizM/NVIDIA_NvAPI, Blazzer10200/exfil). EXPERIMENTAL: not verified on real
// NVIDIA hardware. Any failure leaves available() == false so another provider is used instead.
// Contrast is applied with a gamma ramp, since vibrance only covers saturation.
class NvidiaProvider : public IGpuProvider {
 public:
  NvidiaProvider();
  ~NvidiaProvider() override;
  NvidiaProvider(const NvidiaProvider&) = delete;
  NvidiaProvider& operator=(const NvidiaProvider&) = delete;

  const char* name() const override { return "nvapi"; }
  bool available() const override { return !displays_.empty(); }
  void apply(int saturation, int contrast) override;
  void reset() override;

 private:
  struct DvcInfoEx {  // NV_DISPLAY_DVC_INFO_EX
    unsigned version;
    int currentLevel, minLevel, maxLevel, defaultLevel;
  };
  struct Display {
    void* handle;
    int minLevel, maxLevel, defaultLevel;
  };
  using QueryInterfaceFn = void*(__cdecl*)(unsigned);
  using StatusFn = int(__cdecl*)();
  using EnumDisplayFn = int(__cdecl*)(unsigned, void**);
  using DvcFn = int(__cdecl*)(void*, unsigned, DvcInfoEx*);

  bool loadFunctions();
  void enumerateDisplays(EnumDisplayFn enumDisplay);
  bool setLevel(const Display& display, int level) const;

  HMODULE dll_ = nullptr;
  StatusFn unload_ = nullptr;
  DvcFn getDvc_ = nullptr;
  DvcFn setDvc_ = nullptr;
  EnumDisplayFn enumDisplay_ = nullptr;
  std::vector<Display> displays_;
  GammaRampProvider gamma_;
};

}  // namespace saturei
