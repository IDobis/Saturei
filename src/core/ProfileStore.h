#pragma once
#include <filesystem>
#include <string>
#include <vector>
#include "core/Profile.h"

// Persiste perfis em %APPDATA%\Saturei\profiles.json
class ProfileStore {
 public:
  ProfileStore();
  const std::vector<Profile>& all() const { return profiles_; }
  void upsert(const Profile& p);
  void remove(const std::string& id);
  const Profile* forExe(const std::string& exeLower) const;
  const Profile* byId(const std::string& id) const;

 private:
  void load();
  void save() const;
  std::filesystem::path path_;
  std::vector<Profile> profiles_;
};
