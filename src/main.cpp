#include <windows.h>
#include "app/Application.h"

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int showCommand) {
  // Per-monitor DPI awareness: crisp UI on scaled displays.
  SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

  if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED))) return 1;
  int exitCode = 0;
  {
    saturei::Application app(instance);
    exitCode = app.run(showCommand);
  }
  CoUninitialize();
  return exitCode;
}
