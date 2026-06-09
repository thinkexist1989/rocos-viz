import { Grid, Environment } from '@react-three/drei';

interface SceneHelpersProps {
  showGround: boolean;
}

export function SceneHelpers({ showGround }: SceneHelpersProps) {
  return (
    <>
      <ambientLight intensity={0.3} />
      <directionalLight position={[5, -5, 8]} intensity={1.0} castShadow />
      <directionalLight position={[-3, 3, 5]} intensity={0.4} color="#b0c4de" />
      <spotLight
        position={[0, 0, 6]}
        angle={0.4}
        penumbra={0.5}
        intensity={0.6}
        color="#ffffff"
        castShadow
      />

      <Environment preset="city" background={false} />

      {showGround && (
        <Grid
          position={[0, 0, 0]}
          rotation={[-Math.PI / 2, 0, 0]}
          args={[20, 20]}
          cellSize={0.1}
          cellThickness={0.5}
          cellColor="#3c3f47"
          sectionSize={1}
          sectionThickness={1}
          sectionColor="#555860"
          fadeDistance={15}
          fadeStrength={1}
          infiniteGrid
        />
      )}
    </>
  );
}
