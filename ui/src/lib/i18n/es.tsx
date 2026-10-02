import type { Messages } from './types'

export const es: Messages = {
  newProfile: 'Nuevo perfil',
  newProfileDesc: 'Elige una aplicación abierta o escribe el nombre del ejecutable del juego.',
  noApps: 'No se encontraron aplicaciones.',
  executable: 'Ejecutable',
  name: 'Nombre',
  windowsProfile: 'Windows',
  defaultTag: '(predeterminado)',
  deleteProfile: 'Eliminar perfil',
  confirmDeleteTitle: '¿Eliminar perfil?',
  confirmDeleteDesc: (profileName) => (
    <>El perfil <b>{profileName}</b> se eliminará. Esta acción no se puede deshacer.</>
  ),
  hint: (exe) => (
    <>El perfil se aplica solo cuando la aplicación en primer plano sea <b>{exe}</b>. Seleccionar un perfil aquí muestra el efecto al instante.</>
  ),
  anyOtherApp: 'cualquier otra',
  saturation: 'Saturación',
  contrast: 'Contraste',
  resetFilters: 'Restablecer filtros',
  on: 'Activado',
  off: 'Desactivado',
  cancel: 'Cancelar',
  create: 'Crear',
  delete: 'Eliminar',
  minimize: 'Minimizar',
  maximize: 'Maximizar',
  restore: 'Restaurar',
  close: 'Cerrar',
  collapseSidebar: 'Contraer barra lateral',
  expandSidebar: 'Expandir barra lateral',
  language: 'Idioma',
}
