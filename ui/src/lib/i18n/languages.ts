import { readSetting } from '@/lib/storage'
import { en } from './en'
import { es } from './es'
import { pt } from './pt'
import type { Lang, Messages } from './types'

export const LANGUAGES: { code: Lang; label: string }[] = [
  { code: 'pt', label: 'Português' },
  { code: 'en', label: 'English' },
  { code: 'es', label: 'Español' },
]

export const MESSAGES: Record<Lang, Messages> = { pt, en, es }

export const LANG_STORAGE_KEY = 'lang'

const isLang = (value: string | null): value is Lang => value === 'pt' || value === 'en' || value === 'es'

/** Saved choice first, then the browser/OS language, then English. */
export function detectInitialLang(): Lang {
  const saved = readSetting(LANG_STORAGE_KEY)
  if (isLang(saved)) return saved

  const preferred = (navigator.language || 'en').toLowerCase()
  if (preferred.startsWith('pt')) return 'pt'
  if (preferred.startsWith('es')) return 'es'
  return 'en'
}
