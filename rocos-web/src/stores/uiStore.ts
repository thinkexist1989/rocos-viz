import { create } from 'zustand';
import { persist } from 'zustand/middleware';

export type ThemeMode = 'dark' | 'light';

interface UIState {
  showJointFrames: boolean;
  showWireframe: boolean;
  showGround: boolean;
  showTrajectory: boolean;
  currentView: 'scene' | 'plot';
  themeMode: ThemeMode;

  toggleJointFrames: () => void;
  toggleWireframe: () => void;
  toggleGround: () => void;
  toggleTrajectory: () => void;
  setView: (view: 'scene' | 'plot') => void;
  toggleTheme: () => void;
}

export const useUIStore = create<UIState>()(
  persist(
    (set) => ({
      showJointFrames: false,
      showWireframe: false,
      showGround: true,
      showTrajectory: false,
      currentView: 'scene',
      themeMode: 'dark',

      toggleJointFrames: () => set((state) => ({ showJointFrames: !state.showJointFrames })),
      toggleWireframe: () => set((state) => ({ showWireframe: !state.showWireframe })),
      toggleGround: () => set((state) => ({ showGround: !state.showGround })),
      toggleTrajectory: () => set((state) => ({ showTrajectory: !state.showTrajectory })),
      setView: (view) => set({ currentView: view }),
      toggleTheme: () =>
        set((state) => ({ themeMode: state.themeMode === 'dark' ? 'light' : 'dark' })),
    }),
    { name: 'rocos-ui' },
  ),
);
