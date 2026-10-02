#include "gpu/NvidiaProvider.h"
#include <algorithm>
#include <cmath>

namespace {
constexpr unsigned kInitialize = 0x0150E828;
constexpr unsigned kUnload = 0xD22BDD7E;
constexpr unsigned kEnumDisplay = 0x9ABDD40D;
constexpr unsigned kGetDvcEx = 0x0E45002D;
constexpr unsigned kSetDvcEx = 0x4A82C2B1;
constexpr unsigned kDvcVersion1 = 20u | (1u << 16);  // sizeof(NV_DISPLAY_DVC_INFO_EX) | (1 << 16)
constexpr int kMaxDisplays = 16;

int lerpInt(int a, int b, double t) { return (int)std::lround(a + (b - a) * std::clamp(t, 0.0, 1.0)); }
}  // namespace

NvidiaProvider::NvidiaProvider() {
  dll_ = LoadLibraryW(L"nvapi64.dll");  // vem com o driver NVIDIA
  if (!dll_) return;

  auto query = reinterpret_cast<QueryFn>(GetProcAddress(dll_, "nvapi_QueryInterface"));
  auto init = query ? reinterpret_cast<StatusFn>(query(kInitialize)) : nullptr;
  auto enumFn = query ? reinterpret_cast<EnumFn>(query(kEnumDisplay)) : nullptr;
  auto getFn = query ? reinterpret_cast<DvcFn>(query(kGetDvcEx)) : nullptr;
  set_ = query ? reinterpret_cast<DvcFn>(query(kSetDvcEx)) : nullptr;
  unload_ = query ? reinterpret_cast<StatusFn>(query(kUnload)) : nullptr;
  if (!init || !enumFn || !getFn || !set_ || init() != 0) return;

  // Monitores dirigidos pela NVIDIA. Em notebook hibrido (tela na Intel) a lista vem vazia.
  for (unsigned i = 0; i < kMaxDisplays; ++i) {
    void* h = nullptr;
    if (enumFn(i, &h) != 0 || !h) break;  // NVAPI_END_ENUMERATION
    DvcInfoEx info{kDvcVersion1, 0, 0, 0, 0};
    if (getFn(h, 0, &info) == 0 && info.maxLevel > info.minLevel)
      displays_.push_back({h, info.minLevel, info.maxLevel, info.defaultLevel});
  }
}

NvidiaProvider::~NvidiaProvider() {
  reset();
  if (unload_) unload_();
  if (dll_) FreeLibrary(dll_);
}

bool NvidiaProvider::setLevel(const Display& d, int level) {
  DvcInfoEx info{kDvcVersion1, std::clamp(level, d.minLevel, d.maxLevel), 0, 0, 0};
  return set_(d.handle, 0, &info) == 0;
}

void NvidiaProvider::apply(int saturation, int contrast) {
  const double s = std::clamp(saturation, 0, 300);
  for (const auto& d : displays_) {
    // 100% = nivel padrao do driver; 0% = minimo; 300% = maximo (escala informada pelo proprio driver).
    const int level = s <= 100 ? lerpInt(d.minLevel, d.defaultLevel, s / 100.0)
                               : lerpInt(d.defaultLevel, d.maxLevel, (s - 100.0) / 200.0);
    setLevel(d, level);
  }
  gamma_.apply(100, contrast);
}

void NvidiaProvider::reset() {
  for (const auto& d : displays_) setLevel(d, d.defaultLevel);
  gamma_.reset();
}
