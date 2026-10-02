#include "gpu/GpuManager.h"
#include <windows.h>
#include <dxgi.h>
#include <wrl/client.h>
#include <string>
#include "gpu/GammaRampProvider.h"
#include "gpu/MagnificationProvider.h"
#include "gpu/NvidiaProvider.h"
#include "platform/Log.h"

using Microsoft::WRL::ComPtr;

namespace saturei {

namespace {

constexpr UINT kVendorNvidia = 0x10DE;

bool hasAdapterFromVendor(UINT vendorId) {
  ComPtr<IDXGIFactory1> factory;
  if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory)))) return false;

  ComPtr<IDXGIAdapter1> adapter;
  for (UINT i = 0; factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i, adapter.Reset()) {
    DXGI_ADAPTER_DESC1 desc{};
    adapter->GetDesc1(&desc);
    if (!(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) && desc.VendorId == vendorId) return true;
  }
  return false;
}

}  // namespace

GpuManager::GpuManager() {
  if (hasAdapterFromVendor(kVendorNvidia)) {
    auto nvidia = std::make_unique<NvidiaProvider>();
    if (nvidia->available()) provider_ = std::move(nvidia);
  }
  if (!provider_) {
    auto magnification = std::make_unique<MagnificationProvider>();
    if (magnification->available()) provider_ = std::move(magnification);
  }
  if (!provider_) provider_ = std::make_unique<GammaRampProvider>();

  log::info(std::string("color provider: ") + provider_->name());
}

GpuManager::~GpuManager() { provider_->reset(); }

}  // namespace saturei
