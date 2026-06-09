import { Button, Tooltip } from 'antd';
import { useControlStore } from '@/stores/controlStore';

export function UnitToggle() {
  const isDegree = useControlStore((s) => s.isDegree);
  const isMM = useControlStore((s) => s.isMM);
  const toggleDegree = useControlStore((s) => s.toggleDegree);
  const toggleMM = useControlStore((s) => s.toggleMM);

  return (
    <div style={{ display: 'flex', gap: 4 }}>
      <Tooltip title={isDegree ? '当前：度 (点击切换为弧度)' : '当前：弧度 (点击切换为度)'}>
        <Button size="small" onClick={toggleDegree}>
          {isDegree ? 'deg' : 'rad'}
        </Button>
      </Tooltip>
      <Tooltip title={isMM ? '当前：mm (点击切换为 m)' : '当前：m (点击切换为 mm)'}>
        <Button size="small" onClick={toggleMM}>
          {isMM ? 'mm' : 'm'}
        </Button>
      </Tooltip>
    </div>
  );
}
