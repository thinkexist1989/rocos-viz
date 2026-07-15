import { useEffect } from 'react';
import { useThree } from '@react-three/fiber';
import * as THREE from 'three';
import { RoomEnvironment } from 'three/examples/jsm/environments/RoomEnvironment.js';

/**
 * Sets scene.environment from three.js's built-in RoomEnvironment, generated
 * procedurally on the GPU via PMREM. This replaces drei's Environment preset,
 * which fetches an HDR file from a remote CDN; that download fails or hangs on
 * machines without reliable internet (e.g. Android tablets / Macs on a LAN-only
 * network), leaving MeshStandardMaterial surfaces black. RoomEnvironment needs
 * no network, so the model lights consistently everywhere.
 *
 * Per-surface strength is tuned via each material's envMapIntensity (set in
 * YamlModelParser), since scene.environmentIntensity only exists in three >= r163.
 */
export function LocalEnvironment() {
  const { gl, scene } = useThree();

  useEffect(() => {
    const pmrem = new THREE.PMREMGenerator(gl);
    const envScene = new RoomEnvironment();
    const envMap = pmrem.fromScene(envScene, 0.04).texture;

    const prev = scene.environment;
    scene.environment = envMap;

    return () => {
      scene.environment = prev;
      envMap.dispose();
      pmrem.dispose();
      // RoomEnvironment is a Scene; dispose its geometries/materials.
      envScene.traverse((obj) => {
        const mesh = obj as THREE.Mesh;
        if (mesh.geometry) mesh.geometry.dispose();
        const mat = mesh.material as THREE.Material | THREE.Material[] | undefined;
        if (Array.isArray(mat)) mat.forEach((m) => m.dispose());
        else mat?.dispose();
      });
    };
  }, [gl, scene]);

  return null;
}
