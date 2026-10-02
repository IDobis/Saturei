import { Copy, Globe, Minus, PanelLeftClose, PanelLeftOpen, Plus, Square, Trash2, X } from 'lucide-react'
import { AnimatePresence, motion } from 'motion/react'
import { useEffect, useState } from 'react'
import { Button } from '@/components/ui/button'
import { Card, CardContent } from '@/components/ui/card'
import { DropdownMenu, DropdownMenuContent, DropdownMenuRadioGroup, DropdownMenuRadioItem, DropdownMenuTrigger } from '@/components/ui/dropdown-menu'
import { Dialog, DialogContent, DialogDescription, DialogFooter, DialogHeader, DialogTitle } from '@/components/ui/dialog'
import { Input } from '@/components/ui/input'
import { Label } from '@/components/ui/label'
import { Slider } from '@/components/ui/slider'
import { Switch } from '@/components/ui/switch'
import logo from '@/assets/react.svg'
import { type Profile, onHost, send } from '@/lib/bridge'
import { LANGS, type Lang, useI18n } from '@/lib/i18n'

type Proc = { exe: string; title: string }


function Control({ label, value, onChange, max = 100, unit = '' }: { label: string; value: number; onChange: (v: number) => void; max?: number; unit?: string }) {
  return (
    <div className="space-y-2">
      <div className="flex justify-between text-sm">
        <span>{label}</span>
        <span className="tabular-nums text-muted-foreground">{value}{unit}</span>
      </div>
      <Slider value={[value]} min={0} max={max} step={1} onValueChange={(v) => onChange(Array.isArray(v) ? v[0] : v)} />
    </div>
  )
}

// Texto de item da sidebar: sempre montado (sem quebra de linha); so a opacidade anima.
// Ao expandir espera a largura abrir um pouco; ao recolher some rapido.
function SideLabel({ collapsed, children }: { collapsed: boolean; children: React.ReactNode }) {
  return (
    <motion.span
      initial={false}
      animate={{ opacity: collapsed ? 0 : 1 }}
      transition={{ duration: collapsed ? 0.08 : 0.15, delay: collapsed ? 0 : 0.1 }}
      className="min-w-0 flex-1 overflow-hidden pr-3 text-left whitespace-nowrap"
    >
      {children}
    </motion.span>
  )
}

function WinButton({ onClick, label, danger, children }: { onClick: () => void; label: string; danger?: boolean; children: React.ReactNode }) {
  return (
    <button
      onClick={onClick}
      aria-label={label}
      className={`app-no-drag grid h-full w-12 place-items-center text-muted-foreground transition-colors ${danger ? 'hover:bg-red-600 hover:text-white' : 'hover:bg-accent hover:text-foreground'}`}
    >
      {children}
    </button>
  )
}

function LanguageMenu() {
  const { lang, setLang, t } = useI18n()
  return (
    <DropdownMenu>
      <DropdownMenuTrigger
        aria-label={t.language}
        title={t.language}
        className="app-no-drag mr-1 flex h-7 items-center gap-1.5 rounded-md px-2 text-xs font-medium text-muted-foreground transition-colors outline-none hover:bg-accent hover:text-foreground"
      >
        <Globe className="size-4" />
        {lang.toUpperCase()}
      </DropdownMenuTrigger>
      <DropdownMenuContent align="end" className="min-w-36">
        <DropdownMenuRadioGroup value={lang} onValueChange={(v) => setLang(v as Lang)}>
          {LANGS.map((l) => (
            <DropdownMenuRadioItem key={l.code} value={l.code}>{l.label}</DropdownMenuRadioItem>
          ))}
        </DropdownMenuRadioGroup>
      </DropdownMenuContent>
    </DropdownMenu>
  )
}

function TitleBar({ collapsed, onToggle, maximized }: { collapsed: boolean; onToggle: () => void; maximized: boolean }) {
  const { t } = useI18n()
  return (
    <header className="app-drag flex h-10 shrink-0 items-center border-b bg-background/80 backdrop-blur">
      <div className="flex items-center gap-2 pl-3">
        <img src={logo} alt="" className="size-5" draggable={false} />
        <span className="text-sm font-semibold tracking-tight">Saturei</span>
        <button
          onClick={onToggle}
          aria-label={collapsed ? t.expandSidebar : t.collapseSidebar}
          className="app-no-drag ml-2 grid size-7 place-items-center rounded-md text-muted-foreground transition-colors hover:bg-accent hover:text-foreground"
        >
          {collapsed ? <PanelLeftOpen className="size-4" /> : <PanelLeftClose className="size-4" />}
        </button>
      </div>
      <div className="ml-auto flex h-full items-center">
        <LanguageMenu />
        <div className="flex h-full">
        <WinButton label={t.minimize} onClick={() => send({ type: 'window', action: 'minimize' })}><Minus className="size-4" /></WinButton>
        <WinButton label={maximized ? t.restore : t.maximize} onClick={() => send({ type: 'window', action: 'maximize' })}>
          {maximized ? <Copy className="size-3.5" /> : <Square className="size-3.5" />}
        </WinButton>
        <WinButton label={t.close} danger onClick={() => send({ type: 'window', action: 'close' })}><X className="size-4" /></WinButton>
        </div>
      </div>
    </header>
  )
}

function ConfirmDeleteDialog({ profile, onClose, onConfirm }: { profile: Profile | null; onClose: () => void; onConfirm: () => void }) {
  const { t } = useI18n()
  return (
    <Dialog open={!!profile} onOpenChange={(o) => !o && onClose()}>
      <DialogContent className="sm:max-w-sm" showCloseButton={false}>
        <DialogHeader>
          <DialogTitle>{t.confirmDeleteTitle}</DialogTitle>
          <DialogDescription>{profile && t.confirmDeleteDesc(profile.name)}</DialogDescription>
        </DialogHeader>
        <DialogFooter>
          <Button variant="outline" autoFocus onClick={onClose}>{t.cancel}</Button>
          <Button variant="destructive" onClick={onConfirm}>{t.delete}</Button>
        </DialogFooter>
      </DialogContent>
    </Dialog>
  )
}

function NewProfileDialog({ open, onClose, procs, onCreate }: { open: boolean; onClose: () => void; procs: Proc[]; onCreate: (name: string, exe: string) => void }) {
  const { t } = useI18n()
  const [name, setName] = useState('')
  const [exe, setExe] = useState('')

  useEffect(() => {
    if (open) { setName(''); setExe(''); send({ type: 'listProcesses' }) }
  }, [open])

  const valid = exe.trim().toLowerCase().endsWith('.exe')

  return (
    <Dialog open={open} onOpenChange={(o) => !o && onClose()}>
      <DialogContent className="sm:max-w-md [&>*]:min-w-0">
        <DialogHeader>
          <DialogTitle>{t.newProfile}</DialogTitle>
          <DialogDescription>{t.newProfileDesc}</DialogDescription>
        </DialogHeader>
        <div className="space-y-3">
          <div className="max-h-44 space-y-1 overflow-y-auto overflow-x-hidden rounded-md border p-1">
            {procs.length === 0 && <p className="p-2 text-sm text-muted-foreground">{t.noApps}</p>}
            {procs.map((p) => (
              <button
                key={p.exe}
                onClick={() => { setExe(p.exe); if (!name) setName(p.title) }}
                className={`flex w-full justify-between gap-2 rounded px-2 py-1.5 text-left text-sm hover:bg-accent ${exe === p.exe ? 'bg-accent' : ''}`}
              >
                <span className="truncate">{p.title}</span>
                <span className="shrink-0 text-muted-foreground">{p.exe}</span>
              </button>
            ))}
          </div>
          <div className="space-y-1.5">
            <Label htmlFor="exe">{t.executable}</Label>
            <Input id="exe" value={exe} onChange={(e) => setExe(e.target.value)} placeholder="cs2.exe" />
          </div>
          <div className="space-y-1.5">
            <Label htmlFor="pname">{t.name}</Label>
            <Input id="pname" value={name} onChange={(e) => setName(e.target.value)} placeholder="Counter-Strike 2" />
          </div>
        </div>
        <DialogFooter>
          <Button variant="outline" onClick={onClose}>{t.cancel}</Button>
          <Button disabled={!valid} onClick={() => onCreate(name.trim() || exe.trim(), exe.trim())}>{t.create}</Button>
        </DialogFooter>
      </DialogContent>
    </Dialog>
  )
}

export default function App() {
  const { t } = useI18n()
  const [profiles, setProfiles] = useState<Profile[]>([])
  const [selected, setSelected] = useState<string>('')
  const [active, setActive] = useState<string | null>(null)
  const [procs, setProcs] = useState<Proc[]>([])
  const [dialog, setDialog] = useState(false)
  const [deleting, setDeleting] = useState<Profile | null>(null)
  const [maximized, setMaximized] = useState(false)
  const [collapsed, setCollapsed] = useState(() => {
    try { return localStorage.getItem('sidebar-collapsed') === '1' } catch { return false }
  })

  function toggleSidebar() {
    setCollapsed((c) => {
      try { localStorage.setItem('sidebar-collapsed', c ? '0' : '1') } catch { /* sem storage */ }
      return !c
    })
  }

  useEffect(() => {
    onHost((m) => {
      if (m.type === 'init') {
        setProfiles(m.profiles)
        setActive(m.activeId)
        setSelected((s) => (m.profiles.some((p) => p.id === s) ? s : m.profiles[0]?.id || ''))
      } else if (m.type === 'active') setActive(m.id)
      else if (m.type === 'processes') setProcs(m.list)
      else if (m.type === 'maximized') setMaximized(m.value)
    })
    send({ type: 'ready' })
  }, [])

  const current = profiles.find((p) => p.id === selected)

  function choose(id: string) {
    setSelected(id)
    send({ type: 'select', id }) // aplica o perfil na hora, como preview
  }

  function update(patch: Partial<Profile>) {
    if (!current) return
    const next = { ...current, ...patch }
    setProfiles((ps) => ps.map((p) => (p.id === next.id ? next : p)))
    send({ type: 'setProfile', profile: next })
  }

  function create(name: string, exe: string) {
    const p: Profile = { id: crypto.randomUUID(), name, exe, enabled: true, saturation: 100, contrast: 50 }
    setProfiles((ps) => [...ps, p])
    setSelected(p.id)
    send({ type: 'setProfile', profile: p })
    setDialog(false)
  }

  function askRemove() {
    if (current && current.id !== 'win') setDeleting(current)
  }

  function confirmRemove() {
    if (deleting) send({ type: 'deleteProfile', id: deleting.id })
    setDeleting(null)
  }

  return (
    <div className="flex h-screen flex-col bg-background text-foreground">
      <TitleBar collapsed={collapsed} onToggle={toggleSidebar} maximized={maximized} />
      <div className="flex min-h-0 flex-1">
      <motion.aside
        initial={false}
        animate={{ width: collapsed ? 60 : 256 }}
        transition={{ type: 'spring', stiffness: 400, damping: 38 }}
        className="flex shrink-0 flex-col gap-3 overflow-hidden border-r p-3"
      >
        {/* Nada aqui troca de elemento ao recolher: so largura/opacidade animam (sem pulo de layout). */}
        <motion.ul layout className="flex-1 space-y-1 overflow-y-auto overflow-x-hidden">
          <AnimatePresence initial={false}>
            {profiles.map((p) => (
              <motion.li
                key={p.id}
                layout
                initial={{ opacity: 0, x: -12 }}
                animate={{ opacity: 1, x: 0 }}
                exit={{ opacity: 0, x: -12 }}
                transition={{ type: 'spring', stiffness: 500, damping: 40 }}
              >
                <button
                  onClick={() => choose(p.id)}
                  title={p.name}
                  className={`relative flex h-9 w-full items-center overflow-hidden rounded-md text-sm transition-colors hover:bg-accent ${selected === p.id ? 'bg-accent' : ''}`}
                >
                  <span className="grid size-9 shrink-0 place-items-center font-semibold">{p.name.charAt(0).toUpperCase()}</span>
                  <SideLabel collapsed={collapsed}>{p.name}</SideLabel>
                  {active === p.id && <span className="absolute top-1.5 left-6 size-2 rounded-full bg-green-500 ring-2 ring-background" />}
                </button>
              </motion.li>
            ))}
          </AnimatePresence>
        </motion.ul>
        <Button variant="outline" onClick={() => setDialog(true)} title={t.newProfile} className="h-9 w-full justify-start gap-0 overflow-hidden px-0">
          <span className="grid size-9 shrink-0 place-items-center"><Plus /></span>
          <SideLabel collapsed={collapsed}>{t.newProfile}</SideLabel>
        </Button>
      </motion.aside>

      <main className="flex-1 overflow-auto p-6">
        <AnimatePresence mode="wait">
          {current && (
            <motion.div
              key={current.id}
              initial={{ opacity: 0, y: 8 }}
              animate={{ opacity: 1, y: 0 }}
              exit={{ opacity: 0, y: -8 }}
              transition={{ duration: 0.15 }}
              className="mx-auto max-w-xl"
            >
              <Card>
                <div className="flex items-center gap-3 px-(--card-spacing)">
                  {current.id === 'win' ? (
                    <span className="min-w-0 flex-1 truncate text-base font-medium">{t.windowsProfile} <span className="text-xs text-muted-foreground">{t.defaultTag}</span></span>
                  ) : (
                    <div className="min-w-0 flex-1 space-y-1">
                      <Input value={current.name} onChange={(e) => update({ name: e.target.value })} className="h-8 font-medium" />
                      <p className="truncate px-1 text-xs text-muted-foreground">{current.exe}</p>
                    </div>
                  )}
                  <label className="flex shrink-0 cursor-pointer items-center gap-2.5 select-none">
                    <span className={`w-20 text-right text-xs font-medium transition-colors ${current.enabled ? 'text-emerald-400' : 'text-muted-foreground'}`}>
                      {current.enabled ? t.on : t.off}
                    </span>
                    <Switch checked={current.enabled} onCheckedChange={(v) => update({ enabled: v })} aria-label={current.enabled ? t.on : t.off} />
                  </label>
                  {current.id !== 'win' && (
                    <Button variant="ghost" size="icon" onClick={askRemove} aria-label={t.deleteProfile} className="shrink-0 text-muted-foreground hover:text-red-400">
                      <Trash2 />
                    </Button>
                  )}
                </div>
                <CardContent className={`space-y-5 transition-opacity duration-200 ${current.enabled ? '' : 'opacity-50'}`}>
                  <Control label={t.saturation} value={current.saturation} max={300} unit="%" onChange={(v) => update({ saturation: v })} />
                  <Control label={t.contrast} value={current.contrast} onChange={(v) => update({ contrast: v })} />
                  <p className="text-xs text-muted-foreground">{t.hint(current.exe || t.anyOtherApp)}</p>
                </CardContent>
              </Card>
            </motion.div>
          )}
        </AnimatePresence>
      </main>
      </div>

      <ConfirmDeleteDialog profile={deleting} onClose={() => setDeleting(null)} onConfirm={confirmRemove} />
      <NewProfileDialog open={dialog} onClose={() => setDialog(false)} procs={procs} onCreate={create} />
    </div>
  )
}
