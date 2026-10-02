import { useEffect, useState } from 'react'
import { readSetting, writeSetting } from '@/lib/storage'

const STORAGE_KEY = 'sidebar-collapsed'

/** Collapsed state of the sidebar, remembered between sessions. */
export function useSidebarCollapsed() {
  const [collapsed, setCollapsed] = useState(() => readSetting(STORAGE_KEY) === '1')

  useEffect(() => {
    writeSetting(STORAGE_KEY, collapsed ? '1' : '0')
  }, [collapsed])

  return { collapsed, toggle: () => setCollapsed((value) => !value) }
}
