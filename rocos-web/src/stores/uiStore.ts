import { create } from 'zustand';

interface UIState {
  showAxes: boolean;
  showWireframe: boolean;
  showGround: boolean;
  showTrajectory: boolean;
  currentView: 'scene' | 'plot';

  toggleAxes: () => void;
  toggleWireframe: () => void;
  toggleGround: () => void;
  toggleTrajectory: () => void;
  setView: (view: 'scene' | 'plot') => void;
}

export const useUIStore = create<UIState>((set) => ({
  showAxes: true,
  showWireframe: false,
  showGround: true,
  showTrajectory: false,
  currentView: 'scene',

  toggleAxes: () => set((state) => ({ showAxes: !state.showAxes })),
  toggleWireframe: () => set((state) => ({ showWireframe: !state.showWireframe })),
  toggleGround: () => set((state) => ({ showGround: !state.showGround })),
  toggleTrajectory: () => set((state) => ({ showTrajectory: !state.showTrajectory })),
  setView: (view) => set({ currentView: view }),
}));
