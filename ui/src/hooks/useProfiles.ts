import { useCallback, useEffect, useState } from 'react'
import { CONTRAST, SATURATION } from '@/constants'
import { send, subscribe, type HostApp, type Profile } from '@/lib/bridge'

/**
 * Keeps the profile list in sync with the native host and exposes the actions the UI can perform.
 * The host is the source of truth: every change is sent to it, and it replies with what is active.
 */
export function useProfiles() {
  const [profiles, setProfiles] = useState<Profile[]>([])
  const [selectedId, setSelectedId] = useState('')
  const [activeId, setActiveId] = useState<string | null>(null)
  const [apps, setApps] = useState<HostApp[]>([])

  useEffect(() => {
    const unsubscribe = subscribe((message) => {
      switch (message.type) {
        case 'init':
          setProfiles(message.profiles)
          setActiveId(message.activeId)
          // keep the selection if it still exists (e.g. after a delete), otherwise fall back to the first
          setSelectedId((id) => (message.profiles.some((p) => p.id === id) ? id : (message.profiles[0]?.id ?? '')))
          break
        case 'active':
          setActiveId(message.id)
          break
        case 'processes':
          setApps(message.list)
          break
      }
    })
    send({ type: 'ready' })
    return unsubscribe
  }, [])

  const current = profiles.find((p) => p.id === selectedId)

  /** Selecting also applies the profile, so the effect is visible right away. */
  const select = (id: string) => {
    setSelectedId(id)
    send({ type: 'select', id })
  }

  const updateCurrent = (patch: Partial<Profile>) => {
    if (!current) return
    const next = { ...current, ...patch }
    setProfiles((list) => list.map((p) => (p.id === next.id ? next : p)))
    send({ type: 'setProfile', profile: next })
  }

  const create = (name: string, exe: string) => {
    const profile: Profile = {
      id: crypto.randomUUID(),
      name,
      exe,
      enabled: true,
      saturation: SATURATION.neutral,
      contrast: CONTRAST.neutral,
    }
    setProfiles((list) => [...list, profile])
    setSelectedId(profile.id)
    send({ type: 'setProfile', profile })
  }

  const remove = (id: string) => send({ type: 'deleteProfile', id })

  const refreshApps = useCallback(() => send({ type: 'listProcesses' }), [])

  return { profiles, current, selectedId, activeId, apps, select, updateCurrent, create, remove, refreshApps }
}
