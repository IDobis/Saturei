cmake_minimum_required(VERSION 3.24)
project(Saturei VERSION 0.1.0 LANGUAGES CXX RC)

if(NOT MSVC)
  message(FATAL_ERROR
    "Saturei must be built with MSVC (Visual Studio / Build Tools). Configure with a Visual Studio generator, e.g.:\n"
    "  cmake -S . -B build -G \"Visual Studio 17 2022\" -A x64")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# Static CRT: the app runs without the Visual C++ Redistributable installed.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")

option(SATUREI_BUILD_TESTS "Build the unit tests" ON)

include(FetchContent)

# ---- Dependencies ---------------------------------------------------------------------------------------------------

# WebView2 SDK (a NuGet .nupkg is a zip)
set(WEBVIEW2_VERSION 1.0.2903.40)
set(WEBVIEW2_DIR ${CMAKE_BINARY_DIR}/_deps/webview2)
if(NOT EXISTS ${WEBVIEW2_DIR}/build/native/include/WebView2.h)
  file(DOWNLOAD https://www.nuget.org/api/v2/package/Microsoft.Web.WebView2/${WEBVIEW2_VERSION}
       ${CMAKE_BINARY_DIR}/webview2.zip SHOW_PROGRESS)
  file(ARCHIVE_EXTRACT INPUT ${CMAKE_BINARY_DIR}/webview2.zip DESTINATION ${WEBVIEW2_DIR})
endif()

# JSON
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

# ---- Core library: pure logic (profiles, color math). No UI, no GPU -> unit-testable ---------------------------------

add_library(saturei_core STATIC src/core/ProfileStore.cpp)
target_include_directories(saturei_core PUBLIC src)
target_link_libraries(saturei_core PUBLIC nlohmann_json::nlohmann_json)
target_compile_definitions(saturei_core PUBLIC NOMINMAX WIN32_LEAN_AND_MEAN)

# ---- Application ----------------------------------------------------------------------------------------------------

add_executable(Saturei WIN32
  src/main.cpp
  src/app.rc
  src/app/Application.cpp
  src/gpu/GammaRampProvider.cpp
  src/gpu/GpuManager.cpp
  src/gpu/MagnificationProvider.cpp
  src/gpu/NvidiaProvider.cpp
  src/platform/Log.cpp
  src/platform/MainWindow.cpp
  src/platform/Processes.cpp
  src/platform/Text.cpp
  src/platform/WebViewHost.cpp
)
target_include_directories(Saturei PRIVATE ${WEBVIEW2_DIR}/build/native/include)
target_link_libraries(Saturei PRIVATE saturei_core
  ${WEBVIEW2_DIR}/build/native/x64/WebView2LoaderStatic.lib
  user32 gdi32 shell32 ole32 dxgi Magnification dwmapi)
target_compile_definitions(Saturei PRIVATE UNICODE _UNICODE)

if(MSVC)
  foreach(target saturei_core Saturei)
    target_compile_options(${target} PRIVATE /W4 /permissive- /utf-8)
  endforeach()
endif()

# The web UI (built into ui/dist) is served from a `ui` folder next to the executable.
# The folder is replaced, not merged, so stale bundles from older builds never linger.
add_custom_command(TARGET Saturei POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E rm -rf $<TARGET_FILE_DIR:Saturei>/ui
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/ui/dist $<TARGET_FILE_DIR:Saturei>/ui
  COMMENT "Copying web UI next to the executable")

# ---- Tests ----------------------------------------------------------------------------------------------------------

if(SATUREI_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
