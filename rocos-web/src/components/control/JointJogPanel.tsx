import { useRobotStateStore } from '@/stores/robotStateStore';
import { JointJogItem } from './JointJogItem';
import { PositionBar } from '@/components/common/PositionBar';

export function JointJogPanel() {
  const robotState = useRobotStateStore((s) => s.robotState);
  const joints = robotState?.joint_states || [];

  return (
    <div className="panel-section">
      <div className="panel-section-header">JOINT SPACE</div>
      <div>
        {joints.map((joint, i) => (
          <JointJogItem key={`joint_${i}`} index={i} joint={joint} />
        ))}
        {joints.length === 0 && (
          <div style={{ padding: '8px 0', color: '#858585', fontSize: 12, textAlign: 'center' }}>
            连接机器人后显示关节数据
          </div>
        )}
      </div>
    </div>
  );
}
