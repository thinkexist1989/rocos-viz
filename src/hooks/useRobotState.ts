import { useEffect, useRef } from 'react';
import { useConnectionStore } from '@/stores/connectionStore';
import { useRobotStateStore } from '@/stores/robotStateStore';
import { useControlStore } from '@/stores/controlStore';
import { RobotApiClient } from '@/core/RobotApiClient';

export function useRobotState() {
  const robotState = useRobotStateStore((s) => s.robotState);
  const isConnected = useConnectionStore((s) => s.isConnected);

  return {
    robotState,
    isConnected,
    jointAngles: robotState?.joint_states.map((js) => js.position) || [],
    flangePose: robotState?.flange ?? robotState?.flange_pose,
    toolPose: robotState?.tool ?? robotState?.tool_pose,
    hardware: robotState?.hw_state ?? robotState?.hardware,
  };
}
