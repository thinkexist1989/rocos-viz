/**
 * URDF Model Loader — wraps urdf-loader (gkjohnson) for use with the robot controller API.
 *
 * Configures URDFLoader to fetch meshes through /api/robot/urdf/mesh?path=
 * and provides a promise-based interface that waits for all resources to load.
 */
import { LoadingManager } from 'three';
import URDFLoader from 'urdf-loader';
import type { URDFRobot } from 'urdf-loader';

export interface UrdfLoadResult {
  robot: URDFRobot;
  loader: URDFLoader;
}

/**
 * Parse a URDF XML string into a URDFRobot, loading all meshes via the
 * controller's mesh endpoint.
 *
 * @param urdfContent  Raw URDF XML text
 * @param meshBaseUrl  Base URL for mesh downloads (default: /api/robot/urdf/mesh)
 * @returns Promise that resolves when the robot and all meshes are loaded
 */
export function parseUrdf(
  urdfContent: string,
  meshBaseUrl: string = '/api/robot/urdf/mesh',
): Promise<UrdfLoadResult> {
  return new Promise<UrdfLoadResult>((resolve, reject) => {
    const manager = new LoadingManager();

    let settled = false;
    let robot: URDFRobot;
    let loader: URDFLoader;

    manager.onLoad = () => {
      if (settled) return;
      settled = true;
      console.log('[UrdfModelLoader] All meshes loaded');
      resolve({ robot, loader });
    };

    manager.onError = (url: string) => {
      console.warn(`[UrdfModelLoader] Mesh load error: ${url}`);
    };

    manager.onProgress = (url: string, loaded: number, total: number) => {
      console.log(`[UrdfModelLoader] Mesh progress: ${loaded}/${total} — ${url}`);
    };

    loader = new URDFLoader(manager);

    // Only parse <visual> nodes — loading <collision> meshes as well
    // would render duplicate geometry on top of the visual model.
    loader.parseCollision = false;

    // Intercept mesh loading: extract the leaf filename and route through our API
    loader.loadMeshCb = (meshPath: string, mgr, _material, onComplete) => {
      // The path may include package:// resolution remnants — keep only the
      // leaf filename for the API.
      const cleaned = meshPath.split('?')[0];        // strip any ?query
      const filename = cleaned.split('/').pop() || cleaned;

      const apiUrl = `${meshBaseUrl}?path=${encodeURIComponent(filename)}`;

      console.log(`[UrdfModelLoader] Loading mesh: ${meshPath} → ${apiUrl}`);

      // Use the instance method (NOT URDFLoader.defaultMeshLoader — it's not static)
      loader.defaultMeshLoader(apiUrl, mgr, _material, onComplete);
    };

    // Resolve ROS package:// paths — map to the mesh base URL.
    // The package name becomes a query hint; the leaf filename is extracted in
    // loadMeshCb anyway, so this just needs to produce a resolvable path.
    loader.packages = (pkgName: string): string => {
      return `${meshBaseUrl}/${encodeURIComponent(pkgName)}`;
    };

    try {
      robot = loader.parse(urdfContent);

      if (!robot) {
        reject(new Error('URDF parsing returned null'));
        return;
      }

      console.log(`[UrdfModelLoader] Robot parsed: ${robot.robotName}`);

      // If there are no meshes in the URDF, the LoadingManager.onLoad may
      // never fire (itemsTotal stays 0). Resolve after a tick.
      const meshNodeCount = countMeshNodes(robot);
      if (meshNodeCount === 0) {
        console.log('[UrdfModelLoader] No mesh nodes — resolving immediately');
        setTimeout(() => {
          if (!settled) {
            settled = true;
            resolve({ robot, loader });
          }
        }, 0);
      } else {
        console.log(`[UrdfModelLoader] Found ${meshNodeCount} mesh node(s), waiting for load...`);
      }
    } catch (error) {
      reject(error instanceof Error ? error : new Error(String(error)));
    }
  });
}

/** Count URDFVisual / URDFCollider nodes that reference a mesh file. */
function countMeshNodes(robot: URDFRobot): number {
  let count = 0;
  robot.traverse((child: any) => {
    // URDFVisual and URDFCollider objects have an .urdfNode DOM element
    // that contains the <geometry><mesh filename="..."/> subtree.
    if (child.urdfNode) {
      const meshEl = child.urdfNode.querySelector('geometry mesh');
      if (meshEl?.getAttribute('filename')) {
        count++;
      }
    }
  });
  return count;
}
