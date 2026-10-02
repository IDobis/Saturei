// Mirrors src/core/Limits.h and src/core/Profile.h in the native host.

export const WINDOWS_PROFILE_ID = 'win'

export const SATURATION = { min: 0, neutral: 100, max: 300 } as const
export const CONTRAST = { min: 0, neutral: 50, max: 100 } as const

export const SIDEBAR_WIDTH = { collapsed: 60, expanded: 256 } as const
