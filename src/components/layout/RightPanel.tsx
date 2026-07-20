import { Tabs } from 'antd';
import { AppstoreOutlined, AimOutlined } from '@ant-design/icons';
import { JointJogPanel } from '@/components/control/JointJogPanel';
import { CartesianJogPanel } from '@/components/control/CartesianJogPanel';
import { NullSpacePanel } from '@/components/control/NullSpacePanel';
import { MotionControl } from '@/components/control/MotionControl';
import { FrameSelector } from '@/components/control/FrameSelector';
import { WorkModeSelector } from '@/components/control/WorkModeSelector';
import { MotionCommandPanel } from '@/components/control/MotionCommandPanel';
import { useT } from '@/i18n/useT';

export function RightPanel() {
  const t = useT();

  return (
    <div style={{ display: 'flex', flexDirection: 'column', gap: 8 }}>
      {/* Shared controls — apply to both jog and precise motion */}
      <div className="panel-card" style={{ display: 'flex', gap: 8, alignItems: 'center', flexWrap: 'wrap' }}>
        <WorkModeSelector />
        <FrameSelector />
      </div>

      <div className="panel-card">
        <MotionControl />
      </div>

      {/* Jog vs. Precise split into tabs so the panel doesn't grow too long */}
      <Tabs
        className="control-tabs"
        defaultActiveKey="jog"
        items={[
          {
            key: 'jog',
            label: (
              <span className="control-tab-label">
                <AppstoreOutlined />
                {t('tab.jog')}
              </span>
            ),
            children: (
              <div style={{ display: 'flex', flexDirection: 'column', gap: 8 }}>
                <div className="panel-card">
                  <JointJogPanel />
                </div>
                <div className="panel-card">
                  <CartesianJogPanel />
                </div>
                <div className="panel-card">
                  <NullSpacePanel />
                </div>
              </div>
            ),
          },
          {
            key: 'precise',
            label: (
              <span className="control-tab-label">
                <AimOutlined />
                {t('tab.precise')}
              </span>
            ),
            children: (
              <div className="panel-card">
                <MotionCommandPanel />
              </div>
            ),
          },
        ]}
      />
    </div>
  );
}
