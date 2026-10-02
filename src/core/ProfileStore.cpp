#include "core/ProfileStore.h"
#include <algorithm>
#include <cstdlib>
#include <fstream>

namespace fs = std::filesystem;

static std::string lower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return (char)std::tolower(c); });
  return s;
}

ProfileStore::ProfileStore() {
  const char* appdata = std::getenv("APPDATA");
  fs::path dir = fs::path(appdata ? appdata : ".") / "Saturei";
  fs::create_directories(dir);
  path_ = dir / "profiles.json";
  load();
  if (!byId("win")) profiles_.insert(profiles_.begin(), Profile{"win", "Windows", "", true, 100, 50});
}

void ProfileStore::load() {
  std::ifstream f(path_);
  if (!f) return;
  auto j = nlohmann::json::parse(f, nullptr, false);
  if (j.is_array()) profiles_ = j.get<std::vector<Profile>>();
}

void ProfileStore::save() const {
  std::ofstream(path_) << nlohmann::json(profiles_).dump(2);
}

void ProfileStore::upsert(const Profile& p) {
  auto it = std::find_if(profiles_.begin(), profiles_.end(), [&](auto& x) { return x.id == p.id; });
  if (it != profiles_.end()) *it = p; else profiles_.push_back(p);
  save();
}

void ProfileStore::remove(const std::string& id) {
  if (id == "win") return;  // perfil padrao nao some
  std::erase_if(profiles_, [&](auto& p) { return p.id == id; });
  save();
}

const Profile* ProfileStore::forExe(const std::string& exeLower) const {
  for (auto& p : profiles_)
    if (p.enabled && !p.exe.empty() && lower(p.exe) == exeLower) return &p;
  return byId("win");
}

const Profile* ProfileStore::byId(const std::string& id) const {
  for (auto& p : profiles_) if (p.id == id) return &p;
  return nullptr;
}
