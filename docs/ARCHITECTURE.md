# Architecture

Saturei is a native Windows app (C++20) that renders its interface in a WebView2 control (React + TypeScript).
The native side owns everything that touches the system; the web side only draws UI and sends intents.

```
┌──────────────────────────── Saturei.exe ────────────────────────────┐
│  platform/MainWindow ── frameless Win32 window (+ DWM theming)       │
│  platform/WebViewHost ─ WebView2 ◄──── JSON messages ────► ui/ (React)│
│            │                                                         │
│  app/Application ────── routes messages, follows the focused app     │
│       │             │                                                │
│  core/ProfileStore   gpu/GpuManager ─► NvAPI │ Magnification │ Gamma │
└──────────────────────────────────────────────────────────────────────┘
```

## Layout

| Path | Responsibility |
|---|---|
| `src/main.cpp` | Entry point: DPI awareness, COM, runs `Application`. |
| `src/app/` | `Application`: composition root. Owns the other objects, handles UI messages, switches profiles. |
| `src/core/` | **Pure logic, no Windows UI or GPU calls.** `Profile`, `ProfileStore`, value limits. Unit-tested. |
| `src/gpu/` | Color providers behind `IGpuProvider`, plus `ColorMath.h` (pure, unit-tested) and `GpuManager` (picks a provider). |
| `src/platform/` | Thin Win32/WebView2 wrappers: `MainWindow`, `WebViewHost`, `Processes`, `Text`, `Log`, `Theme`. |
| `ui/` | Vite + React + TypeScript + Tailwind + shadcn/ui. Built to `ui/dist`, copied next to the exe as `ui/`. |
| `tests/` | Unit tests for `core/` and `gpu/ColorMath.h` (tiny in-repo harness, run with CTest). |

Dependencies point inward: `app → platform, gpu, core`; `gpu → core`; `core → nothing`.

## Color providers

`GpuManager` picks one provider at startup, in this order:

1. **`NvidiaProvider`** (NvAPI digital vibrance). Works in exclusive fullscreen. *Experimental*: the DVC functions are not in
   the public SDK and the code has not been verified on real NVIDIA hardware. Any failure makes it unavailable.
2. **`MagnificationProvider`**: a 5x5 color matrix applied to the whole desktop by the compositor. Any GPU. Does not affect
   games in true exclusive fullscreen.
3. **`GammaRampProvider`**: contrast only (a gamma ramp cannot saturate).

The choice and any NvAPI failure reason are written to `%LOCALAPPDATA%\Saturei\saturei.log`.

To add a provider (e.g. AMD ADL, Intel IGCL): implement `IGpuProvider`, add it to the chain in `GpuManager.cpp`, and put any
math in `ColorMath.h` with tests.

## Profiles

`ProfileStore` persists profiles to `%APPDATA%\Saturei\profiles.json`. Invariants, all covered by tests:

- the **Windows profile always exists** and cannot be deleted;
- numeric fields are **clamped** to their valid range (`core/Limits.h`; mirrored in `ui/src/constants.ts`);
- saves are **atomic** (temp file + rename);
- a file that cannot be parsed is **moved to `profiles.json.corrupt`**, never silently overwritten;
- unknown keys from older versions are ignored.

`Application` polls the foreground window every 500 ms and activates the profile whose `exe` matches (case-insensitive),
falling back to the Windows profile. While Saturei itself is focused the current profile is kept, so editing works.

## UI ⇄ host protocol

JSON messages through WebView2 `postMessage`. Types live in `ui/src/lib/bridge.ts`; handlers in `Application.cpp`.

**UI → host**

| `type` | Fields | Effect |
|---|---|---|
| `ready` | | Host replies with `init`. |
| `setProfile` | `profile` | Create or update, persist, and activate it (live preview). |
| `select` | `id` | Activate the profile (preview). |
| `deleteProfile` | `id` | Delete (not the Windows profile), then reply with `init`. |
| `listProcesses` | | Host replies with `processes`. |
| `window` | `action`: `minimize` \| `maximize` \| `close` | Window control (the title bar is drawn by the UI). |

**Host → UI**

| `type` | Fields |
|---|---|
| `init` | `profiles`, `activeId` |
| `active` | `id` |
| `processes` | `list`: `[{ exe, title }]` |
| `maximized` | `value` |

Malformed messages are logged and ignored; they never crash the host.

## Frameless window

`MainWindow` keeps the native frame (resize borders, shadow, rounded corners) but removes the title bar in `WM_NCCALCSIZE`.
The UI marks its title bar with CSS `app-region: drag` (enabled through WebView2 non-client region support); interactive
elements use `app-no-drag`.

## Web UI

```
ui/src/
  App.tsx            composition only
  components/        TitleBar, Sidebar, ProfilePanel, dialogs, ... (components/ui = shadcn, not edited by hand)
  hooks/             useProfiles (host sync + actions), useSidebarCollapsed, useMaximized
  lib/bridge.ts      typed protocol + send/subscribe
  lib/devHost.ts     simulated host so `npm run dev` works in a plain browser
  lib/i18n/          pt / en / es dictionaries, provider and hook
  constants.ts       value ranges (mirror of src/core/Limits.h)
```

**Adding a language:** add a file next to `pt.tsx` implementing `Messages`, then register it in `lib/i18n/languages.ts`.
The compiler flags every missing key.

## Develop

```bash
cd ui && npm ci && npm run dev        # UI in the browser with the simulated host
cd ui && npm run lint && npm run typecheck

cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

The CMake project refuses non-MSVC compilers on purpose (it depends on the Windows SDK and links the static CRT).
