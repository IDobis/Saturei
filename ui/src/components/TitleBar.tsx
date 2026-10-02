import { PanelLeftClose, PanelLeftOpen } from 'lucide-react'
import logo from '@/assets/react.svg'
import { LanguageMenu } from '@/components/LanguageMenu'
import { WindowControls } from '@/components/WindowControls'
import { useI18n } from '@/lib/i18n'

type Props = {
  sidebarCollapsed: boolean
  onToggleSidebar: () => void
  maximized: boolean
}

/**
 * Custom title bar. The `app-drag` class lets the native host move the window by dragging it;
 * interactive children opt out with `app-no-drag` (see index.css).
 */
export function TitleBar({ sidebarCollapsed, onToggleSidebar, maximized }: Props) {
  const { t } = useI18n()
  return (
    <header className="app-drag flex h-10 shrink-0 items-center border-b bg-background/80 backdrop-blur">
      <div className="flex items-center gap-2 pl-3">
        <img src={logo} alt="" className="size-5" draggable={false} />
        <span className="text-sm font-semibold tracking-tight">Saturei</span>
        <button
          onClick={onToggleSidebar}
          aria-label={sidebarCollapsed ? t.expandSidebar : t.collapseSidebar}
          className="app-no-drag ml-2 grid size-7 place-items-center rounded-md text-muted-foreground transition-colors hover:bg-accent hover:text-foreground"
        >
          {sidebarCollapsed ? <PanelLeftOpen className="size-4" /> : <PanelLeftClose className="size-4" />}
        </button>
      </div>
      <div className="ml-auto flex h-full items-center">
        <LanguageMenu />
        <WindowControls maximized={maximized} />
      </div>
    </header>
  )
}
