import { useRef, useEffect } from 'react';
import * as THREE from 'three';
import { useFrame } from '@react-three/fiber';
import { YamlModelParser, type ParsedLink } from '@/core/YamlModelParser';
import { ForwardKinematics } from '@/core/ForwardKinematics';
import { useRobotStateStore } from '@/stores/robotStateStore';
import { useUIStore } from '@/stores/uiStore';

interface RobotModelProps {
  yamlContent: string | null;
  meshBaseUrl: string;
}

export function RobotModel({ yamlContent, meshBaseUrl }: RobotModelProps) {
  const groupRef = useRef<THREE.Group>(null);
  const parserRef = useRef<YamlModelParser>(new YamlModelParser());
  const fkRef = useRef<ForwardKinematics>(new ForwardKinematics());
  const linksRef = useRef<ParsedLink[]>([]);

  const robotState = useRobotStateStore((s) => s.robotState);
  const showWireframe = useUIStore((s) => s.showWireframe);

  useEffect(() => {
    let cancelled = false;

    async function loadModel() {
      if (!yamlContent) return;

      try {
        if (groupRef.current) {
          while (groupRef.current.children.length > 0) {
            groupRef.current.remove(groupRef.current.children[0]);
          }
        }

        const parser = parserRef.current;
        parser.dispose();

        const robotGroup = await parser.parse(yamlContent, meshBaseUrl);

        if (!cancelled && groupRef.current) {
          groupRef.current.add(robotGroup);
          linksRef.current = parser.getLinks();
          fkRef.current.setLinks(linksRef.current);
        }
      } catch (error) {
        console.error('Failed to load robot model:', error);
      }
    }

    loadModel();

    return () => {
      cancelled = true;
      parserRef.current.dispose();
    };
  }, [yamlContent, meshBaseUrl]);

  useFrame(() => {
    if (!robotState || robotState.joint_states.length === 0) return;

    const jointAngles = robotState.joint_states.map((js) => js.position);
    fkRef.current.update(jointAngles);

    for (const link of linksRef.current) {
      if (link.mesh) {
        (link.mesh.material as THREE.MeshStandardMaterial).wireframe = showWireframe;
      }
    }
  });

  return <group ref={groupRef} />;
}
