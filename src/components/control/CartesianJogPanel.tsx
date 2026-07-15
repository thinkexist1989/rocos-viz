import { useRobotStateStore } from '@/stores/robotStateStore';
import { CartesianJogItem } from './CartesianJogItem';
import { useControlStore } from '@/stores/controlStore';
import { useT } from '@/i18n/useT';
import { FRAME } from '@/core/constants';
import * as THREE from 'three';

function quaternionToRPY(q: { x: number; y: number; z: number; w: number }) {
  const euler = new THREE.Euler().setFromQuaternion(
    new THREE.Quaternion(q.x, q.y, q.z, q.w),
    'ZYX',
  );
  return { roll: euler.x, pitch: euler.y, yaw: euler.z };
}

export function CartesianJogPanel() {
  const t = useT();
  const robotState = useRobotStateStore((s) => s.robotState);
  const currentFrame = useControlStore((s) => s.currentFrame);

  const getPoseData = () => {
    if (!robotState) return null;

    switch (currentFrame) {
      case FRAME.FLANGE:
        return robotState.flange ?? robotState.flange_pose;
      case FRAME.TOOL:
        return robotState.tool ?? robotState.tool_pose;
      case FRAME.OBJECT:
        return robotState.object ?? robotState.object_pose;
      default:
        return robotState.flange ?? robotState.flange_pose;
    }
  };

  const pose = getPoseData();
  const rpy = pose ? quaternionToRPY(pose.orientation) : { roll: 0, pitch: 0, yaw: 0 };

  const getFrameLabel = () => {
    switch (currentFrame) {
      case FRAME.BASE: return t('frame.BASE');
      case FRAME.FLANGE: return t('frame.FLANGE');
      case FRAME.TOOL: return t('frame.TOOL');
      case FRAME.OBJECT: return t('frame.OBJECT');
      default: return t('frame.BASE');
    }
  };

  return (
    <div className="panel-section">
      <div className="panel-section-header">
        {t('jog.cartesianSpace')} (<span className="frame-highlight">{getFrameLabel()}</span>)
      </div>
      <div>
        <CartesianJogItem
          label="X"
          value={pose?.position.x || 0}
          frame={currentFrame}
          freedom={0}
          unit="mm"
          isPosition={true}
        />
        <CartesianJogItem
          label="Y"
          value={pose?.position.y || 0}
          frame={currentFrame}
          freedom={1}
          unit="mm"
          isPosition={true}
        />
        <CartesianJogItem
          label="Z"
          value={pose?.position.z || 0}
          frame={currentFrame}
          freedom={2}
          unit="mm"
          isPosition={true}
        />
        <CartesianJogItem
          label="R"
          value={rpy.roll}
          frame={currentFrame}
          freedom={3}
          unit="deg"
          isPosition={false}
        />
        <CartesianJogItem
          label="P"
          value={rpy.pitch}
          frame={currentFrame}
          freedom={4}
          unit="deg"
          isPosition={false}
        />
        <CartesianJogItem
          label="Y"
          value={rpy.yaw}
          frame={currentFrame}
          freedom={5}
          unit="deg"
          isPosition={false}
        />
      </div>
    </div>
  );
}
