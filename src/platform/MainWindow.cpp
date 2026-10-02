#include "platform/MainWindow.h"
#include <dwmapi.h>
#include "platform/Theme.h"

namespace saturei {

namespace {

constexpr wchar_t kClassName[] = L"SatureiMainWindow";
constexpr wchar_t kTitle[] = L"Saturei";
constexpr int kInitialWidth = 960;
constexpr int kInitialHeight = 640;
constexpr int kAppIconResourceId = 101;  // see src/app.rc
constexpr UINT_PTR kTickTimerId = 1;

// DWM window attributes (numeric values keep the build independent of the SDK version).
constexpr DWORD kDwmUseImmersiveDarkMode = 20;
constexpr DWORD kDwmBorderColor = 34;   // Windows 11+
constexpr DWORD kDwmCaptionColor = 35;  // Windows 11+

void registerWindowClass(HINSTANCE instance, WNDPROC proc) {
  WNDCLASSW wc{};
  wc.lpfnWndProc = proc;
  wc.hInstance = instance;
  wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
  wc.hIcon = LoadIconW(instance, MAKEINTRESOURCEW(kAppIconResourceId));
  wc.hbrBackground = CreateSolidBrush(theme::kBackground);
  wc.lpszClassName = kClassName;
  RegisterClassW(&wc);
}

// Paints the DWM-owned frame parts (border, caption) with the app palette instead of the system accent.
void applyDwmTheme(HWND hwnd) {
  const BOOL dark = TRUE;
  const COLORREF background = theme::kBackground;
  const COLORREF border = theme::kBorder;
  DwmSetWindowAttribute(hwnd, kDwmUseImmersiveDarkMode, &dark, sizeof(dark));
  DwmSetWindowAttribute(hwnd, kDwmCaptionColor, &background, sizeof(background));
  DwmSetWindowAttribute(hwnd, kDwmBorderColor, &border, sizeof(border));
}

}  // namespace

MainWindow::MainWindow(HINSTANCE instance) {
  registerWindowClass(instance, &MainWindow::wndProc);
  hwnd_ = CreateWindowExW(0, kClassName, kTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, kInitialWidth,
                          kInitialHeight, nullptr, nullptr, instance, this);
  if (!hwnd_) return;
  applyDwmTheme(hwnd_);
  // Re-run WM_NCCALCSIZE so the custom (frameless) client area takes effect.
  SetWindowPos(hwnd_, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

MainWindow::~MainWindow() {
  events_ = {};  // the owner is being destroyed; do not call back into it
  if (hwnd_ && IsWindow(hwnd_)) DestroyWindow(hwnd_);
}

bool MainWindow::isMaximized() const { return IsZoomed(hwnd_) != FALSE; }

void MainWindow::show(int showCommand) { ShowWindow(hwnd_, showCommand); }

void MainWindow::startTick(UINT intervalMs) { SetTimer(hwnd_, kTickTimerId, intervalMs, nullptr); }

void MainWindow::execute(WindowCommand command) {
  switch (command) {
    case WindowCommand::Minimize: ShowWindow(hwnd_, SW_MINIMIZE); break;
    case WindowCommand::ToggleMaximize: ShowWindow(hwnd_, isMaximized() ? SW_RESTORE : SW_MAXIMIZE); break;
    case WindowCommand::Close: PostMessageW(hwnd_, WM_CLOSE, 0, 0); break;
  }
}

LRESULT CALLBACK MainWindow::wndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
  MainWindow* self = nullptr;
  if (msg == WM_NCCREATE) {
    self = static_cast<MainWindow*>(reinterpret_cast<CREATESTRUCTW*>(lp)->lpCreateParams);
    self->hwnd_ = hwnd;
    SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
  } else {
    self = reinterpret_cast<MainWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
  }
  const LRESULT result = self ? self->handleMessage(msg, wp, lp) : DefWindowProcW(hwnd, msg, wp, lp);
  if (msg == WM_NCDESTROY && self) {  // last message: detach only after it has been handled
    SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
    self->hwnd_ = nullptr;
  }
  return result;
}

LRESULT MainWindow::handleMessage(UINT msg, WPARAM wp, LPARAM lp) {
  switch (msg) {
    case WM_NCCALCSIZE:
      if (wp) return onNcCalcSize(wp, lp);
      break;
    case WM_SIZE:
      if (events_.onResized) events_.onResized();
      return 0;
    case WM_TIMER:
      if (wp == kTickTimerId && events_.onTick) events_.onTick();
      return 0;
    case WM_DESTROY:
      KillTimer(hwnd_, kTickTimerId);
      if (events_.onDestroyed) events_.onDestroyed();
      PostQuitMessage(0);
      return 0;
  }
  return DefWindowProcW(hwnd_, msg, wp, lp);
}

// Removes the native title bar while keeping the resize borders and the shadow.
LRESULT MainWindow::onNcCalcSize(WPARAM wp, LPARAM lp) {
  auto* params = reinterpret_cast<NCCALCSIZE_PARAMS*>(lp);
  const LONG proposedTop = params->rgrc[0].top;
  DefWindowProcW(hwnd_, WM_NCCALCSIZE, wp, lp);

  if (isMaximized()) {
    // A maximized window extends past the monitor by the border thickness; compensate at the top.
    const UINT dpi = GetDpiForWindow(hwnd_);
    params->rgrc[0].top = proposedTop + GetSystemMetricsForDpi(SM_CYSIZEFRAME, dpi) + GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
  } else {
    // Content starts exactly at the top edge: nothing is drawn above the web title bar.
    params->rgrc[0].top = proposedTop;
  }
  return 0;
}

}  // namespace saturei
