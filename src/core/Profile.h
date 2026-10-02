#pragma once
#include <algorithm>
#include <string>
#include <nlohmann/json.hpp>
#include "core/Limits.h"

namespace saturei {

// Id of the built-in profile used when no game profile matches the focused app.
inline constexpr const char* kWindowsProfileId = "win";

struct Profile {
  std::string id;
  std::string name;
  std::string exe;  // executable file name, e.g. "cs2.exe"; empty for the Windows profile
  bool enabled = true;
  int saturation = kNeutralSaturation;
  int contrast = kNeutralContrast;

  // Forces the numeric fields into their valid ranges (input may come from the UI or a hand-edited file).
  Profile& sanitize() {
    saturation = std::clamp(saturation, kMinSaturation, kMaxSaturation);
    contrast = std::clamp(contrast, kMinContrast, kMaxContrast);
    return *this;
  }
};

// Unknown JSON keys (e.g. from older versions) are ignored; missing keys keep their defaults.
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Profile, id, name, exe, enabled, saturation, contrast)

inline Profile makeWindowsProfile() {
  return Profile{kWindowsProfileId, "Windows", "", true, kNeutralSaturation, kNeutralContrast};
}

}  // namespace saturei
