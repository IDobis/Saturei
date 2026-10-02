#pragma once
#include <string_view>

namespace saturei::log {

// Appends a timestamped line to %LOCALAPPDATA%\Saturei\saturei.log (best effort, never throws).
// The file is restarted when it grows past ~256 KB. Used to diagnose which GPU path is active.
void info(std::string_view message);

}  // namespace saturei::log
