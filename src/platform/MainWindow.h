#pragma once
#include <windows.h>
#include <functional>

namespace saturei {

enum class WindowCommand { Minimize, ToggleMaximize, Close };

// Frameless top-level window: the title bar is drawn by the web UI, while the native frame keeps
// resize borders, the drop shadow and Windows 11 rounded corners.
class MainWindow {
 public:
  struct Events {
    std::function<void()> onTick;       // periodic timer (see startTick)
    std::function<void()> onResized;    // client size or maximized state changed
    std::function<void()> onDestroyed;  // window is going away
  };

  explicit MainWindow(HINSTANCE instance);
  ~MainWindow();
  MainWindow(const MainWindow&) = delete;
  MainWindow& operator=(const MainWindow&) = delete;

  // Callbacks are attached after construction so they can never fire on a half-built owner.
  void setEvents(Events events) { events_ = std::move(events); }

  HWND handle() const { return hwnd_; }
  bool isMaximized() const;
  void show(int showCommand);
  void startTick(UINT intervalMs);
  void execute(WindowCommand command);

 private:
  static LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
  LRESULT handleMessage(UINT msg, WPARAM wp, LPARAM lp);
  LRESULT onNcCalcSize(WPARAM wp, LPARAM lp);

  HWND hwnd_ = nullptr;
  Events events_;
};

}  // namespace saturei
