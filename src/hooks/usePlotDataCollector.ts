import { useEffect } from 'react';
import { useRobotStateStore } from '@/stores/robotStateStore';
import { usePlotStore, DataSourceKey } from '@/stores/plotStore';

export function usePlotDataCollector() {
  const robotState = useRobotStateStore((s) => s.robotState);
  const appendData = usePlotStore((s) => s.appendData);

  useEffect(() => {
    if (!robotState) return;

    robotState.joint_states.forEach((joint, index) => {
      appendData(`joint_position_${index}`, joint.position);
      appendData(`joint_velocity_${index}`, joint.velocity);
      appendData(`joint_torque_${index}`, joint.torque);
      appendData(`joint_load_${index}`, joint.load ?? joint.load_torque ?? 0);
    });

    const flange = robotState.flange;
    if (flange) {
      appendData('flange_x', flange.position.x);
      appendData('flange_y', flange.position.y);
      appendData('flange_z', flange.position.z);
    }
  }, [robotState, appendData]);
}

export function getChartDataKey(source: DataSourceKey, jointIndex?: number): string {
  if (source.startsWith('joint_') && jointIndex !== undefined) {
    return `${source}_${jointIndex}`;
  }
  return source;
}
