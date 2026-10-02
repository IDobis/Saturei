// localStorage can throw or be unavailable (blocked site data, private windows). These helpers
// never throw: a setting that cannot be stored simply does not persist.

export function readSetting(key: string): string | null {
  try {
    return localStorage.getItem(key)
  } catch {
    return null
  }
}

export function writeSetting(key: string, value: string): void {
  try {
    localStorage.setItem(key, value)
  } catch {
    // not persisted
  }
}
