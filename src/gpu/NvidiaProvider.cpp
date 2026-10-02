#include "gpu/NvidiaProvider.h"
#include <algorithm>
#include <string>
#include "gpu/ColorMath.h"
#include "platform/Log.h"

namespace saturei {

namespace {

// nvapi_QueryInterface function ids.
constexpr unsigned kIdInitialize = 0x0150E828;
constexpr unsigned kIdUnload = 0xD22BDD7E;
constexpr unsigned kIdEnumDisplayHandle = 0x9ABDD40D;
constexpr unsigned kIdGetDvcInfoEx = 0x0E45002D;
constexpr unsigned kIdSetDvcLevelEx = 0x4A82C2B1;

constexpr unsigned kDvcInfoExVersion1 = 20u | (1u << 16);  // sizeof(NV_DISPLAY_DVC_INFO_EX) | (1 << 16)
constexpr unsigned kMaxDisplays = 16;
constexpr int kNvApiOk = 0;

}  // namespace

NvidiaProvider::NvidiaProvider() {
  if (!loadFunctions()) return;
  enumerateDisplays(enumDisplay_);
  if (displays_.empty()) {
    // Typical on hybrid laptops: the screen is driven by the integrated GPU.
    log::info("nvapi: no display is driven by the NVIDIA GPU");
    return;
  }
  const Display& d = displays_.front();
  log::info("nvapi: " + std::to_string(displays_.size()) + " display(s), vibrance range " + std::to_string(d.minLevel) +
            ".." + std::to_string(d.maxLevel) + ", default " + std::to_string(d.defaultLevel));
}

NvidiaProvider::~NvidiaProvider() {
  if (!displays_.empty()) reset();
  if (unload_) unload_();
  if (dll_) FreeLibrary(dll_);
}

bool NvidiaProvider::loadFunctions() {
  dll_ = LoadLibraryW(L"nvapi64.dll");  // installed with the NVIDIA driver
  if (!dll_) {
    log::info("nvapi: nvapi64.dll not found");
    return false;
  }
  const auto query = reinterpret_cast<QueryInterfaceFn>(GetProcAddress(dll_, "nvapi_QueryInterface"));
  if (!query) {
    log::info("nvapi: nvapi_QueryInterface not exported");
    return false;
  }

  const auto initialize = reinterpret_cast<StatusFn>(query(kIdInitialize));
  unload_ = reinterpret_cast<StatusFn>(query(kIdUnload));
  enumDisplay_ = reinterpret_cast<EnumDisplayFn>(query(kIdEnumDisplayHandle));
  getDvc_ = reinterpret_cast<DvcFn>(query(kIdGetDvcInfoEx));
  setDvc_ = reinterpret_cast<DvcFn>(query(kIdSetDvcLevelEx));

  if (!initialize || !enumDisplay_ || !getDvc_ || !setDvc_) {
    log::info("nvapi: required functions are missing from this driver");
    return false;
  }
  if (const int status = initialize(); status != kNvApiOk) {
    log::info("nvapi: NvAPI_Initialize failed, status " + std::to_string(status));
    return false;
  }
  return true;
}

void NvidiaProvider::enumerateDisplays(EnumDisplayFn enumDisplay) {
  for (unsigned i = 0; i < kMaxDisplays; ++i) {
    void* handle = nullptr;
    if (enumDisplay(i, &handle) != kNvApiOk || !handle) break;  // NVAPI_END_ENUMERATION

    DvcInfoEx info{kDvcInfoExVersion1, 0, 0, 0, 0};
    if (getDvc_(handle, 0, &info) == kNvApiOk && info.maxLevel > info.minLevel)
      displays_.push_back({handle, info.minLevel, info.maxLevel, info.defaultLevel});
  }
}

bool NvidiaProvider::setLevel(const Display& display, int level) const {
  DvcInfoEx info{kDvcInfoExVersion1, std::clamp(level, display.minLevel, display.maxLevel), 0, 0, 0};
  return setDvc_(display.handle, 0, &info) == kNvApiOk;
}

void NvidiaProvider::apply(int saturation, int contrast) {
  for (const Display& d : displays_)
    setLevel(d, color::saturationToLevel(saturation, d.minLevel, d.defaultLevel, d.maxLevel));
  gamma_.apply(kNeutralSaturation, contrast);
}

void NvidiaProvider::reset() {
  for (const Display& d : displays_) setLevel(d, d.defaultLevel);
  gamma_.reset();
}

}  // namespace saturei
