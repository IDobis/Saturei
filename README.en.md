<div align="center">

<img src="ui/public/logo.svg" alt="Saturei" width="80" />

# Saturei

**Color saturation for Windows and games. On any graphics card.**

[🇧🇷 Português](README.md) · 🇺🇸 English · [🇪🇸 Español](README.es.md)

</div>

---

## What it is

Saturei makes your screen colors more vivid (saturation from **0% to 300%**) and adjusts contrast, with **per-game profiles** that switch automatically when the game is in focus.

It was made for people who want saturation, like vibranceGUI offers, but **don't have an NVIDIA card**. It works with **Intel, AMD and NVIDIA**.

## How it works

| Your GPU | Method | Status |
|---|---|---|
| **Intel / AMD** | Windows color matrix applied to the whole screen | Tested (Intel Iris Xe) |
| **NVIDIA** | Driver digital vibrance (NvAPI), works even in exclusive fullscreen | ⚠️ Experimental, not yet tested on a real NVIDIA card |

## How to use

1. Download the `.zip` from the **Releases** tab and extract the whole folder (don't run it from inside the zip).
2. Open `Saturei.exe`. Keep the `ui` folder next to it.
3. Pick your language with the 🌐 button at the top (Português, English or Español). Drag **Saturation** and **Contrast**. The change shows up instantly.
4. Click **New profile**, pick the game (or type its `.exe`) and adjust. The profile applies by itself when the game is in focus and goes away when you switch apps.

When you close Saturei, the screen goes back to normal.

> **Windows warning:** the app is not code-signed, so SmartScreen may show "Windows protected your PC". Click **More info → Run anyway**.

## Requirements

- Windows 10 or 11 (64-bit)
- [WebView2 Runtime](https://developer.microsoft.com/microsoft-edge/webview2/) (already included in Windows 11)

## Limitations

- Games in **exclusive fullscreen** may ignore the effect on Intel and AMD cards. Use **borderless windowed** mode.
- Saturei **does not inject anything into games**, but there is no guarantee about how each anti-cheat reacts to programs that change screen color. Use at your own risk.

## Build from source

You need: Windows, [Visual Studio Build Tools](https://visualstudio.microsoft.com/downloads/) with the **C++** workload, [CMake](https://cmake.org/) and [Node.js](https://nodejs.org/).

```bash
# 1. Interface
cd ui
npm install
npm run build
cd ..

# 2. Application
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release

# 3. Tests (optional)
ctest --test-dir build -C Release
```

Replace `Visual Studio 17 2022` with the version you have installed. The result is `build\Release\Saturei.exe` (the `ui` folder is copied next to it). See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for how the code is organized.

## Data and uninstalling

Profiles are stored in `%APPDATA%\Saturei\profiles.json`. To uninstall, delete the app folder and, if you want, `%APPDATA%\Saturei` and `%LOCALAPPDATA%\Saturei`.

## Troubleshooting

Saturei records which color method is in use in `%LOCALAPPDATA%\Saturei\saturei.log`. It helps diagnose problems, especially on NVIDIA cards.

## License

[MIT](LICENSE). Use, copy and modify freely.

## Credits

Inspired by [vibranceGUI](https://vibrancegui.com/). Not affiliated with it. The NVIDIA part builds on community reverse-engineering work in [jNizM/NVIDIA_NvAPI](https://github.com/jNizM/NVIDIA_NvAPI) and [Blazzer10200/exfil](https://github.com/Blazzer10200/exfil).
