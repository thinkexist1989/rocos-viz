import { Button, Tooltip } from 'antd';
import { useControlStore } from '@/stores/controlStore';
import { useT } from '@/i18n/useT';

export function UnitToggle() {
  const t = useT();
  const isDegree = useControlStore((s) => s.isDegree);
  const isMM = useControlStore((s) => s.isMM);
  const toggleDegree = useControlStore((s) => s.toggleDegree);
  const toggleMM = useControlStore((s) => s.toggleMM);

  return (
    <div style={{ display: 'flex', gap: 4 }}>
      <Tooltip title={isDegree ? t('unit.degTip') : t('unit.radTip')}>
        <Button size="small" onClick={toggleDegree}>
          {isDegree ? 'deg' : 'rad'}
        </Button>
      </Tooltip>
      <Tooltip title={isMM ? t('unit.mmTip') : t('unit.mTip')}>
        <Button size="small" onClick={toggleMM}>
          {isMM ? 'mm' : 'm'}
        </Button>
      </Tooltip>
    </div>
  );
}
