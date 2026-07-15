import { create } from 'zustand';
import type { RobotState } from '@/core/types';

interface RobotStateStore {
  robotState: RobotState | null;
  updateState: (state: RobotState) => void;
  clear: () => void;
}

const defaultPose = { position: { x: 0, y: 0, z: 0 }, orientation: { x: 0, y: 0, z: 0, w: 1 } };

const initialState: RobotState = {
  joint_states: [],
  flange: defaultPose,
  tool: defaultPose,
  object: defaultPose,
  hw_state: {},
};

export const useRobotStateStore = create<RobotStateStore>((set) => ({
  robotState: initialState,

  updateState: (state) => set({ robotState: state }),

  clear: () => set({ robotState: initialState }),
}));
