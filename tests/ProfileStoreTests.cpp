#include <chrono>
#include <filesystem>
#include <fstream>
#include "TestFramework.h"
#include "core/ProfileStore.h"

using namespace saturei;
namespace fs = std::filesystem;

namespace {

// A throw-away directory removed at the end of each test.
struct TempDir {
  fs::path dir;
  TempDir() {
    static int counter = 0;
    const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
    dir = fs::temp_directory_path() / ("saturei-tests-" + std::to_string(stamp) + "-" + std::to_string(++counter));
    fs::create_directories(dir);
  }
  ~TempDir() {
    std::error_code ec;
    fs::remove_all(dir, ec);
  }
  fs::path file() const { return dir / "profiles.json"; }
};

Profile game(const std::string& id, const std::string& exe, int saturation = 200) {
  return Profile{id, "Game " + id, exe, true, saturation, 50};
}

void writeFile(const fs::path& path, const std::string& content) { std::ofstream(path) << content; }

}  // namespace

TEST(new_store_has_only_the_windows_profile) {
  TempDir t;
  ProfileStore store(t.file());
  CHECK_EQ(store.all().size(), size_t{1});
  CHECK(store.byId(kWindowsProfileId) != nullptr);
  CHECK_EQ(store.windowsProfile().saturation, 100);
}

TEST(profiles_survive_a_restart) {
  TempDir t;
  {
    ProfileStore store(t.file());
    store.upsert(game("a", "cs2.exe", 250));
  }
  ProfileStore reloaded(t.file());
  CHECK_EQ(reloaded.all().size(), size_t{2});
  const Profile* p = reloaded.byId("a");
  CHECK(p != nullptr);
  CHECK_EQ(p->saturation, 250);
  CHECK(p->exe == "cs2.exe");
}

TEST(upsert_replaces_an_existing_profile) {
  TempDir t;
  ProfileStore store(t.file());
  store.upsert(game("a", "cs2.exe", 150));
  store.upsert(game("a", "cs2.exe", 180));
  CHECK_EQ(store.all().size(), size_t{2});
  CHECK_EQ(store.byId("a")->saturation, 180);
}

TEST(upsert_clamps_out_of_range_values) {
  TempDir t;
  ProfileStore store(t.file());
  Profile wild = game("a", "x.exe", 99999);
  wild.contrast = -40;
  store.upsert(wild);
  CHECK_EQ(store.byId("a")->saturation, kMaxSaturation);
  CHECK_EQ(store.byId("a")->contrast, kMinContrast);
}

TEST(windows_profile_cannot_be_removed) {
  TempDir t;
  ProfileStore store(t.file());
  CHECK(!store.remove(kWindowsProfileId));
  CHECK(store.byId(kWindowsProfileId) != nullptr);
}

TEST(remove_deletes_a_profile_and_reports_unknown_ids) {
  TempDir t;
  ProfileStore store(t.file());
  store.upsert(game("a", "x.exe"));
  CHECK(store.remove("a"));
  CHECK(store.byId("a") == nullptr);
  CHECK(!store.remove("a"));
  ProfileStore reloaded(t.file());
  CHECK(reloaded.byId("a") == nullptr);
}

TEST(forExe_matches_ignoring_case) {
  TempDir t;
  ProfileStore store(t.file());
  store.upsert(game("a", "CS2.exe"));
  CHECK(store.forExe("cs2.exe").id == "a");
  CHECK(store.forExe("CS2.EXE").id == "a");
}

TEST(forExe_falls_back_to_windows_for_unknown_apps) {
  TempDir t;
  ProfileStore store(t.file());
  store.upsert(game("a", "cs2.exe"));
  CHECK(store.forExe("notepad.exe").id == kWindowsProfileId);
}

TEST(forExe_ignores_disabled_profiles) {
  TempDir t;
  ProfileStore store(t.file());
  Profile p = game("a", "cs2.exe");
  p.enabled = false;
  store.upsert(p);
  CHECK(store.forExe("cs2.exe").id == kWindowsProfileId);
}

TEST(missing_windows_profile_is_recreated_on_load) {
  TempDir t;
  writeFile(t.file(), R"([{"id":"a","name":"A","exe":"a.exe","enabled":true,"saturation":150,"contrast":50}])");
  ProfileStore store(t.file());
  CHECK(store.byId(kWindowsProfileId) != nullptr);
  CHECK(store.byId("a") != nullptr);
}

TEST(old_files_with_unknown_keys_still_load) {
  TempDir t;
  writeFile(t.file(), R"([{"id":"win","name":"Windows","exe":"","enabled":true,"vibrance":70,"sharpness":30,"saturation":120,"contrast":55}])");
  ProfileStore store(t.file());
  CHECK_EQ(store.windowsProfile().saturation, 120);
  CHECK_EQ(store.windowsProfile().contrast, 55);
}

TEST(a_corrupt_file_is_quarantined_not_overwritten) {
  TempDir t;
  writeFile(t.file(), "{ this is not json");
  {
    ProfileStore store(t.file());
    CHECK_EQ(store.all().size(), size_t{1});  // starts clean
    store.upsert(game("a", "x.exe"));         // and saving must not destroy the evidence
  }
  CHECK(fs::exists(t.dir / "profiles.json.corrupt"));
}

TEST(saving_leaves_no_temporary_file_behind) {
  TempDir t;
  ProfileStore store(t.file());
  store.upsert(game("a", "x.exe"));
  CHECK(fs::exists(t.file()));
  CHECK(!fs::exists(t.dir / "profiles.json.tmp"));
}
