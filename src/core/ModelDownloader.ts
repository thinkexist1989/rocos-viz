import { RobotApiClient } from '@/core/RobotApiClient';

const DB_NAME = 'rocos-model-cache';
const DB_VERSION = 1;
const STORE_NAME = 'meshes';

interface CachedMesh {
  key: string;
  blob: Blob;
  timestamp: number;
}

function openDB(): Promise<IDBDatabase> {
  return new Promise((resolve, reject) => {
    const request = indexedDB.open(DB_NAME, DB_VERSION);

    request.onupgradeneeded = () => {
      const db = request.result;
      if (!db.objectStoreNames.contains(STORE_NAME)) {
        db.createObjectStore(STORE_NAME, { keyPath: 'key' });
      }
    };

    request.onsuccess = () => resolve(request.result);
    request.onerror = () => reject(request.error);
  });
}

async function getCachedMesh(key: string): Promise<Blob | null> {
  try {
    const db = await openDB();
    return new Promise((resolve) => {
      const tx = db.transaction(STORE_NAME, 'readonly');
      const store = tx.objectStore(STORE_NAME);
      const request = store.get(key) as IDBRequest<CachedMesh | undefined>;

      request.onsuccess = () => {
        resolve(request.result?.blob ?? null);
      };
      request.onerror = () => resolve(null);
    });
  } catch {
    return null;
  }
}

async function cacheMesh(key: string, blob: Blob): Promise<void> {
  try {
    const db = await openDB();
    return new Promise((resolve) => {
      const tx = db.transaction(STORE_NAME, 'readwrite');
      const store = tx.objectStore(STORE_NAME);
      store.put({ key, blob, timestamp: Date.now() });
      tx.oncomplete = () => resolve();
    });
  } catch {
    // Ignore cache errors
  }
}

export interface ModelDownloadProgress {
  total: number;
  loaded: number;
  currentFile: string;
}

export async function downloadModelWithCache(
  host: string,
  port: string,
  onProgress?: (progress: ModelDownloadProgress) => void,
): Promise<{ yaml: string; meshes: Map<string, Blob> }> {
  const client = new RobotApiClient(host, port);

  const modelConfig = await client.getRobotModel();
  const yamlContent = (modelConfig as any).yaml ?? '';

  const meshFiles = new Set<string>();
  const links = modelConfig.links || [];
  for (const link of links) {
    if (link.mesh) {
      meshFiles.add(link.mesh);
    }
  }

  const meshList = Array.from(meshFiles);
  const meshes = new Map<string, Blob>();
  let loaded = 0;

  const downloadOne = async (meshPath: string) => {
    const cacheKey = `${host}:${port}_${meshPath}`;

    let blob = await getCachedMesh(cacheKey);
    if (blob) {
      meshes.set(meshPath, blob);
      loaded++;
      onProgress?.({ total: meshList.length, loaded, currentFile: meshPath });
      return;
    }

    blob = await client.downloadMesh(meshPath);
    meshes.set(meshPath, blob);
    await cacheMesh(cacheKey, blob);

    loaded++;
    onProgress?.({ total: meshList.length, loaded, currentFile: meshPath });
  };

  const CONCURRENCY = 4;
  for (let i = 0; i < meshList.length; i += CONCURRENCY) {
    const batch = meshList.slice(i, i + CONCURRENCY);
    await Promise.all(batch.map(downloadOne));
  }

  return { yaml: yamlContent, meshes };
}
