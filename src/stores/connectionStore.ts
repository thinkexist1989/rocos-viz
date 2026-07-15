import { create } from 'zustand';
import { persist } from 'zustand/middleware';

interface ConnectionState {
  host: string;
  port: string;
  isConnected: boolean;
  isRobotEnabled: boolean;

  setConnection: (host: string, port: string) => void;
  setConnected: (connected: boolean) => void;
  setEnabled: (enabled: boolean) => void;
  reset: () => void;
}

export const useConnectionStore = create<ConnectionState>()(
  persist(
    (set) => ({
      host: window.location.hostname,
      port: '8080',
      isConnected: false,
      isRobotEnabled: false,

      setConnection: (host, port) => set({ host, port }),
      setConnected: (connected) => set({ isConnected: connected }),
      setEnabled: (enabled) => set({ isRobotEnabled: enabled }),
      reset: () => set({ isConnected: false, isRobotEnabled: false }),
    }),
    {
      name: 'rocos-connection',
    },
  ),
);
