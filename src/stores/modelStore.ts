import { create } from 'zustand';
import type * as THREE from 'three';
import type { RobotModelConfig } from '@/core/types';

interface ModelState {
  /** The loaded robot model — can be a THREE.Group (legacy YAML) or URDFRobot (Object3D). */
  model: THREE.Object3D | null;
  config: RobotModelConfig | null;
  isLoading: boolean;

  setModel: (model: THREE.Object3D, config: RobotModelConfig) => void;
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
