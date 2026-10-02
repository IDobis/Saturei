import { Copy, Minus, Square, X } from 'lucide-react'
import type { ReactNode } from 'react'
import { send, type WindowAction } from '@/lib/bridge'
import { useI18n } from '@/lib/i18n'
import { cn } from '@/lib/utils'

function WindowButton({ action, label, danger, children }: { action: WindowAction; label: string; danger?: boolean; children: ReactNode }) {
  return (
    <button
      onClick={() => send({ type: 'window', action })}
      aria-label={label}
      className={cn(
        'app-no-drag grid h-full w-12 place-items-center text-muted-foreground transition-colors',
        danger ? 'hover:bg-red-600 hover:text-white' : 'hover:bg-accent hover:text-foreground',
      )}
    >
      {children}
    </button>
  )
}

/** Minimize / maximize-restore / close, replacing the native caption buttons. */
export function WindowControls({ maximized }: { maximized: boolean }) {
  const { t } = useI18n()
  return (
    <div className="flex h-full">
      <WindowButton action="minimize" label={t.minimize}>
        <Minus className="size-4" />
      </WindowButton>
      <WindowButton action="maximize" label={maximized ? t.restore : t.maximize}>
        {maximized ? <Copy className="size-3.5" /> : <Square className="size-3.5" />}
      </WindowButton>
      <WindowButton action="close" label={t.close} danger>
        <X className="size-4" />
      </WindowButton>
    </div>
  )
}
