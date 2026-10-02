#pragma once
#include <windows.h>
#include <string>
#include <nlohmann/json.hpp>
#include "core/Profile.h"
#include "core/ProfileStore.h"
#include "gpu/GpuManager.h"
#include "platform/MainWindow.h"
#include "platform/WebViewHost.h"

namespace saturei {

// Composition root: owns the profile store, the GPU color provider, the native window and the web UI,
// and routes messages between them. See docs/ARCHITECTURE.md for the message protocol.
class Application {
 public:
  explicit Application(HINSTANCE instance);
  Application(const Application&) = delete;
  Application& operator=(const Application&) = delete;

  // Runs the message loop; returns the process exit code.
  int run(int showCommand);

 private:
  // UI -> host messages
  void onUiMessage(const nlohmann::json& message);
  void onReady();
  void onSetProfile(const nlohmann::json& message);
  void onSelect(const nlohmann::json& message);
  void onDeleteProfile(const nlohmann::json& message);
  void onWindowCommand(const nlohmann::json& message);
  void onListProcesses();

  // Host -> UI messages
  void sendInit();
  void sendMaximized();

  // Switches to a profile: applies its colors and tells the UI which one is active.
  void activate(const Profile& profile);
  // Follows the focused app: activates the profile matching its executable.
  void followForegroundApp();

  ProfileStore store_;
  GpuManager gpu_;
  MainWindow window_;
  WebViewHost web_;
  std::string activeId_;
};

}  // namespace saturei
