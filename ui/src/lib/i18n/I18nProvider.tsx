import { useEffect, useMemo, useState, type ReactNode } from 'react'
import { writeSetting } from '@/lib/storage'
import { I18nContext } from './context'
import { detectInitialLang, LANG_STORAGE_KEY, MESSAGES } from './languages'
import type { Lang } from './types'

export function I18nProvider({ children }: { children: ReactNode }) {
  const [lang, setLangState] = useState<Lang>(detectInitialLang)

  useEffect(() => {
    document.documentElement.lang = lang === 'pt' ? 'pt-BR' : lang
  }, [lang])

  const value = useMemo(
    () => ({
      lang,
      t: MESSAGES[lang],
      setLang: (next: Lang) => {
        setLangState(next)
        writeSetting(LANG_STORAGE_KEY, next) // only an explicit choice is persisted
      },
    }),
    [lang],
  )

  return <I18nContext.Provider value={value}>{children}</I18nContext.Provider>
}
