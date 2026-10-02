import { createContext, useContext, useEffect, useState, type ReactNode } from 'react'

export type Lang = 'pt' | 'en' | 'es'

export const LANGS: { code: Lang; label: string }[] = [
  { code: 'pt', label: 'Português' },
  { code: 'en', label: 'English' },
  { code: 'es', label: 'Español' },
]

const pt = {
  newProfile: 'Novo perfil',
  newProfileDesc: 'Escolha um app aberto ou digite o nome do executável do jogo.',
  noApps: 'Nenhum app encontrado.',
  executable: 'Executável',
  name: 'Nome',
  cancel: 'Cancelar',
  create: 'Criar',
  windowsProfile: 'Windows',
  defaultTag: '(padrão)',
  saturation: 'Saturação',
  contrast: 'Contraste',
  deleteProfile: 'Apagar perfil',
  confirmDeleteTitle: 'Apagar perfil?',
  confirmDeleteDesc: (name: string): ReactNode => (
    <>O perfil <b>{name}</b> será apagado. Isso não pode ser desfeito.</>
  ),
  delete: 'Apagar',
  anyOtherApp: 'qualquer outro',
  hint: (exe: string): ReactNode => (
    <>O perfil é aplicado sozinho quando o app em foco for <b>{exe}</b>. Selecionar um perfil aqui mostra o efeito na hora.</>
  ),
  minimize: 'Minimizar',
  maximize: 'Maximizar',
  restore: 'Restaurar',
  close: 'Fechar',
  collapseSidebar: 'Recolher barra lateral',
  expandSidebar: 'Expandir barra lateral',
  language: 'Idioma',
  on: 'Ligado',
  off: 'Desligado',
}

type Messages = typeof pt

const en: Messages = {
  newProfile: 'New profile',
  newProfileDesc: "Pick an open app or type the game's executable name.",
  noApps: 'No apps found.',
  executable: 'Executable',
  name: 'Name',
  cancel: 'Cancel',
  create: 'Create',
  windowsProfile: 'Windows',
  defaultTag: '(default)',
  saturation: 'Saturation',
  contrast: 'Contrast',
  deleteProfile: 'Delete profile',
  confirmDeleteTitle: 'Delete profile?',
  confirmDeleteDesc: (name) => (
    <>The profile <b>{name}</b> will be deleted. This cannot be undone.</>
  ),
  delete: 'Delete',
  anyOtherApp: 'any other',
  hint: (exe) => (
    <>The profile applies automatically when the focused app is <b>{exe}</b>. Selecting a profile here shows the effect instantly.</>
  ),
  minimize: 'Minimize',
  maximize: 'Maximize',
  restore: 'Restore',
  close: 'Close',
  collapseSidebar: 'Collapse sidebar',
  expandSidebar: 'Expand sidebar',
  language: 'Language',
  on: 'On',
  off: 'Off',
}

const es: Messages = {
  newProfile: 'Nuevo perfil',
  newProfileDesc: 'Elige una aplicación abierta o escribe el nombre del ejecutable del juego.',
  noApps: 'No se encontraron aplicaciones.',
  executable: 'Ejecutable',
  name: 'Nombre',
  cancel: 'Cancelar',
  create: 'Crear',
  windowsProfile: 'Windows',
  defaultTag: '(predeterminado)',
  saturation: 'Saturación',
  contrast: 'Contraste',
  deleteProfile: 'Eliminar perfil',
  confirmDeleteTitle: '¿Eliminar perfil?',
  confirmDeleteDesc: (name) => (
    <>El perfil <b>{name}</b> se eliminará. Esta acción no se puede deshacer.</>
  ),
  delete: 'Eliminar',
  anyOtherApp: 'cualquier otra',
  hint: (exe) => (
    <>El perfil se aplica solo cuando la aplicación en primer plano sea <b>{exe}</b>. Seleccionar un perfil aquí muestra el efecto al instante.</>
  ),
  minimize: 'Minimizar',
  maximize: 'Maximizar',
  restore: 'Restaurar',
  close: 'Cerrar',
  collapseSidebar: 'Contraer barra lateral',
  expandSidebar: 'Expandir barra lateral',
  language: 'Idioma',
  on: 'Activado',
  off: 'Desactivado',
}

const dict: Record<Lang, Messages> = { pt, en, es }
const STORAGE_KEY = 'lang'

function initialLang(): Lang {
  try {
    const saved = localStorage.getItem(STORAGE_KEY)
    if (saved === 'pt' || saved === 'en' || saved === 'es') return saved
  } catch { /* sem storage */ }
  const nav = (navigator.language || 'en').toLowerCase()
  return nav.startsWith('pt') ? 'pt' : nav.startsWith('es') ? 'es' : 'en'
}

const Ctx = createContext<{ lang: Lang; setLang: (l: Lang) => void; t: Messages }>({
  lang: 'en',
  setLang: () => {},
  t: en,
})

export function I18nProvider({ children }: { children: ReactNode }) {
  const [lang, setLangState] = useState<Lang>(initialLang)

  useEffect(() => {
    document.documentElement.lang = lang === 'pt' ? 'pt-BR' : lang
  }, [lang])

  function setLang(l: Lang) {
    setLangState(l)
    try { localStorage.setItem(STORAGE_KEY, l) } catch { /* sem storage */ }
  }

  return <Ctx.Provider value={{ lang, setLang, t: dict[lang] }}>{children}</Ctx.Provider>
}

export const useI18n = () => useContext(Ctx)
