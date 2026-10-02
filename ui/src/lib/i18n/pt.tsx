import type { Messages } from './types'

export const pt: Messages = {
  newProfile: 'Novo perfil',
  newProfileDesc: 'Escolha um app aberto ou digite o nome do executável do jogo.',
  noApps: 'Nenhum app encontrado.',
  executable: 'Executável',
  name: 'Nome',
  windowsProfile: 'Windows',
  defaultTag: '(padrão)',
  deleteProfile: 'Apagar perfil',
  confirmDeleteTitle: 'Apagar perfil?',
  confirmDeleteDesc: (profileName) => (
    <>O perfil <b>{profileName}</b> será apagado. Isso não pode ser desfeito.</>
  ),
  hint: (exe) => (
    <>O perfil é aplicado sozinho quando o app em foco for <b>{exe}</b>. Selecionar um perfil aqui mostra o efeito na hora.</>
  ),
  anyOtherApp: 'qualquer outro',
  saturation: 'Saturação',
  contrast: 'Contraste',
  resetFilters: 'Redefinir filtros',
  on: 'Ligado',
  off: 'Desligado',
  cancel: 'Cancelar',
  create: 'Criar',
  delete: 'Apagar',
  minimize: 'Minimizar',
  maximize: 'Maximizar',
  restore: 'Restaurar',
  close: 'Fechar',
  collapseSidebar: 'Recolher barra lateral',
  expandSidebar: 'Expandir barra lateral',
  language: 'Idioma',
}
