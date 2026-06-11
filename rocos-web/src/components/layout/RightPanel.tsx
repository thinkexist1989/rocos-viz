import { JointJogPanel } from '@/components/control/JointJogPanel';
import { CartesianJogPanel } from '@/components/control/CartesianJogPanel';
import { SpeedSlider } from '@/components/control/SpeedSlider';
import { FrameSelector } from '@/components/control/FrameSelector';
import { WorkModeSelector } from '@/components/control/WorkModeSelector';
import { MotionCommandPanel } from '@/components/control/MotionCommandPanel';

export function RightPanel() {
  return (
    <div style={{ display: 'flex', flexDirection: 'column', gap: 8 }}>
      {/* Top toolbar row */}
      <div className="panel-card" style={{ display: 'flex', gap: 8, alignItems: 'center', flexWrap: 'wrap' }}>
        <WorkModeSelector />
        <FrameSelector />
      </div>

      {/* Speed */}
      <div className="panel-card">
        <SpeedSlider />
      </div>

      {/* Joint Space */}
      <div className="panel-card">
        <JointJogPanel />
      </div>

      {/* Cartesian Space */}
      <div className="panel-card">
        <CartesianJogPanel />
      </div>

      {/* Precise Control */}
      <div className="panel-card">
        <MotionCommandPanel />
      </div>
    </div>
  );
}
