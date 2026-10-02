<div align="center">

<img src="ui/public/logo.svg" alt="Saturei" width="80" />

# Saturei

**Saturación de color para Windows y juegos. En cualquier tarjeta gráfica.**

[🇧🇷 Português](README.md) · [🇺🇸 English](README.en.md) · 🇪🇸 Español

</div>

---

## Qué es

Saturei hace que los colores de la pantalla se vean más vivos (saturación del **0% al 300%**) y ajusta el contraste, con **perfiles por juego** que cambian solos cuando el juego está en primer plano.

Nació para quienes quieren usar saturación, como en vibranceGUI, pero **no tienen una tarjeta NVIDIA**. Funciona con **Intel, AMD y NVIDIA**.

## Cómo funciona

| Tu tarjeta | Método | Estado |
|---|---|---|
| **Intel / AMD** | Matriz de color de Windows aplicada a toda la pantalla | Probado (Intel Iris Xe) |
| **NVIDIA** | Vibración digital del controlador (NvAPI), funciona incluso en pantalla completa exclusiva | ⚠️ Experimental, aún no probado en una tarjeta NVIDIA real |

## Cómo usar

1. Descarga el `.zip` desde la pestaña **Releases** y extrae la carpeta completa (no lo ejecutes desde dentro del zip).
2. Abre `Saturei.exe`. Mantén la carpeta `ui` junto a él.
3. Elige el idioma con el botón 🌐 de arriba (Português, English o Español). Arrastra **Saturación** y **Contraste**. El cambio se ve al instante.
4. Haz clic en **Nuevo perfil**, elige el juego (o escribe su `.exe`) y ajusta. El perfil se aplica solo cuando el juego está en primer plano y se quita al cambiar de aplicación.

Al cerrar Saturei, la pantalla vuelve a la normalidad.

> **Aviso de Windows:** la aplicación no está firmada, por lo que SmartScreen puede mostrar "Windows protegió su PC". Haz clic en **Más información → Ejecutar de todas formas**.

## Requisitos

- Windows 10 u 11 (64 bits)
- [WebView2 Runtime](https://developer.microsoft.com/microsoft-edge/webview2/) (ya incluido en Windows 11)

## Limitaciones

- Los juegos en **pantalla completa exclusiva** pueden ignorar el efecto en tarjetas Intel y AMD. Usa el modo **ventana sin bordes**.
- Saturei **no inyecta nada en los juegos**, pero no hay garantía de cómo reacciona cada anticheat ante programas que cambian el color de la pantalla. Úsalo bajo tu propio riesgo.

## Compilar desde el código fuente

Necesitas: Windows, [Visual Studio Build Tools](https://visualstudio.microsoft.com/downloads/) con la carga de trabajo **C++**, [CMake](https://cmake.org/) y [Node.js](https://nodejs.org/).

```bash
# 1. Interfaz
cd ui
npm install
npm run build
cd ..

# 2. Aplicación
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release

# 3. Pruebas (opcional)
ctest --test-dir build -C Release
```

Cambia `Visual Studio 17 2022` por la versión que tengas instalada. El resultado queda en `build\Release\Saturei.exe` (la carpeta `ui` se copia junto a él). La estructura del código está explicada en [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md).

## Datos y desinstalación

Los perfiles se guardan en `%APPDATA%\Saturei\profiles.json`. Para desinstalar, borra la carpeta de la aplicación y, si quieres, `%APPDATA%\Saturei` y `%LOCALAPPDATA%\Saturei`.

## ¿Problemas?

Saturei registra qué método de color está en uso en `%LOCALAPPDATA%\Saturei\saturei.log`. Ayuda a diagnosticar problemas, sobre todo con tarjetas NVIDIA.

## Licencia

[MIT](LICENSE). Úsalo, cópialo y modifícalo libremente.

## Créditos

Inspirado en [vibranceGUI](https://vibrancegui.com/). Sin relación con él. La parte de NVIDIA se apoya en el trabajo de ingeniería inversa de la comunidad en [jNizM/NVIDIA_NvAPI](https://github.com/jNizM/NVIDIA_NvAPI) y [Blazzer10200/exfil](https://github.com/Blazzer10200/exfil).
