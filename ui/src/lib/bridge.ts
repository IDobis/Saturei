// Typed JSON protocol between this UI and the native host (WebView2). See docs/ARCHITECTURE.md.
// Outside the app (e.g. `npm run dev` in a browser) a simulated host answers instead.

import { createDevHost } from './devHost'

export type Profile = {
  id: string
  name: string
  exe: string // executable file name, e.g. "cs2.exe"; empty for the Windows profile
  enabled: boolean
  saturation: number // 0..300 (%), 100 = unchanged
  contrast: number // 0..100, 50 = unchanged
}

export type HostApp = { exe: string; title: string }
export type WindowAction = 'minimize' | 'maximize' | 'close'

/** UI -> host */
export type ToHost =
  | { type: 'ready' }
  | { type: 'setProfile'; profile: Profile }
  | { type: 'select'; id: string }
  | { type: 'deleteProfile'; id: string }
  | { type: 'listProcesses' }
  | { type: 'window'; action: WindowAction }

/** host -> UI */
export type FromHost =
  | { type: 'init'; profiles: Profile[]; activeId: string | null }
  | { type: 'active'; id: string | null }
  | { type: 'processes'; list: HostApp[] }
  | { type: 'maximized'; value: boolean }

type Listener = (message: FromHost) => void

type WebView = {
  postMessage(message: unknown): void
  addEventListener(type: 'message', handler: (event: { data: FromHost }) => void): void
}

const webview = (window as unknown as { chrome?: { webview?: WebView } }).chrome?.webview
const listeners = new Set<Listener>()

const emit: Listener = (message) => listeners.forEach((listener) => listener(message))
const devHost = webview ? null : createDevHost(emit)

webview?.addEventListener('message', (event) => emit(event.data))

export function send(message: ToHost): void {
  if (webview) webview.postMessage(message)
  else devHost?.handle(message)
}

/** Registers a listener for host messages. Returns the function that removes it. */
export function subscribe(listener: Listener): () => void {
  listeners.add(listener)
  return () => listeners.delete(listener)
}
