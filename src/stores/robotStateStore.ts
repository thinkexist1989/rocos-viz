import { create } from 'zustand';
import type { RobotState } from '@/core/types';

interface RobotStateStore {
  robotState: RobotState | null;
  updateState: (state: RobotState) => void;
  clear: () => void;
}

const initialState: RobotState = {
  joint_states: [],
  hw_state: {},
};

export const useRobotStateStore = create<RobotStateStore>((set) => ({
  robotState: initialState,

  updateState: (state) => set({ robotState: state }),

  clear: () => set({ robotState: initialState }),
}));
