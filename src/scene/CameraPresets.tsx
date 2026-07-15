import { useRef } from 'react';
import { useThree } from '@react-three/fiber';
import * as THREE from 'three';

const CAMERA_PRESETS = {
  axonometric: { position: [3, -3, 3] as [number, number, number], target: [0, 0, 0.5] as [number, number, number] },
  top: { position: [0, 0, 5] as [number, number, number], target: [0, 0, 0.5] as [number, number, number] },
  front: { position: [0, -5, 0.5] as [number, number, number], target: [0, 0, 0.5] as [number, number, number] },
  right: { position: [5, 0, 0.5] as [number, number, number], target: [0, 0, 0.5] as [number, number, number] },
};

type CameraPresetKey = keyof typeof CAMERA_PRESETS;

let currentPreset: CameraPresetKey | null = null;
let onPresetChange: ((key: CameraPresetKey) => void) | null = null;

export function setCameraPreset(key: CameraPresetKey) {
  currentPreset = key;
  onPresetChange?.(key);
}

export function CameraPresets() {
  const { camera } = useThree();
  const appliedRef = useRef<string | null>(null);

  onPresetChange = (key) => {
    const preset = CAMERA_PRESETS[key];
    if (!preset) return;

    camera.position.set(...preset.position);
    camera.lookAt(...preset.target);
    camera.updateProjectionMatrix();
    appliedRef.current = key;
  };

  return null;
}
