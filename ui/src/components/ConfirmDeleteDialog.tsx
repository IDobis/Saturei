import { Button } from '@/components/ui/button'
import { Dialog, DialogContent, DialogDescription, DialogFooter, DialogHeader, DialogTitle } from '@/components/ui/dialog'
import type { Profile } from '@/lib/bridge'
import { useI18n } from '@/lib/i18n'

type Props = {
  /** Profile pending deletion; the dialog is open while this is set. */
  profile: Profile | null
  onCancel: () => void
  onConfirm: () => void
}

/** Asks before deleting, to protect against misclicks. Focus starts on Cancel so Enter is safe. */
export function ConfirmDeleteDialog({ profile, onCancel, onConfirm }: Props) {
  const { t } = useI18n()
  return (
    <Dialog open={!!profile} onOpenChange={(open) => !open && onCancel()}>
      <DialogContent className="sm:max-w-sm" showCloseButton={false}>
        <DialogHeader>
          <DialogTitle>{t.confirmDeleteTitle}</DialogTitle>
          <DialogDescription>{profile && t.confirmDeleteDesc(profile.name)}</DialogDescription>
        </DialogHeader>
        <DialogFooter>
          <Button variant="outline" autoFocus onClick={onCancel}>
            {t.cancel}
          </Button>
          <Button variant="destructive" onClick={onConfirm}>
            {t.delete}
          </Button>
        </DialogFooter>
      </DialogContent>
    </Dialog>
  )
}
