import { Canvas } from '@react-three/fiber';
import { OrbitControls } from '@react-three/drei';
import { useUIStore } from '@/stores/uiStore';
import { SceneHelpers } from './SceneHelpers';
import { CameraPresets } from './CameraPresets';
import * as THREE from 'three';

interface RobotViewerProps {
  children?: React.ReactNode;
}

export function RobotViewer({ children }: RobotViewerProps) {
  const showGround = useUIStore((s) => s.showGround);

  return (
    <Canvas
      camera={{
        position: [3, -3, 3],
        fov: 50,
        near: 0.01,
        far: 1000,
        up: [0, 0, 1],
      }}
      gl={{
        antialias: true,
        toneMapping: THREE.ACESFilmicToneMapping,
        toneMappingExposure: 1.2,
      }}
      style={{ width: '100%', height: '100%', background: '#111318' }}
    >
      <SceneHelpers showGround={showGround} />
      <OrbitControls
        target={[0, 0, 0.5]}
        enableDamping
        dampingFactor={0.1}
        minDistance={0.5}
        maxDistance={50}
      />
      <CameraPresets />
      {children}
    </Canvas>
  );
}
