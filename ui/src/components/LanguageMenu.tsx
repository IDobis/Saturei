import { Globe } from 'lucide-react'
import {
  DropdownMenu,
  DropdownMenuContent,
  DropdownMenuRadioGroup,
  DropdownMenuRadioItem,
  DropdownMenuTrigger,
} from '@/components/ui/dropdown-menu'
import { LANGUAGES, useI18n, type Lang } from '@/lib/i18n'

export function LanguageMenu() {
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
        <DropdownMenuRadioGroup value={lang} onValueChange={(value) => setLang(value as Lang)}>
          {LANGUAGES.map((language) => (
            <DropdownMenuRadioItem key={language.code} value={language.code} closeOnClick>
              {language.label}
            </DropdownMenuRadioItem>
          ))}
        </DropdownMenuRadioGroup>
      </DropdownMenuContent>
    </DropdownMenu>
  )
}
