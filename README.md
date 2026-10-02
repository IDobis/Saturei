<div align="center">

<img src="ui/public/logo.svg" alt="Saturei" width="80" />

# Saturei

**Saturação de cor para o Windows e para jogos. Em qualquer placa de vídeo.**

🇧🇷 Português · [🇺🇸 English](README.en.md) · [🇪🇸 Español](README.es.md)

</div>

---

## O que é

O Saturei deixa as cores da tela mais vivas (saturação de **0% a 300%**) e ajusta o contraste, com **perfis por jogo** que trocam sozinhos quando o jogo está em foco.

Ele nasceu para quem quer usar saturação, como no vibranceGUI, mas **não tem placa NVIDIA**. Funciona com **Intel, AMD e NVIDIA**.

## Como funciona

| Sua placa | Método | Status |
|---|---|---|
| **Intel / AMD** | Matriz de cor do Windows aplicada na tela inteira | Testado (Intel Iris Xe) |
| **NVIDIA** | Vibrância digital do driver (NvAPI), funciona até em tela cheia exclusiva | ⚠️ Experimental, ainda não testado numa placa NVIDIA real |

## Como usar

1. Baixe o `.zip` na aba **Releases** e extraia a pasta inteira (não rode de dentro do zip).
2. Abra o `Saturei.exe`. Mantenha a pasta `ui` ao lado dele.
3. Escolha o idioma no botão 🌐 no topo (Português, English ou Español). Arraste **Saturação** e **Contraste**. A mudança aparece na hora.
4. Clique em **Novo perfil**, escolha o jogo (ou digite o `.exe`) e ajuste. O perfil entra sozinho quando o jogo estiver em foco e sai quando você trocar de app.

Ao fechar o Saturei, a tela volta ao normal.

> **Aviso do Windows:** o app não é assinado, então o SmartScreen pode mostrar "O Windows protegeu seu computador". Clique em **Mais informações → Executar assim mesmo**.

## Requisitos

- Windows 10 ou 11 (64 bits)
- [WebView2 Runtime](https://developer.microsoft.com/microsoft-edge/webview2/) (já vem no Windows 11)

## Limitações

- Jogos em **tela cheia exclusiva** podem ignorar o efeito nas placas Intel e AMD. Use o modo **janela sem borda**.
- O Saturei **não injeta nada nos jogos**, mas não há garantia sobre como cada anti-cheat reage a programas que alteram a cor da tela. Use por sua conta e risco.

## Compilar do código-fonte

Precisa de: Windows, [Visual Studio Build Tools](https://visualstudio.microsoft.com/downloads/) com a carga de trabalho **C++**, [CMake](https://cmake.org/) e [Node.js](https://nodejs.org/).

```bash
# 1. Interface
cd ui
npm install
npm run build
cd ..

# 2. Aplicativo
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release

# 3. Testes (opcional)
ctest --test-dir build -C Release
```

Troque `Visual Studio 17 2022` pela versão que você tem instalada. O resultado fica em `build\Release\Saturei.exe` (a pasta `ui` é copiada junto). Detalhes da estrutura do código em [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md).

## Dados e desinstalação

Os perfis ficam em `%APPDATA%\Saturei\profiles.json`. Para desinstalar, apague a pasta do app e, se quiser, `%APPDATA%\Saturei` e `%LOCALAPPDATA%\Saturei`.

## Problemas?

O Saturei registra qual método de cor está em uso em `%LOCALAPPDATA%\Saturei\saturei.log`. Ajuda a diagnosticar problemas, especialmente com placas NVIDIA.

## Licença

[MIT](LICENSE). Use, copie e modifique à vontade.

## Créditos

Inspirado no [vibranceGUI](https://vibrancegui.com/). Não tem ligação com ele. A parte NVIDIA se apoia no trabalho de engenharia reversa da comunidade em [jNizM/NVIDIA_NvAPI](https://github.com/jNizM/NVIDIA_NvAPI) e [Blazzer10200/exfil](https://github.com/Blazzer10200/exfil).
