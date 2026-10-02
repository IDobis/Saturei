#include "app/Application.h"
#include <filesystem>
#include "platform/Log.h"
#include "platform/Processes.h"

using nlohmann::json;

namespace saturei {

namespace {

constexpr UINT kFocusPollIntervalMs = 500;
constexpr const char* kOwnExe = "saturei.exe";

// The web UI is built to a `ui` folder next to the executable.
std::filesystem::path uiDirectory() {
  wchar_t exePath[MAX_PATH];
  GetModuleFileNameW(nullptr, exePath, MAX_PATH);
  return std::filesystem::path(exePath).parent_path() / "ui";
}

}  // namespace

Application::Application(HINSTANCE instance)
    : window_(instance), web_(window_.handle(), uiDirectory(), [this](const json& m) { onUiMessage(m); }) {
  window_.setEvents({
      .onTick = [this] { followForegroundApp(); },
      .onResized = [this] {
        web_.resize();
        sendMaximized();
      },
      .onDestroyed = [this] { gpu_.reset(); },  // never leave the screen altered on exit
  });
}

int Application::run(int showCommand) {
  window_.show(showCommand);
  window_.startTick(kFocusPollIntervalMs);

  MSG msg;
  while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
    TranslateMessage(&msg);
    DispatchMessageW(&msg);
  }
  return static_cast<int>(msg.wParam);
}

// ---- UI -> host ------------------------------------------------------------------------------

void Application::onUiMessage(const json& message) {
  try {
    const std::string type = message.value("type", "");
    if (type == "ready") onReady();
    else if (type == "setProfile") onSetProfile(message);
    else if (type == "select") onSelect(message);
    else if (type == "deleteProfile") onDeleteProfile(message);
    else if (type == "window") onWindowCommand(message);
    else if (type == "listProcesses") onListProcesses();
    else log::info("ignored unknown UI message: " + type);
  } catch (const json::exception& e) {
    log::info(std::string("malformed UI message: ") + e.what());  // a bad message must never crash the app
  }
}

void Application::onReady() { sendInit(); }

void Application::onSetProfile(const json& message) {
  Profile profile = message.at("profile").get<Profile>();
  store_.upsert(profile);
  activate(*store_.byId(profile.id));  // live preview of what is being edited
}

void Application::onSelect(const json& message) {
  if (const Profile* profile = store_.byId(message.value("id", ""))) activate(*profile);
}

void Application::onDeleteProfile(const json& message) {
  store_.remove(message.value("id", ""));
  if (!store_.byId(activeId_)) activate(store_.windowsProfile());
  sendInit();
}

void Application::onWindowCommand(const json& message) {
  const std::string action = message.value("action", "");
  if (action == "minimize") window_.execute(WindowCommand::Minimize);
  else if (action == "maximize") window_.execute(WindowCommand::ToggleMaximize);
  else if (action == "close") window_.execute(WindowCommand::Close);
}

void Application::onListProcesses() {
  json apps = json::array();
  for (const auto& app : processes::listWindowedApps(kOwnExe)) apps.push_back({{"exe", app.exe}, {"title", app.title}});
  web_.post({{"type", "processes"}, {"list", apps}});
}

// ---- host -> UI ------------------------------------------------------------------------------

void Application::sendInit() {
  web_.post({{"type", "init"},
             {"profiles", store_.all()},
             {"activeId", activeId_.empty() ? json(nullptr) : json(activeId_)}});
}

void Application::sendMaximized() { web_.post({{"type", "maximized"}, {"value", window_.isMaximized()}}); }

// ---- behavior --------------------------------------------------------------------------------

void Application::activate(const Profile& profile) {
  activeId_ = profile.id;
  if (profile.enabled) gpu_.apply(profile.saturation, profile.contrast);
  else gpu_.reset();
  web_.post({{"type", "active"}, {"id", activeId_}});
}

void Application::followForegroundApp() {
  const std::string exe = processes::foregroundExe();
  if (exe.empty() || exe == kOwnExe) return;  // keep the current profile while the user is in this app
  const Profile& profile = store_.forExe(exe);
  if (profile.id != activeId_) activate(profile);
}

}  // namespace saturei
