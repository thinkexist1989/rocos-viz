import { useRef } from 'react';
import * as THREE from 'three';
import type { LinkConfig } from '@/core/types';
import { LINK_TYPE } from '@/core/constants';

interface LinkComponentProps {
  meshFile?: string;
}

export function LinkComponent({ meshFile }: LinkComponentProps) {
  if (!meshFile) {
    return null;
  }

  return (
    <mesh>
      <boxGeometry args={[0.1, 0.1, 0.1]} />
      <meshStandardMaterial color="#808080" metalness={0.3} roughness={0.7} />
    </mesh>
  );
}
