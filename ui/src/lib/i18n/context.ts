import { createContext } from 'react'
import { en } from './en'
import type { Lang, Messages } from './types'

export type I18nValue = { lang: Lang; setLang: (lang: Lang) => void; t: Messages }

export const I18nContext = createContext<I18nValue>({ lang: 'en', setLang: () => {}, t: en })
