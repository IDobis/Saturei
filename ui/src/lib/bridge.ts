// Ponte UI <-> core C++ (WebView2). Em `npm run dev` (navegador) cai num mock.
export type GpuInfo = { vendor: 'nvidia' | 'amd' | 'intel' | 'unknown'; name: string; trueSaturation: boolean }

export type Profile = {
  id: string
  name: string
  exe: string // ex.: "cs2.exe"; "" = Windows (perfil padrão)
  enabled: boolean
  saturation: number // 0..300 (%), 100 = neutro
  contrast: number
}

export type ToHost =
  | { type: 'ready' }
  | { type: 'setProfile'; profile: Profile }
  | { type: 'deleteProfile'; id: string }
  | { type: 'select'; id: string }
  | { type: 'listProcesses' }
  | { type: 'window'; action: 'minimize' | 'maximize' | 'close' }

export type FromHost =
  | { type: 'init'; method: 'nvapi' | 'magnification' | 'gamma-ramp'; gpus: GpuInfo[]; profiles: Profile[]; activeId: string | null }
  | { type: 'active'; id: string | null }
  | { type: 'maximized'; value: boolean }
  | { type: 'processes'; list: { exe: string; title: string }[] }

type Wv = { postMessage(m: unknown): void; addEventListener(t: 'message', cb: (e: { data: FromHost }) => void): void }
const wv = (window as unknown as { chrome?: { webview?: Wv } }).chrome?.webview

export const inHost = !!wv

export function send(msg: ToHost) {
  if (wv) wv.postMessage(msg)
  else {
    console.debug('[bridge:mock] ->', msg)
    if (msg.type === 'listProcesses')
      queueMicrotask(() => mockCb?.({ type: 'processes', list: [{ exe: 'cs2.exe', title: 'Counter-Strike 2' }, { exe: 'chrome.exe', title: 'YouTube' }] }))
  }
}

let mockCb: ((m: FromHost) => void) | undefined

export function onHost(cb: (m: FromHost) => void) {
  mockCb = cb
  if (wv) {
    wv.addEventListener('message', (e) => cb(e.data))
    return
  }
  queueMicrotask(() =>
    cb({
      type: 'init',
      method: 'magnification',
      activeId: 'win',
      gpus: [{ vendor: 'intel', name: 'Intel Iris Xe Graphics (mock)', trueSaturation: true }],
      profiles: [
        { id: 'win', name: 'Windows', exe: '', enabled: true, saturation: 100, contrast: 50 },
        { id: 'cs2', name: 'Counter-Strike 2', exe: 'cs2.exe', enabled: true, saturation: 160, contrast: 60 },
      ],
    }),
  )
}
