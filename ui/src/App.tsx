import { AnimatePresence } from 'motion/react'
import { useState } from 'react'
import { ConfirmDeleteDialog } from '@/components/ConfirmDeleteDialog'
import { NewProfileDialog } from '@/components/NewProfileDialog'
import { ProfilePanel } from '@/components/ProfilePanel'
import { Sidebar } from '@/components/Sidebar'
import { TitleBar } from '@/components/TitleBar'
import { WINDOWS_PROFILE_ID } from '@/constants'
import { useMaximized } from '@/hooks/useMaximized'
import { useProfiles } from '@/hooks/useProfiles'
import { useSidebarCollapsed } from '@/hooks/useSidebarCollapsed'
import type { Profile } from '@/lib/bridge'

export default function App() {
  const { profiles, current, selectedId, activeId, apps, select, updateCurrent, create, remove, refreshApps } = useProfiles()
  const { collapsed, toggle } = useSidebarCollapsed()
  const maximized = useMaximized()

  const [newProfileOpen, setNewProfileOpen] = useState(false)
  const [pendingDelete, setPendingDelete] = useState<Profile | null>(null)

  const requestDelete = () => {
    if (current && current.id !== WINDOWS_PROFILE_ID) setPendingDelete(current)
  }

  const confirmDelete = () => {
    if (pendingDelete) remove(pendingDelete.id)
    setPendingDelete(null)
  }

  const handleCreate = (name: string, exe: string) => {
    create(name, exe)
    setNewProfileOpen(false)
  }

  return (
    <div className="flex h-screen flex-col bg-background text-foreground">
      <TitleBar sidebarCollapsed={collapsed} onToggleSidebar={toggle} maximized={maximized} />

      <div className="flex min-h-0 flex-1">
        <Sidebar
          profiles={profiles}
          selectedId={selectedId}
          activeId={activeId}
          collapsed={collapsed}
          onSelect={select}
          onNewProfile={() => setNewProfileOpen(true)}
        />

        <main className="flex-1 overflow-auto p-6">
          <AnimatePresence mode="wait">
            {current && <ProfilePanel key={current.id} profile={current} onChange={updateCurrent} onRequestDelete={requestDelete} />}
          </AnimatePresence>
        </main>
      </div>

      <ConfirmDeleteDialog profile={pendingDelete} onCancel={() => setPendingDelete(null)} onConfirm={confirmDelete} />
      <NewProfileDialog
        open={newProfileOpen}
        apps={apps}
        onLoadApps={refreshApps}
        onClose={() => setNewProfileOpen(false)}
        onCreate={handleCreate}
      />
    </div>
  )
}
