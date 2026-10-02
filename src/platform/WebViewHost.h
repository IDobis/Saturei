#pragma once
#include <windows.h>
#include <wrl.h>
#include <WebView2.h>
#include <filesystem>
#include <functional>
#include <nlohmann/json.hpp>

namespace saturei {

// Hosts the WebView2 control that renders the UI (the files in `uiDirectory`, served from the virtual
// host https://saturei.local/). Messages travel as JSON in both directions.
class WebViewHost {
 public:
  using MessageHandler = std::function<void(const nlohmann::json&)>;

  WebViewHost(HWND parent, std::filesystem::path uiDirectory, MessageHandler onMessage);
  WebViewHost(const WebViewHost&) = delete;
  WebViewHost& operator=(const WebViewHost&) = delete;

  // Fits the control to the parent's client area.
  void resize();
  // Sends a message to the UI. Dropped silently until the page is ready.
  void post(const nlohmann::json& message);

 private:
  void createEnvironment();
  void onEnvironmentReady(HRESULT result, ICoreWebView2Environment* environment);
  void onControllerReady(HRESULT result, ICoreWebView2Controller* controller);
  void configure();
  void reportRuntimeUnavailable() const;

  HWND parent_;
  std::filesystem::path uiDirectory_;
  MessageHandler onMessage_;
  Microsoft::WRL::ComPtr<ICoreWebView2Controller> controller_;
  Microsoft::WRL::ComPtr<ICoreWebView2> webview_;
};

}  // namespace saturei
