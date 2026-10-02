import type { Messages } from './types'

export const en: Messages = {
  newProfile: 'New profile',
  newProfileDesc: "Pick an open app or type the game's executable name.",
  noApps: 'No apps found.',
  executable: 'Executable',
  name: 'Name',
  windowsProfile: 'Windows',
  defaultTag: '(default)',
  deleteProfile: 'Delete profile',
  confirmDeleteTitle: 'Delete profile?',
  confirmDeleteDesc: (profileName) => (
    <>The profile <b>{profileName}</b> will be deleted. This cannot be undone.</>
  ),
  hint: (exe) => (
    <>The profile applies automatically when the focused app is <b>{exe}</b>. Selecting a profile here shows the effect instantly.</>
  ),
  anyOtherApp: 'any other',
  saturation: 'Saturation',
  contrast: 'Contrast',
  resetFilters: 'Reset filters',
  on: 'On',
  off: 'Off',
  cancel: 'Cancel',
  create: 'Create',
  delete: 'Delete',
  minimize: 'Minimize',
  maximize: 'Maximize',
  restore: 'Restore',
  close: 'Close',
  collapseSidebar: 'Collapse sidebar',
  expandSidebar: 'Expand sidebar',
  language: 'Language',
}
