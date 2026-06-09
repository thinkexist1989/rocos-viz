import { Select, Typography } from 'antd';
import { useControlStore } from '@/stores/controlStore';
import { FRAME } from '@/core/constants';

const { Text } = Typography;

const FRAME_OPTIONS = [
  { value: FRAME.BASE, label: 'BASE' },
  { value: FRAME.FLANGE, label: 'FLANGE' },
  { value: FRAME.TOOL, label: 'TOOL' },
  { value: FRAME.OBJECT, label: 'OBJECT' },
];

export function FrameSelector() {
  const currentFrame = useControlStore((s) => s.currentFrame);
  const setFrame = useControlStore((s) => s.setFrame);

  const currentLabel = FRAME_OPTIONS.find((o) => o.value === currentFrame)?.label || 'BASE';

  return (
    <div style={{ display: 'flex', alignItems: 'center', gap: 6 }}>
      <Text style={{ fontSize: 12, color: 'var(--color-text-secondary)' }}>Frame:</Text>
      <Select
        value={currentFrame}
        onChange={setFrame}
        options={FRAME_OPTIONS}
        size="small"
        style={{ width: 100 }}
      />
    </div>
  );
}
