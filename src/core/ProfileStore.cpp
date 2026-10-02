#include "core/ProfileStore.h"
#include <cstdlib>
#include <fstream>
#include <memory>
#include "core/StringUtil.h"

namespace fs = std::filesystem;

namespace saturei {

fs::path ProfileStore::defaultPath() {
  char* raw = nullptr;
  size_t len = 0;
  _dupenv_s(&raw, &len, "APPDATA");
  const std::unique_ptr<char, decltype(&std::free)> appData(raw, &std::free);
  const fs::path base = appData ? fs::path(appData.get()) : fs::current_path();
  return base / "Saturei" / "profiles.json";
}

ProfileStore::ProfileStore(fs::path path) : path_(std::move(path)) {
  std::error_code ec;
  fs::create_directories(path_.parent_path(), ec);
  load();
  if (!byId(kWindowsProfileId)) profiles_.insert(profiles_.begin(), makeWindowsProfile());
}

const Profile* ProfileStore::byId(std::string_view id) const {
  for (const auto& p : profiles_)
    if (p.id == id) return &p;
  return nullptr;
}

const Profile& ProfileStore::windowsProfile() const { return *byId(kWindowsProfileId); }

const Profile& ProfileStore::forExe(std::string_view exe) const {
  const std::string wanted = toLowerAscii(exe);
  for (const auto& p : profiles_)
    if (p.enabled && !p.exe.empty() && toLowerAscii(p.exe) == wanted) return p;
  return windowsProfile();
}

void ProfileStore::upsert(Profile profile) {
  profile.sanitize();
  auto it = std::find_if(profiles_.begin(), profiles_.end(), [&](const Profile& p) { return p.id == profile.id; });
  if (it != profiles_.end()) *it = std::move(profile);
  else profiles_.push_back(std::move(profile));
  save();
}

bool ProfileStore::remove(std::string_view id) {
  if (id == kWindowsProfileId) return false;
  const auto removed = std::erase_if(profiles_, [&](const Profile& p) { return p.id == id; });
  if (removed == 0) return false;
  save();
  return true;
}

void ProfileStore::load() {
  nlohmann::json json;
  {
    std::ifstream in(path_);
    if (!in) return;  // first run
    json = nlohmann::json::parse(in, nullptr, /*allow_exceptions=*/false);
  }  // the file must be closed before it can be renamed below

  if (json.is_discarded() || !json.is_array()) {
    quarantineCorruptFile();
    return;
  }
  try {
    profiles_ = json.get<std::vector<Profile>>();
  } catch (const nlohmann::json::exception&) {
    quarantineCorruptFile();
    return;
  }
  for (auto& p : profiles_) p.sanitize();
}

// Keeps the unreadable file next to the original so a later save can never destroy the user's data.
void ProfileStore::quarantineCorruptFile() {
  profiles_.clear();
  fs::path backup = path_;
  backup += ".corrupt";
  std::error_code ec;
  fs::rename(path_, backup, ec);
}

void ProfileStore::save() const {
  fs::path tmp = path_;
  tmp += ".tmp";
  {
    std::ofstream out(tmp, std::ios::binary | std::ios::trunc);
    out << nlohmann::json(profiles_).dump(2);
    if (!out) return;  // keep the previous file untouched
  }
  std::error_code ec;
  fs::rename(tmp, path_, ec);  // atomic replace
}

}  // namespace saturei
