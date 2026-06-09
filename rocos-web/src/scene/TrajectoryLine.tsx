import { useMemo, useRef, useEffect } from 'react';
import * as THREE from 'three';
import { useFrame } from '@react-three/fiber';
import { MAX_TRAJECTORY_POINTS } from '@/core/constants';

interface TrajectoryLineProps {
  points: THREE.Vector3[];
  visible: boolean;
}

function LineObject({ geometry, visible, points }: { geometry: THREE.BufferGeometry; visible: boolean; points: THREE.Vector3[] }) {
  const lineRef = useRef<THREE.Line>(null);

  useFrame(() => {
    if (!lineRef.current || points.length < 2) {
      geometry.setDrawRange(0, 0);
      return;
    }

    const positions = geometry.getAttribute('position') as THREE.BufferAttribute;
    for (let i = 0; i < points.length && i < MAX_TRAJECTORY_POINTS; i++) {
      positions.setXYZ(i, points[i].x, points[i].y, points[i].z);
    }
    positions.needsUpdate = true;
    geometry.setDrawRange(0, Math.min(points.length, MAX_TRAJECTORY_POINTS));
  });

  if (!visible || points.length < 2) return null;

  return (
    <primitive ref={lineRef} object={new THREE.Line(geometry, new THREE.LineBasicMaterial({ color: 0x496fff, linewidth: 2 }))} />
  );
}

export function TrajectoryLine({ points, visible }: TrajectoryLineProps) {
  const geometry = useMemo(() => {
    const geo = new THREE.BufferGeometry();
    const positions = new Float32Array(MAX_TRAJECTORY_POINTS * 3);
    geo.setAttribute('position', new THREE.BufferAttribute(positions, 3));
    geo.setDrawRange(0, 0);
    return geo;
  }, []);

  return <LineObject geometry={geometry} visible={visible} points={points} />;
}
