import { create } from 'zustand';
import { FRAME, DEFAULT_SPEED_FACTOR } from '@/core/constants';

interface ControlState {
  speedFactor: number;
  currentFrame: number;
  isDegree: boolean;
  isMM: boolean;

  setSpeedFactor: (factor: number) => void;
  setFrame: (frame: number) => void;
  toggleDegree: () => void;
  toggleMM: () => void;
  reset: () => void;
}

export const useControlStore = create<ControlState>((set) => ({
  speedFactor: DEFAULT_SPEED_FACTOR,
  currentFrame: FRAME.BASE,
  isDegree: true,
  isMM: true,

  setSpeedFactor: (factor) => set({ speedFactor: factor }),
  setFrame: (frame) => set({ currentFrame: frame }),
  toggleDegree: () => set((state) => ({ isDegree: !state.isDegree })),
  toggleMM: () => set((state) => ({ isMM: !state.isMM })),
  reset: () =>
    set({
      speedFactor: DEFAULT_SPEED_FACTOR,
      currentFrame: FRAME.BASE,
      isDegree: true,
      isMM: true,
    }),
}));
