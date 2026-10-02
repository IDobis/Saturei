#pragma once
#include <string>
#include <vector>

namespace saturei::processes {

struct WindowedApp {
  std::string exe;    // file name as reported by Windows, e.g. "Cs2.exe"
  std::string title;  // main window title
};

// Executable name (lowercase) of the process that owns the foreground window; empty if unknown.
std::string foregroundExe();

// One entry per executable that has a visible top-level window (used to pick a game for a new profile).
// `excludeExe` (lowercase) is skipped, typically this app itself.
std::vector<WindowedApp> listWindowedApps(const std::string& excludeExe);

}  // namespace saturei::processes
