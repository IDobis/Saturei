import { useEffect, useState } from 'react'
import { subscribe } from '@/lib/bridge'

/** Whether the native window is maximized (the host reports it whenever the window is resized). */
export function useMaximized() {
  const [maximized, setMaximized] = useState(false)

  useEffect(
    () =>
      subscribe((message) => {
        if (message.type === 'maximized') setMaximized(message.value)
      }),
    [],
  )

  return maximized
}
