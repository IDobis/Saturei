import type { ReactNode } from 'react'

export type Lang = 'pt' | 'en' | 'es'

/** Every user-visible string. Add a key here and the compiler points at each language missing it. */
export interface Messages {
  // profiles
  newProfile: string
  newProfileDesc: string
  noApps: string
  executable: string
  name: string
  windowsProfile: string
  defaultTag: string
  deleteProfile: string
  confirmDeleteTitle: string
  confirmDeleteDesc(profileName: string): ReactNode
  hint(exe: string): ReactNode
  anyOtherApp: string
  // controls
  saturation: string
  contrast: string
  resetFilters: string
  on: string
  off: string
  // actions
  cancel: string
  create: string
  delete: string
  // window chrome
  minimize: string
  maximize: string
  restore: string
  close: string
  collapseSidebar: string
  expandSidebar: string
  language: string
}
