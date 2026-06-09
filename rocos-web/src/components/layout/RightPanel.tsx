import { JointJogPanel } from '@/components/control/JointJogPanel';
import { CartesianJogPanel } from '@/components/control/CartesianJogPanel';
import { SpeedSlider } from '@/components/control/SpeedSlider';
import { FrameSelector } from '@/components/control/FrameSelector';
import { WorkModeSelector } from '@/components/control/WorkModeSelector';
import { EnableButton } from '@/components/control/EnableButton';
import { MotionCommandPanel } from '@/components/control/MotionCommandPanel';
import { DisplayToggles } from '@/components/common/DisplayToggles';
import { useControlStore } from '@/stores/controlStore';

export function RightPanel() {
  return (
    <div>
      <div style={{ display: 'flex', gap: 8, marginBottom: 12, alignItems: 'center' }}>
        <WorkModeSelector />
        <FrameSelector />
        <EnableButton />
      </div>

      <SpeedSlider />

      <CartesianJogPanel />

      <JointJogPanel />

      <MotionCommandPanel />
    </div>
  );
}
