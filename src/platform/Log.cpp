#include "platform/Log.h"
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <memory>
#include <mutex>
#include <string>

namespace fs = std::filesystem;

namespace saturei::log {

namespace {

constexpr std::uintmax_t kMaxBytes = 256 * 1024;

fs::path logPath() {
  char* raw = nullptr;
  size_t len = 0;
  _dupenv_s(&raw, &len, "LOCALAPPDATA");
  const std::unique_ptr<char, decltype(&std::free)> localAppData(raw, &std::free);
  if (!localAppData) return {};
  return fs::path(localAppData.get()) / "Saturei" / "saturei.log";
}

}  // namespace

void info(std::string_view message) {
  try {
    static std::mutex mutex;
    const std::scoped_lock lock(mutex);

    const fs::path path = logPath();
    if (path.empty()) return;
    std::error_code ec;
    fs::create_directories(path.parent_path(), ec);

    const bool restart = fs::exists(path, ec) && fs::file_size(path, ec) > kMaxBytes;
    std::ofstream out(path, restart ? std::ios::trunc : std::ios::app);

    const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm local{};
    localtime_s(&local, &now);
    char stamp[32];
    std::strftime(stamp, sizeof(stamp), "%Y-%m-%d %H:%M:%S", &local);
    out << stamp << "  " << message << '\n';
  } catch (...) {
    // logging must never take the app down
  }
}

}  // namespace saturei::log
