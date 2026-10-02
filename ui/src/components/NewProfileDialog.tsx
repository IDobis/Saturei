import { useEffect, useState } from 'react'
import { Button } from '@/components/ui/button'
import { Dialog, DialogContent, DialogDescription, DialogFooter, DialogHeader, DialogTitle } from '@/components/ui/dialog'
import { Input } from '@/components/ui/input'
import { Label } from '@/components/ui/label'
import type { HostApp } from '@/lib/bridge'
import { useI18n } from '@/lib/i18n'
import { cn } from '@/lib/utils'

const isExecutable = (value: string) => value.trim().toLowerCase().endsWith('.exe')

type FormProps = {
  apps: HostApp[]
  onLoadApps: () => void
  onClose: () => void
  onCreate: (name: string, exe: string) => void
}

/** The dialog content is only mounted while the dialog is open, so the form always starts empty. */
function NewProfileForm({ apps, onLoadApps, onClose, onCreate }: FormProps) {
  const { t } = useI18n()
  const [name, setName] = useState('')
  const [nameEdited, setNameEdited] = useState(false) // true once the user typed their own name
  const [exe, setExe] = useState('')

  useEffect(() => {
    onLoadApps() // refresh the list of open apps each time the dialog opens
  }, [onLoadApps])

  const pickApp = (app: HostApp) => {
    setExe(app.exe)
    if (!nameEdited) setName(app.title) // follow the picked app until the user chooses a name
  }

  const editName = (value: string) => {
    setName(value)
    setNameEdited(value !== '') // clearing the field hands control back to the picked app
  }

  return (
    <>
      <DialogHeader>
        <DialogTitle>{t.newProfile}</DialogTitle>
        <DialogDescription>{t.newProfileDesc}</DialogDescription>
      </DialogHeader>

      <div className="space-y-3">
        <div className="max-h-44 space-y-1 overflow-x-hidden overflow-y-auto rounded-md border p-1">
          {apps.length === 0 && <p className="p-2 text-sm text-muted-foreground">{t.noApps}</p>}
          {apps.map((app) => (
            <button
              key={app.exe}
              onClick={() => pickApp(app)}
              className={cn('flex w-full justify-between gap-2 rounded px-2 py-1.5 text-left text-sm hover:bg-accent', exe === app.exe && 'bg-accent')}
            >
              <span className="truncate">{app.title}</span>
              <span className="shrink-0 text-muted-foreground">{app.exe}</span>
            </button>
          ))}
        </div>

        <div className="space-y-1.5">
          <Label htmlFor="exe">{t.executable}</Label>
          <Input id="exe" value={exe} onChange={(event) => setExe(event.target.value)} placeholder="cs2.exe" />
        </div>
        <div className="space-y-1.5">
          <Label htmlFor="pname">{t.name}</Label>
          <Input id="pname" value={name} onChange={(event) => editName(event.target.value)} placeholder="Counter-Strike 2" />
        </div>
      </div>

      <DialogFooter>
        <Button variant="outline" onClick={onClose}>
          {t.cancel}
        </Button>
        <Button disabled={!isExecutable(exe)} onClick={() => onCreate(name.trim() || exe.trim(), exe.trim())}>
          {t.create}
        </Button>
      </DialogFooter>
    </>
  )
}

type Props = FormProps & { open: boolean }

/** Create a profile by picking one of the open apps or typing the executable name. */
export function NewProfileDialog({ open, onClose, ...form }: Props) {
  return (
    <Dialog open={open} onOpenChange={(isOpen) => !isOpen && onClose()}>
      <DialogContent className="sm:max-w-md [&>*]:min-w-0">
        <NewProfileForm onClose={onClose} {...form} />
      </DialogContent>
    </Dialog>
  )
}
