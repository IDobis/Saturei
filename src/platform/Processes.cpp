#include "platform/Processes.h"
#include <windows.h>
#include <filesystem>
#include <memory>
#include <set>
#include "core/StringUtil.h"
#include "platform/Text.h"

namespace saturei::processes {

namespace {

struct HandleCloser {
  void operator()(HANDLE h) const { CloseHandle(h); }
};
using UniqueHandle = std::unique_ptr<void, HandleCloser>;

// File name of the process image, or empty if the process cannot be queried (e.g. elevated).
std::string exeNameOf(DWORD pid) {
  const UniqueHandle process(OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid));
  if (!process) return {};
  wchar_t path[MAX_PATH];
  DWORD len = MAX_PATH;
  if (!QueryFullProcessImageNameW(process.get(), 0, path, &len)) return {};
  return text::narrow(std::filesystem::path(path).filename().wstring());
}

struct EnumState {
  const std::string* exclude;
  std::set<std::string> seen;
  std::vector<WindowedApp> apps;
};

BOOL CALLBACK collectWindow(HWND hwnd, LPARAM param) {
  auto& state = *reinterpret_cast<EnumState*>(param);

  wchar_t title[256];
  if (!IsWindowVisible(hwnd) || GetWindow(hwnd, GW_OWNER) || !GetWindowTextW(hwnd, title, 256)) return TRUE;

  DWORD pid = 0;
  GetWindowThreadProcessId(hwnd, &pid);
  std::string exe = exeNameOf(pid);
  if (exe.empty()) return TRUE;

  const std::string key = toLowerAscii(exe);
  if (key != *state.exclude && state.seen.insert(key).second)
    state.apps.push_back({std::move(exe), text::narrow(title)});
  return TRUE;
}

}  // namespace

std::string foregroundExe() {
  HWND foreground = GetForegroundWindow();
  DWORD pid = 0;
  if (!foreground || !GetWindowThreadProcessId(foreground, &pid) || pid == 0) return {};
  return toLowerAscii(exeNameOf(pid));
}

std::vector<WindowedApp> listWindowedApps(const std::string& excludeExe) {
  EnumState state{&excludeExe, {}, {}};
  EnumWindows(collectWindow, reinterpret_cast<LPARAM>(&state));
  return std::move(state.apps);
}

}  // namespace saturei::processes
