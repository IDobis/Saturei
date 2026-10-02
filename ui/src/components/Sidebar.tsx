import { Plus } from 'lucide-react'
import { AnimatePresence, motion } from 'motion/react'
import type { ReactNode } from 'react'
import { Button } from '@/components/ui/button'
import { SIDEBAR_WIDTH } from '@/constants'
import type { Profile } from '@/lib/bridge'
import { useI18n } from '@/lib/i18n'
import { cn } from '@/lib/utils'

/**
 * Label next to a fixed-size icon slot. It is always mounted (never swapped for something else), so
 * collapsing only animates width and opacity and the layout never jumps or wraps mid-animation.
 */
function SidebarLabel({ collapsed, children }: { collapsed: boolean; children: ReactNode }) {
  return (
    <motion.span
      initial={false}
      animate={{ opacity: collapsed ? 0 : 1 }}
      // fade out fast when collapsing; when expanding wait for the width to open a little first
      transition={{ duration: collapsed ? 0.08 : 0.15, delay: collapsed ? 0 : 0.1 }}
      className="min-w-0 flex-1 overflow-hidden pr-3 text-left whitespace-nowrap"
    >
      {children}
    </motion.span>
  )
}

type Props = {
  profiles: Profile[]
  selectedId: string
  activeId: string | null
  collapsed: boolean
  onSelect: (id: string) => void
  onNewProfile: () => void
}

export function Sidebar({ profiles, selectedId, activeId, collapsed, onSelect, onNewProfile }: Props) {
  const { t } = useI18n()
  return (
    <motion.aside
      initial={false}
      animate={{ width: collapsed ? SIDEBAR_WIDTH.collapsed : SIDEBAR_WIDTH.expanded }}
      transition={{ type: 'spring', stiffness: 400, damping: 38 }}
      className="flex shrink-0 flex-col gap-3 overflow-hidden border-r p-3"
    >
      <motion.ul layout className="flex-1 space-y-1 overflow-x-hidden overflow-y-auto">
        <AnimatePresence initial={false}>
          {profiles.map((profile) => (
            <motion.li
              key={profile.id}
              layout
              initial={{ opacity: 0, x: -12 }}
              animate={{ opacity: 1, x: 0 }}
              exit={{ opacity: 0, x: -12 }}
              transition={{ type: 'spring', stiffness: 500, damping: 40 }}
            >
              <button
                onClick={() => onSelect(profile.id)}
                title={profile.name}
                className={cn(
                  'relative flex h-9 w-full items-center overflow-hidden rounded-md text-sm transition-colors hover:bg-accent',
                  selectedId === profile.id && 'bg-accent',
                )}
              >
                <span className="grid size-9 shrink-0 place-items-center font-semibold">{profile.name.charAt(0).toUpperCase()}</span>
                <SidebarLabel collapsed={collapsed}>{profile.name}</SidebarLabel>
                {activeId === profile.id && (
                  <span className="absolute top-1.5 left-6 size-2 rounded-full bg-green-500 ring-2 ring-background" />
                )}
              </button>
            </motion.li>
          ))}
        </AnimatePresence>
      </motion.ul>

      <Button variant="outline" onClick={onNewProfile} title={t.newProfile} className="h-9 w-full justify-start gap-0 overflow-hidden px-0">
        <span className="grid size-9 shrink-0 place-items-center">
          <Plus />
        </span>
        <SidebarLabel collapsed={collapsed}>{t.newProfile}</SidebarLabel>
      </Button>
    </motion.aside>
  )
}
