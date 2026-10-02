import { RotateCcw, Trash2 } from 'lucide-react'
import { motion } from 'motion/react'
import { SliderField } from '@/components/SliderField'
import { Button } from '@/components/ui/button'
import { Card, CardContent } from '@/components/ui/card'
import { Input } from '@/components/ui/input'
import { Switch } from '@/components/ui/switch'
import { CONTRAST, SATURATION, WINDOWS_PROFILE_ID } from '@/constants'
import type { Profile } from '@/lib/bridge'
import { useI18n } from '@/lib/i18n'
import { cn } from '@/lib/utils'

type Props = {
  profile: Profile
  onChange: (patch: Partial<Profile>) => void
  onRequestDelete: () => void
}

/** Editor for one profile: name, on/off switch and the color sliders. */
export function ProfilePanel({ profile, onChange, onRequestDelete }: Props) {
  const { t } = useI18n()
  const isWindowsProfile = profile.id === WINDOWS_PROFILE_ID
  const isNeutral = profile.saturation === SATURATION.neutral && profile.contrast === CONTRAST.neutral

  // Back to "no filter": the profile keeps its name, executable and on/off state.
  const resetFilters = () => onChange({ saturation: SATURATION.neutral, contrast: CONTRAST.neutral })

  return (
    <motion.div
      initial={{ opacity: 0, y: 8 }}
      animate={{ opacity: 1, y: 0 }}
      exit={{ opacity: 0, y: -8 }}
      transition={{ duration: 0.15 }}
      className="mx-auto max-w-xl"
    >
      <Card>
        <div className="flex items-center gap-3 px-(--card-spacing)">
          {isWindowsProfile ? (
            <span className="min-w-0 flex-1 truncate text-base font-medium">
              {t.windowsProfile} <span className="text-xs text-muted-foreground">{t.defaultTag}</span>
            </span>
          ) : (
            <div className="min-w-0 flex-1 space-y-1">
              <Input value={profile.name} onChange={(event) => onChange({ name: event.target.value })} className="h-8 font-medium" />
              <p className="truncate px-1 text-xs text-muted-foreground">{profile.exe}</p>
            </div>
          )}

          <label className="flex shrink-0 cursor-pointer items-center gap-2.5 select-none">
            <span className={cn('w-20 text-right text-xs font-medium transition-colors', profile.enabled ? 'text-emerald-400' : 'text-muted-foreground')}>
              {profile.enabled ? t.on : t.off}
            </span>
            <Switch checked={profile.enabled} onCheckedChange={(enabled) => onChange({ enabled })} aria-label={profile.enabled ? t.on : t.off} />
          </label>

          {!isWindowsProfile && (
            <Button variant="ghost" size="icon" onClick={onRequestDelete} aria-label={t.deleteProfile} className="shrink-0 text-muted-foreground hover:text-red-400">
              <Trash2 />
            </Button>
          )}
        </div>

        <CardContent className={cn('space-y-5 transition-opacity duration-200', !profile.enabled && 'opacity-50')}>
          <SliderField
            label={t.saturation}
            value={profile.saturation}
            min={SATURATION.min}
            max={SATURATION.max}
            unit="%"
            onChange={(saturation) => onChange({ saturation })}
          />
          <SliderField
            label={t.contrast}
            value={profile.contrast}
            min={CONTRAST.min}
            max={CONTRAST.max}
            onChange={(contrast) => onChange({ contrast })}
          />
          <div className="flex items-start justify-between gap-4">
            <p className="text-xs text-muted-foreground">{t.hint(profile.exe || t.anyOtherApp)}</p>
            <Button variant="outline" size="sm" onClick={resetFilters} disabled={isNeutral} className="shrink-0">
              <RotateCcw />
              {t.resetFilters}
            </Button>
          </div>
        </CardContent>
      </Card>
    </motion.div>
  )
}
