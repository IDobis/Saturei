#include <windows.h>
#include <dwmapi.h>
#include <shlwapi.h>
#include <wrl.h>
#include <WebView2.h>
#include <algorithm>
#include <filesystem>
#include <memory>
#include <string>
#include <nlohmann/json.hpp>
#include "core/ProfileStore.h"
#include <set>
#include "gpu/GpuManager.h"

using json = nlohmann::json;
using Microsoft::WRL::Callback;
using Microsoft::WRL::ComPtr;

namespace {

constexpr UINT_PTR kFocusTimer = 1;
constexpr UINT kFocusPollMs = 500;

HWND g_hwnd = nullptr;
ComPtr<ICoreWebView2Controller> g_controller;
ComPtr<ICoreWebView2> g_webview;
std::unique_ptr<ProfileStore> g_store;
std::unique_ptr<GpuManager> g_gpu;
std::string g_activeId;

std::wstring widen(const std::string& s) {
  int n = MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), nullptr, 0);
  std::wstring w(n, L'\0');
  MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), w.data(), n);
  return w;
}

std::string narrow(const std::wstring& w) {
  int n = WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), nullptr, 0, nullptr, nullptr);
  std::string s(n, '\0');
  WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), s.data(), n, nullptr, nullptr);
  return s;
}

void postToUi(const json& j) {
  if (g_webview) g_webview->PostWebMessageAsJson(widen(j.dump()).c_str());
}

const char* vendorName(GpuAdapter::Vendor v) {
  switch (v) {
    case GpuAdapter::Vendor::Nvidia: return "nvidia";
    case GpuAdapter::Vendor::Amd: return "amd";
    case GpuAdapter::Vendor::Intel: return "intel";
    default: return "unknown";
  }
}

void sendInit() {
  json gpus = json::array();
  for (auto& a : g_gpu->adapters())
    gpus.push_back({{"vendor", vendorName(a.vendor)}, {"name", a.name}, {"trueSaturation", g_gpu->trueSaturation()}});
  postToUi({{"type", "init"}, {"gpus", gpus}, {"method", g_gpu->method()}, {"profiles", g_store->all()},
            {"activeId", g_activeId.empty() ? json(nullptr) : json(g_activeId)}});
}

void applyProfile(const Profile& p) {
  if (p.enabled) g_gpu->apply(p.saturation, p.contrast); else g_gpu->reset();
}

void setActive(const Profile& p) {
  g_activeId = p.id;
  applyProfile(p);
  postToUi({{"type", "active"}, {"id", g_activeId}});
}

// Apps com janela visivel (para criar perfil a partir de um app aberto).
json listProcesses() {
  struct Ctx { std::set<std::string> seen; json out = json::array(); } ctx;
  EnumWindows([](HWND h, LPARAM lp) -> BOOL {
    auto& c = *reinterpret_cast<Ctx*>(lp);
    wchar_t title[256];
    if (!IsWindowVisible(h) || GetWindow(h, GW_OWNER) || !GetWindowTextW(h, title, 256)) return TRUE;
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    HANDLE ph = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!ph) return TRUE;
    wchar_t buf[MAX_PATH]; DWORD len = MAX_PATH;
    if (QueryFullProcessImageNameW(ph, 0, buf, &len)) {
      std::string exe = narrow(std::filesystem::path(buf).filename().wstring());
      std::string key = exe;
      std::transform(key.begin(), key.end(), key.begin(), [](unsigned char ch) { return (char)std::tolower(ch); });
      if (key != "saturei.exe" && c.seen.insert(key).second)
        c.out.push_back({{"exe", exe}, {"title", narrow(title)}});
    }
    CloseHandle(ph);
    return TRUE;
  }, reinterpret_cast<LPARAM>(&ctx));
  return ctx.out;
}

// Nome do .exe (minusculo) da janela em primeiro plano.
std::string foregroundExe() {
  HWND fg = GetForegroundWindow();
  DWORD pid = 0;
  if (!fg || !GetWindowThreadProcessId(fg, &pid) || !pid) return {};
  HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
  if (!h) return {};
  wchar_t buf[MAX_PATH]; DWORD len = MAX_PATH;
  std::string out;
  if (QueryFullProcessImageNameW(h, 0, buf, &len)) {
    out = narrow(std::filesystem::path(buf).filename().wstring());
    std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) { return (char)std::tolower(c); });
  }
  CloseHandle(h);
  return out;
}

void pollForeground() {
  std::string exe = foregroundExe();
  if (exe.empty() || exe == "saturei.exe") return;
  const Profile* p = g_store->forExe(exe);
  if (!p || p->id == g_activeId) return;
  setActive(*p);
}

void onUiMessage(const std::string& raw) {
  auto m = json::parse(raw, nullptr, false);
  if (!m.is_object()) return;
  const std::string type = m.value("type", "");
  if (type == "ready") {
    sendInit();
  } else if (type == "setProfile") {
    Profile p = m["profile"].get<Profile>();
    g_store->upsert(p);
    setActive(p);  // preview ao vivo do que esta sendo editado
  } else if (type == "select") {
    if (const Profile* p = g_store->byId(m.value("id", ""))) setActive(*p);
  } else if (type == "deleteProfile") {
    g_store->remove(m.value("id", ""));
    if (!g_store->byId(g_activeId)) setActive(*g_store->byId("win"));
    sendInit();
  } else if (type == "window") {
    const std::string a = m.value("action", "");
    if (a == "minimize") ShowWindow(g_hwnd, SW_MINIMIZE);
    else if (a == "maximize") ShowWindow(g_hwnd, IsZoomed(g_hwnd) ? SW_RESTORE : SW_MAXIMIZE);
    else if (a == "close") PostMessageW(g_hwnd, WM_CLOSE, 0, 0);
  } else if (type == "listProcesses") {
    postToUi({{"type", "processes"}, {"list", listProcesses()}});
  }
}

void initWebView(HWND hwnd) {
  wchar_t exe[MAX_PATH];
  GetModuleFileNameW(nullptr, exe, MAX_PATH);
  std::filesystem::path uiDir = std::filesystem::path(exe).parent_path() / "ui";

  wchar_t tmp[MAX_PATH];
  GetEnvironmentVariableW(L"LOCALAPPDATA", tmp, MAX_PATH);
  std::wstring userData = std::wstring(tmp) + L"\\Saturei\\webview";

  CreateCoreWebView2EnvironmentWithOptions(nullptr, userData.c_str(), nullptr,
    Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
      [hwnd, uiDir](HRESULT, ICoreWebView2Environment* env) -> HRESULT {
        if (!env) return S_OK;
        env->CreateCoreWebView2Controller(hwnd,
          Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
            [hwnd, uiDir](HRESULT, ICoreWebView2Controller* ctrl) -> HRESULT {
              if (!ctrl) return S_OK;
              g_controller = ctrl;
              ComPtr<ICoreWebView2Controller2> ctrl2;
              if (SUCCEEDED(g_controller.As(&ctrl2)))
                ctrl2->put_DefaultBackgroundColor(COREWEBVIEW2_COLOR{255, 10, 10, 10});  // sem flash branco
              g_controller->get_CoreWebView2(&g_webview);

              ComPtr<ICoreWebView2Settings> settings;
              ComPtr<ICoreWebView2Settings9> settings9;
              if (SUCCEEDED(g_webview->get_Settings(&settings)) && SUCCEEDED(settings.As(&settings9)))
                settings9->put_IsNonClientRegionSupportEnabled(TRUE);  // CSS app-region: drag

              ComPtr<ICoreWebView2_3> wv3;
              if (SUCCEEDED(g_webview.As(&wv3)))
                wv3->SetVirtualHostNameToFolderMapping(L"saturei.local", uiDir.c_str(),
                                                       COREWEBVIEW2_HOST_RESOURCE_ACCESS_KIND_ALLOW);

              EventRegistrationToken tok;
              g_webview->add_WebMessageReceived(
                Callback<ICoreWebView2WebMessageReceivedEventHandler>(
                  [](ICoreWebView2*, ICoreWebView2WebMessageReceivedEventArgs* a) -> HRESULT {
                    LPWSTR s = nullptr;
                    if (SUCCEEDED(a->get_WebMessageAsJson(&s)) && s) {
                      onUiMessage(narrow(s));
                      CoTaskMemFree(s);
                    }
                    return S_OK;
                  }).Get(), &tok);

              RECT rc; GetClientRect(hwnd, &rc);
              g_controller->put_Bounds(rc);
              g_webview->Navigate(L"https://saturei.local/index.html");
              return S_OK;
            }).Get());
        return S_OK;
      }).Get());
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
  switch (msg) {
    case WM_NCCALCSIZE:
      // Remove a barra de titulo nativa mantendo bordas de redimensionar e sombra.
      if (wp) {
        auto* p = reinterpret_cast<NCCALCSIZE_PARAMS*>(lp);
        LONG top = p->rgrc[0].top;
        DefWindowProcW(hwnd, msg, wp, lp);
        // Maximizada: a janela extrapola o monitor pela espessura da borda superior.
        // Normal: o conteudo comeca no topo exato, sem faixa nenhuma acima da barra.
        if (IsZoomed(hwnd)) {
          const UINT dpi = GetDpiForWindow(hwnd);
          p->rgrc[0].top = top + GetSystemMetricsForDpi(SM_CYSIZEFRAME, dpi) + GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
        } else {
          p->rgrc[0].top = top;
        }
        return 0;
      }
      break;
    case WM_SIZE:
      if (g_controller) { RECT rc; GetClientRect(hwnd, &rc); g_controller->put_Bounds(rc); }
      postToUi({{"type", "maximized"}, {"value", IsZoomed(hwnd) != FALSE}});
      return 0;
    case WM_TIMER:
      if (wp == kFocusTimer) pollForeground();
      return 0;
    case WM_DESTROY:
      KillTimer(hwnd, kFocusTimer);
      g_gpu->reset();  // nunca deixar a tela alterada ao sair
      PostQuitMessage(0);
      return 0;
  }
  return DefWindowProcW(hwnd, msg, wp, lp);
}

}  // namespace

int WINAPI wWinMain(HINSTANCE inst, HINSTANCE, PWSTR, int show) {
  SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);  // UI nitida em telas com escala
  CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
  g_store = std::make_unique<ProfileStore>();
  g_gpu = std::make_unique<GpuManager>();

  WNDCLASSW wc{};
  wc.lpfnWndProc = WndProc;
  wc.hInstance = inst;
  wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
  wc.hIcon = LoadIconW(inst, MAKEINTRESOURCEW(101));  // icone do app (React)
  wc.hbrBackground = CreateSolidBrush(RGB(10, 10, 10));
  wc.lpszClassName = L"SatureiWnd";
  RegisterClassW(&wc);

  g_hwnd = CreateWindowExW(0, wc.lpszClassName, L"Saturei", WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, CW_USEDEFAULT, 960, 640, nullptr, nullptr, inst, nullptr);
  // Titulo/borda do DWM na cor do app (senao usa a cor de titulo do Windows).
  const BOOL dark = TRUE;
  const COLORREF bg = RGB(10, 10, 10), border = RGB(38, 38, 38);
  DwmSetWindowAttribute(g_hwnd, 20 /*DWMWA_USE_IMMERSIVE_DARK_MODE*/, &dark, sizeof(dark));
  DwmSetWindowAttribute(g_hwnd, 35 /*DWMWA_CAPTION_COLOR*/, &bg, sizeof(bg));
  DwmSetWindowAttribute(g_hwnd, 34 /*DWMWA_BORDER_COLOR*/, &border, sizeof(border));
  SetWindowPos(g_hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
  ShowWindow(g_hwnd, show);
  initWebView(g_hwnd);
  SetTimer(g_hwnd, kFocusTimer, kFocusPollMs, nullptr);

  MSG msg;
  while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
    TranslateMessage(&msg);
    DispatchMessageW(&msg);
  }
  return (int)msg.wParam;
}
