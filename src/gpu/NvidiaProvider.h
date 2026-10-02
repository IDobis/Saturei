#pragma once
#include <windows.h>
#include <vector>
#include "gpu/GammaRampProvider.h"
#include "gpu/GpuProvider.h"

// Vibrancia digital (saturacao) no driver NVIDIA via nvapi64.dll + nvapi_QueryInterface.
// Funciona tambem em jogos em fullscreen exclusivo (nao passa pelo compositor do Windows).
// As funcoes DVC nao sao publicas no SDK; IDs/assinaturas conferidos em jNizM/NVIDIA_NvAPI e
// Blazzer10200/exfil. Se qualquer passo falhar, available() = false e o app usa outro provedor.
// Contraste: gamma ramp (a NvAPI de vibrancia so cuida de saturacao).
class NvidiaProvider : public IGpuProvider {
 public:
  NvidiaProvider();
  ~NvidiaProvider() override;
  const char* name() const override { return "nvapi"; }
  bool available() const override { return !displays_.empty(); }
  bool trueSaturation() const override { return true; }
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
  using QueryFn = void* (__cdecl*)(unsigned);
  using StatusFn = int(__cdecl*)();
  using EnumFn = int(__cdecl*)(unsigned, void**);
  using DvcFn = int(__cdecl*)(void*, unsigned, DvcInfoEx*);

  bool setLevel(const Display& d, int level);
  HMODULE dll_ = nullptr;
  StatusFn unload_ = nullptr;
  DvcFn set_ = nullptr;
  std::vector<Display> displays_;
  GammaRampProvider gamma_;
};
