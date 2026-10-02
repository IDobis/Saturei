#include "platform/WebViewHost.h"
#include <shellapi.h>
#include <cstdlib>
#include <memory>
#include <string>
#include "platform/Log.h"
#include "platform/Text.h"
#include "platform/Theme.h"

using Microsoft::WRL::Callback;
using Microsoft::WRL::ComPtr;

namespace saturei {

namespace {

constexpr wchar_t kVirtualHost[] = L"saturei.local";
constexpr wchar_t kStartUrl[] = L"https://saturei.local/index.html";
constexpr wchar_t kRuntimeDownloadUrl[] = L"https://go.microsoft.com/fwlink/p/?LinkId=2124703";

std::filesystem::path userDataFolder() {
  char* raw = nullptr;
  size_t len = 0;
  _dupenv_s(&raw, &len, "LOCALAPPDATA");
  const std::unique_ptr<char, decltype(&std::free)> localAppData(raw, &std::free);
  const std::filesystem::path base = localAppData ? std::filesystem::path(localAppData.get()) : std::filesystem::temp_directory_path();
  return base / "Saturei" / "webview";
}

const wchar_t* runtimeMissingMessage() {
  switch (PRIMARYLANGID(GetUserDefaultUILanguage())) {
    case LANG_PORTUGUESE:
      return L"O Saturei precisa do WebView2 Runtime da Microsoft, que não foi encontrado.\n\nClique em OK para abrir a página de download.";
    case LANG_SPANISH:
      return L"Saturei necesita el WebView2 Runtime de Microsoft, que no se encontró.\n\nHaz clic en Aceptar para abrir la página de descarga.";
    default:
      return L"Saturei needs the Microsoft WebView2 Runtime, which was not found.\n\nClick OK to open the download page.";
  }
}

}  // namespace

WebViewHost::WebViewHost(HWND parent, std::filesystem::path uiDirectory, MessageHandler onMessage)
    : parent_(parent), uiDirectory_(std::move(uiDirectory)), onMessage_(std::move(onMessage)) {
  createEnvironment();
}

void WebViewHost::resize() {
  if (!controller_) return;
  RECT bounds;
  GetClientRect(parent_, &bounds);
  controller_->put_Bounds(bounds);
}

void WebViewHost::post(const nlohmann::json& message) {
  if (webview_) webview_->PostWebMessageAsJson(text::widen(message.dump()).c_str());
}

void WebViewHost::createEnvironment() {
  const std::wstring dataFolder = userDataFolder().wstring();
  const HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(
      nullptr, dataFolder.c_str(), nullptr,
      Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
          [this](HRESULT result, ICoreWebView2Environment* environment) -> HRESULT {
            onEnvironmentReady(result, environment);
            return S_OK;
          }).Get());
  if (FAILED(hr)) reportRuntimeUnavailable();
}

void WebViewHost::onEnvironmentReady(HRESULT result, ICoreWebView2Environment* environment) {
  if (FAILED(result) || !environment) {
    reportRuntimeUnavailable();
    return;
  }
  environment->CreateCoreWebView2Controller(
      parent_, Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                   [this](HRESULT hr, ICoreWebView2Controller* controller) -> HRESULT {
                     onControllerReady(hr, controller);
                     return S_OK;
                   }).Get());
}

void WebViewHost::onControllerReady(HRESULT result, ICoreWebView2Controller* controller) {
  if (FAILED(result) || !controller) {
    log::info("webview: controller creation failed");
    return;
  }
  controller_ = controller;
  controller_->get_CoreWebView2(&webview_);
  configure();
  resize();
  webview_->Navigate(kStartUrl);
}

void WebViewHost::configure() {
  // Same color as the window: no white flash while the page loads.
  ComPtr<ICoreWebView2Controller2> controller2;
  if (SUCCEEDED(controller_.As(&controller2)))
    controller2->put_DefaultBackgroundColor(COREWEBVIEW2_COLOR{255, theme::kBackgroundR, theme::kBackgroundG, theme::kBackgroundB});

  // Lets the page mark its own title bar as draggable (CSS `app-region: drag`).
  ComPtr<ICoreWebView2Settings> settings;
  ComPtr<ICoreWebView2Settings9> settings9;
  if (SUCCEEDED(webview_->get_Settings(&settings)) && SUCCEEDED(settings.As(&settings9)))
    settings9->put_IsNonClientRegionSupportEnabled(TRUE);

  ComPtr<ICoreWebView2_3> webview3;
  if (SUCCEEDED(webview_.As(&webview3)))
    webview3->SetVirtualHostNameToFolderMapping(kVirtualHost, uiDirectory_.c_str(), COREWEBVIEW2_HOST_RESOURCE_ACCESS_KIND_ALLOW);

  EventRegistrationToken token;
  webview_->add_WebMessageReceived(
      Callback<ICoreWebView2WebMessageReceivedEventHandler>(
          [this](ICoreWebView2*, ICoreWebView2WebMessageReceivedEventArgs* args) -> HRESULT {
            LPWSTR raw = nullptr;
            if (SUCCEEDED(args->get_WebMessageAsJson(&raw)) && raw) {
              const std::string payload = text::narrow(raw);
              CoTaskMemFree(raw);
              const auto json = nlohmann::json::parse(payload, nullptr, /*allow_exceptions=*/false);
              if (json.is_object() && onMessage_) onMessage_(json);
            }
            return S_OK;
          }).Get(),
      &token);
}

void WebViewHost::reportRuntimeUnavailable() const {
  log::info("webview: runtime unavailable");
  if (MessageBoxW(parent_, runtimeMissingMessage(), L"Saturei", MB_OKCANCEL | MB_ICONWARNING) == IDOK)
    ShellExecuteW(nullptr, L"open", kRuntimeDownloadUrl, nullptr, nullptr, SW_SHOWNORMAL);
}

}  // namespace saturei
