#pragma once
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>
#include "core/Profile.h"

namespace saturei {

// Persistent list of profiles (JSON file). Invariants:
//  * the Windows profile always exists and cannot be removed;
//  * stored profiles always have sanitized numeric fields;
//  * writes are atomic (temp file + rename), and an unreadable file is quarantined, never overwritten.
class ProfileStore {
 public:
  // %APPDATA%\Saturei\profiles.json
  static std::filesystem::path defaultPath();

  explicit ProfileStore(std::filesystem::path path = defaultPath());

  const std::vector<Profile>& all() const { return profiles_; }
  const Profile* byId(std::string_view id) const;
  const Profile& windowsProfile() const;

  // Profile to apply for the given focused executable (case-insensitive). Falls back to the Windows profile.
  const Profile& forExe(std::string_view exe) const;

  void upsert(Profile profile);
  // Returns false if the id is unknown or is the Windows profile.
  bool remove(std::string_view id);

 private:
  void load();
  void quarantineCorruptFile();
  void save() const;

  std::filesystem::path path_;
  std::vector<Profile> profiles_;
};

}  // namespace saturei
