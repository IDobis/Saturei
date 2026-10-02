// Simulated native host for `npm run dev` in a regular browser. Mirrors the real host's behavior
// closely enough to work on the whole UI without building the C++ app.

import { CONTRAST, SATURATION, WINDOWS_PROFILE_ID } from '@/constants'
import type { FromHost, Profile, ToHost } from './bridge'

export function createDevHost(emit: (message: FromHost) => void) {
  let profiles: Profile[] = [
    { id: WINDOWS_PROFILE_ID, name: 'Windows', exe: '', enabled: true, saturation: SATURATION.neutral, contrast: CONTRAST.neutral },
    { id: 'cs2', name: 'Counter-Strike 2', exe: 'cs2.exe', enabled: true, saturation: 160, contrast: 60 },
  ]
  let activeId: string | null = WINDOWS_PROFILE_ID

  const reply = (message: FromHost) => queueMicrotask(() => emit(message))
  const activate = (id: string) => {
    activeId = id
    reply({ type: 'active', id })
  }

  return {
    handle(message: ToHost) {
      switch (message.type) {
        case 'ready':
          reply({ type: 'init', profiles, activeId })
          break
        case 'setProfile': {
          const exists = profiles.some((p) => p.id === message.profile.id)
          profiles = exists ? profiles.map((p) => (p.id === message.profile.id ? message.profile : p)) : [...profiles, message.profile]
          activate(message.profile.id)
          break
        }
        case 'select':
          activate(message.id)
          break
        case 'deleteProfile':
          if (message.id === WINDOWS_PROFILE_ID) break
          profiles = profiles.filter((p) => p.id !== message.id)
          if (activeId === message.id) activeId = WINDOWS_PROFILE_ID
          reply({ type: 'init', profiles, activeId })
          break
        case 'listProcesses':
          reply({ type: 'processes', list: [{ exe: 'cs2.exe', title: 'Counter-Strike 2' }, { exe: 'chrome.exe', title: 'YouTube' }] })
          break
        case 'window':
          break
      }
    },
  }
}
