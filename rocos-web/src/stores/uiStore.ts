import { create } from 'zustand';
import { persist } from 'zustand/middleware';

export type ThemeMode = 'dark' | 'light';

interface UIState {
  showAxes: boolean;
  showWireframe: boolean;
  showGround: boolean;
  showTrajectory: boolean;
  currentView: 'scene' | 'plot';
  themeMode: ThemeMode;

  toggleAxes: () => void;
  toggleWireframe: () => void;
  toggleGround: () => void;
  toggleTrajectory: () => void;
  setView: (view: 'scene' | 'plot') => void;
  toggleTheme: () => void;
}

export const useUIStore = create<UIState>()(
  persist(
    (set) => ({
      showAxes: true,
      showWireframe: false,
      showGround: true,
      showTrajectory: false,
      currentView: 'scene',
      themeMode: 'dark',

      toggleAxes: () => set((state) => ({ showAxes: !state.showAxes })),
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
