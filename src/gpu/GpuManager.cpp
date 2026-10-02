#include "gpu/GpuManager.h"
#include <windows.h>
#include <dxgi.h>
#include <algorithm>
#include <wrl/client.h>
#include "gpu/GammaRampProvider.h"
#include "gpu/MagnificationProvider.h"
#include "gpu/NvidiaProvider.h"

using Microsoft::WRL::ComPtr;

static std::string narrow(const wchar_t* w) {
  int n = WideCharToMultiByte(CP_UTF8, 0, w, -1, nullptr, 0, nullptr, nullptr);
  std::string s(n > 0 ? n - 1 : 0, '\0');
  WideCharToMultiByte(CP_UTF8, 0, w, -1, s.data(), n, nullptr, nullptr);
  return s;
}

GpuManager::GpuManager() {
  ComPtr<IDXGIFactory1> f;
  if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&f)))) {
    ComPtr<IDXGIAdapter1> a;
    for (UINT i = 0; f->EnumAdapters1(i, &a) != DXGI_ERROR_NOT_FOUND; ++i, a.Reset()) {
      DXGI_ADAPTER_DESC1 d{};
      a->GetDesc1(&d);
      if (d.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
      GpuAdapter g;
      g.name = narrow(d.Description);
      g.vendor = d.VendorId == 0x10DE ? GpuAdapter::Vendor::Nvidia
               : d.VendorId == 0x1002 ? GpuAdapter::Vendor::Amd
               : d.VendorId == 0x8086 ? GpuAdapter::Vendor::Intel
                                      : GpuAdapter::Vendor::Unknown;
      adapters_.push_back(std::move(g));
    }
  }
  // Ordem: driver NVIDIA (NvAPI) -> matriz de cor do Windows (qualquer GPU) -> so contraste.
  // TODO: AdlProvider (AMD) e IgclProvider (Intel) nativos.
  const bool hasNvidia = std::any_of(adapters_.begin(), adapters_.end(),
                                     [](const GpuAdapter& a) { return a.vendor == GpuAdapter::Vendor::Nvidia; });
  if (hasNvidia) {
    auto nv = std::make_unique<NvidiaProvider>();
    if (nv->available()) provider_ = std::move(nv);
  }
  if (!provider_) {
    auto mag = std::make_unique<MagnificationProvider>();
    if (mag->available()) provider_ = std::move(mag);
  }
  if (!provider_) provider_ = std::make_unique<GammaRampProvider>();
}

GpuManager::~GpuManager() { reset(); }
