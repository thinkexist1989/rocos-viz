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

    // Intercept mesh loading: strip package:// artifacts and send a clean
    // relative path so the server can fuzzy-search for the mesh file.
    loader.loadMeshCb = (meshPath: string, mgr, _material, onComplete) => {
      // Remove any ?query cruft and leading slashes / api prefixes left over
      // from a package:// → URL transformation.
      let cleaned = meshPath.split('?')[0];
      // If the packages function returned an API URL, extract just the
      // relative path portion (everything after the last "/mesh/" segment).
      const meshIdx = cleaned.lastIndexOf('/mesh/');
      if (meshIdx !== -1) {
        cleaned = cleaned.slice(meshIdx + 6); // after "/mesh/"
      }
      // Normalise: strip leading slash so it's a relative path.
      cleaned = cleaned.replace(/^\/+/, '');

      const apiUrl = `${meshBaseUrl}?path=${encodeURIComponent(cleaned)}`;

      console.log(`[UrdfModelLoader] Loading mesh: ${meshPath} → ${apiUrl}`);

      loader.defaultMeshLoader(apiUrl, mgr, _material, onComplete);
    };

    // Strip package:// prefix — the server will search by filename.
    // Returning '' causes resolvePath to produce just "/relPath" which
    // loadMeshCb normalises into a clean relative path.
    loader.packages = (_pkgName: string): string => {
      return '';
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
