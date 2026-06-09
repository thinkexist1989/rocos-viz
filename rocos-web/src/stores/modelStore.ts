import { create } from 'zustand';
import type * as THREE from 'three';
import type { RobotModelConfig } from '@/core/types';

interface ModelState {
  model: THREE.Group | null;
  config: RobotModelConfig | null;
  isLoading: boolean;

  setModel: (model: THREE.Group, config: RobotModelConfig) => void;
  setLoading: (loading: boolean) => void;
  clear: () => void;
}

export const useModelStore = create<ModelState>((set) => ({
  model: null,
  config: null,
  isLoading: false,

  setModel: (model, config) => set({ model, config, isLoading: false }),
  setLoading: (loading) => set({ isLoading: loading }),
  clear: () => set({ model: null, config: null, isLoading: false }),
}));
