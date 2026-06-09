import { Grid } from '@react-three/drei';

interface SceneHelpersProps {
  showGround: boolean;
}

export function SceneHelpers({ showGround }: SceneHelpersProps) {
  return (
    <>
      <ambientLight intensity={0.4} />
      <directionalLight position={[5, -5, 8]} intensity={0.8} castShadow />
      <directionalLight position={[-3, -3, 5]} intensity={0.3} />
      <pointLight position={[0, 0, 5]} intensity={0.2} />

      {showGround && (
        <Grid
          position={[0, 0, 0]}
          rotation={[-Math.PI / 2, 0, 0]}
          args={[20, 20]}
          cellSize={0.1}
          cellThickness={0.5}
          cellColor="#4a5568"
          sectionSize={1}
          sectionThickness={1}
          sectionColor="#718096"
          fadeDistance={15}
          fadeStrength={1}
          infiniteGrid
        />
      )}
    </>
  );
}
