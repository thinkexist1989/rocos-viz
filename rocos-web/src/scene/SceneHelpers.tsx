import { Grid, Environment } from '@react-three/drei';
import { useUIStore } from '@/stores/uiStore';

interface SceneHelpersProps {
  showGround: boolean;
}

export function SceneHelpers({ showGround }: SceneHelpersProps) {
  const isLight = useUIStore((s) => s.themeMode === 'light');

  return (
    <>
      <ambientLight intensity={isLight ? 0.6 : 0.3} />
      <directionalLight
        position={[5, -5, 8]}
        intensity={isLight ? 0.8 : 1.0}
        castShadow
      />
      <directionalLight
        position={[-3, 3, 5]}
        intensity={isLight ? 0.3 : 0.4}
        color={isLight ? '#e0e0e0' : '#b0c4de'}
      />
      {!isLight && (
        <spotLight
          position={[0, 0, 6]}
          angle={0.4}
          penumbra={0.5}
          intensity={0.6}
          color="#ffffff"
          castShadow
        />
      )}

      <Environment preset={isLight ? 'studio' : 'city'} background={false} />

      {showGround && (
        <Grid
          position={[0, 0, 0]}
          rotation={[-Math.PI / 2, 0, 0]}
          args={[20, 20]}
          cellSize={0.1}
          cellThickness={0.5}
          cellColor={isLight ? '#d1d5db' : '#3c3f47'}
          sectionSize={1}
          sectionThickness={1}
          sectionColor={isLight ? '#9ca3af' : '#555860'}
          fadeDistance={15}
          fadeStrength={1}
          infiniteGrid
        />
      )}
    </>
  );
}
