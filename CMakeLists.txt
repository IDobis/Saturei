cmake_minimum_required(VERSION 3.24)
project(Saturei LANGUAGES CXX RC)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")  # CRT estatica: amigo nao precisa do VC++ Redistributable

include(FetchContent)

# --- WebView2 SDK (NuGet .nupkg = zip) ---
set(WV2_VER 1.0.2903.40)
set(WV2_DIR ${CMAKE_BINARY_DIR}/_deps/webview2)
if(NOT EXISTS ${WV2_DIR}/build/native/include/WebView2.h)
  file(DOWNLOAD https://www.nuget.org/api/v2/package/Microsoft.Web.WebView2/${WV2_VER}
       ${CMAKE_BINARY_DIR}/webview2.zip SHOW_PROGRESS)
  file(ARCHIVE_EXTRACT INPUT ${CMAKE_BINARY_DIR}/webview2.zip DESTINATION ${WV2_DIR})
endif()

# --- JSON ---
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

add_executable(Saturei WIN32
  src/main.cpp
  src/app.rc
  src/core/ProfileStore.cpp
  src/gpu/GammaRampProvider.cpp
  src/gpu/GpuManager.cpp
  src/gpu/MagnificationProvider.cpp
  src/gpu/NvidiaProvider.cpp
)
target_include_directories(Saturei PRIVATE src ${WV2_DIR}/build/native/include)
target_link_libraries(Saturei PRIVATE nlohmann_json::nlohmann_json
  ${WV2_DIR}/build/native/x64/WebView2LoaderStatic.lib
  user32 gdi32 shell32 ole32 shlwapi version dxgi Magnification dwmapi)
target_compile_definitions(Saturei PRIVATE UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN)
if(MSVC)
  target_compile_options(Saturei PRIVATE /W4 /utf-8)
endif()

# UI buildada (ui/dist) ao lado do exe, servida em https://saturei.local/
add_custom_command(TARGET Saturei POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/ui/dist $<TARGET_FILE_DIR:Saturei>/ui)
