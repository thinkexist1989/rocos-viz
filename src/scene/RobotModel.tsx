import { useRef, useEffect } from 'react';
import * as THREE from 'three';
import { useFrame } from '@react-three/fiber';
import { parseUrdf } from '@/core/UrdfModelLoader';
import { useRobotStateStore } from '@/stores/robotStateStore';
import { useUIStore } from '@/stores/uiStore';
import type { URDFRobot } from 'urdf-loader';

interface RobotModelProps {
  /** URDF XML content. When null/empty the robot is cleared. */
  urdfContent: string | null;
  /** Base URL for mesh downloads (default: /api/robot/urdf/mesh) */
  meshBasePath?: string;
}

/** Walk the robot tree and apply wireframe / transparency to every mesh. */
function applyVisualToggles(
  robot: URDFRobot,
  showWireframe: boolean,
  showJointFrames: boolean,
  helpers: THREE.AxesHelper[],
): void {
  robot.traverse((child: THREE.Object3D) => {
    if (!(child instanceof THREE.Mesh)) return;

    const materials: THREE.Material[] = Array.isArray(child.material)
      ? child.material
      : [child.material];

    for (const mat of materials) {
      // wireframe is on mesh-material subtypes, not on the base Material class
      const m = mat as THREE.Material & { wireframe?: boolean };
      m.transparent = showJointFrames;
      m.opacity = showJointFrames ? 0.3 : 1.0;
      m.depthWrite = !showJointFrames;
      m.wireframe = showWireframe;
      m.needsUpdate = true;
    }
  });

  for (const helper of helpers) {
    helper.visible = showJointFrames;
  }
}

export function RobotModel({
  urdfContent,
  meshBasePath = '/api/robot/urdf/mesh',
}: RobotModelProps) {
  const groupRef = useRef<THREE.Group>(null);
  const robotRef = useRef<URDFRobot | null>(null);
  const jointFrameHelpersRef = useRef<THREE.AxesHelper[]>([]);

  const robotState = useRobotStateStore((s) => s.robotState);
  const showWireframe = useUIStore((s) => s.showWireframe);
  const showJointFrames = useUIStore((s) => s.showJointFrames);

  // ── Load / reload the URDF model ──────────────────────────────
  useEffect(() => {
    let cancelled = false;

    async function loadModel() {
      if (!urdfContent) return;

      if (groupRef.current) {
        while (groupRef.current.children.length > 0) {
          groupRef.current.remove(groupRef.current.children[0]);
        }
      }
      disposeRobot(robotRef.current);
      robotRef.current = null;
      jointFrameHelpersRef.current = [];

      try {
        const { robot } = await parseUrdf(urdfContent, meshBasePath);

        if (cancelled) {
          disposeRobot(robot);
          return;
        }

        if (groupRef.current) {
          groupRef.current.add(robot);
          robotRef.current = robot;

          const helpers: THREE.AxesHelper[] = [];
          for (const jointName of Object.keys(robot.joints)) {
            const joint = robot.joints[jointName];
            const helper = new THREE.AxesHelper(0.08);
            helper.visible = showJointFrames;
            joint.add(helper);
            helpers.push(helper);
          }
          jointFrameHelpersRef.current = helpers;

          // Apply current toggle state to the freshly loaded model
          applyVisualToggles(robot, showWireframe, showJointFrames, helpers);
        }
      } catch (error) {
        console.error('Failed to load URDF model:', error);
      }
    }

    loadModel();

    return () => {
      cancelled = true;
    };
  }, [urdfContent, meshBasePath]);

  // ── Toggle visual effects on state change ────────────────────
  const toggleRef = useRef({ showWireframe, showJointFrames });
  useEffect(() => {
    toggleRef.current = { showWireframe, showJointFrames };
    const robot = robotRef.current;
    if (!robot) return;
    applyVisualToggles(robot, showWireframe, showJointFrames, jointFrameHelpersRef.current);
  }, [showWireframe, showJointFrames]);

  // ── Per-frame joint update ──────────────────────────────────
  useFrame(() => {
    const robot = robotRef.current;
    if (!robot || !robotState) return;

    if (robotState.joint_states?.length) {
      const jointValues: Record<string, number> = {};
      for (const js of robotState.joint_states) {
        jointValues[js.name] = js.position;
      }
      robot.setJointValues(jointValues);
    }
  });

  // ── Cleanup on unmount ───────────────────────────────────────
  useEffect(() => {
    return () => {
      disposeRobot(robotRef.current);
      robotRef.current = null;
    };
  }, []);

  return <group ref={groupRef} />;
}

function disposeRobot(robot: URDFRobot | null): void {
  if (!robot) return;
  robot.traverse((child: THREE.Object3D) => {
    if (child instanceof THREE.Mesh) {
      child.geometry?.dispose();
      const materials = Array.isArray(child.material)
        ? child.material
        : [child.material];
      for (const m of materials) {
        m.dispose();
      }
    }
  });
}
