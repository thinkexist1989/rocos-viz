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
  const isLight = useUIStore((s) => s.themeMode === 'light');

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
        toneMappingExposure: isLight ? 1.0 : 1.2,
      }}
      shadows={isLight ? { type: THREE.PCFSoftShadowMap } : undefined}
      style={{
        width: '100%',
        height: '100%',
        background: isLight ? '#f8f9fa' : '#111318',
      }}
    >
      <color attach="background" args={[isLight ? '#f8f9fa' : '#111318']} />
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
