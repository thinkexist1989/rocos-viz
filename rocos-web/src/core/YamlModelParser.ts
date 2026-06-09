import * as THREE from 'three';
import * as yaml from 'js-yaml';
import type { LinkConfig, RobotModelConfig } from './types';
import { LINK_TYPE } from './constants';

export interface ParsedLink {
  config: LinkConfig;
  linkType: number;
  jointNode: THREE.Object3D;
  meshContainer: THREE.Object3D;
  mesh?: THREE.Mesh;
}

function toVec3(val: any): [number, number, number] | null {
  if (!val) return null;
  if (Array.isArray(val)) {
    return [val[0] ?? 0, val[1] ?? 0, val[2] ?? 0];
  }
  if (typeof val === 'object' && 'x' in val) {
    return [val.x ?? 0, val.y ?? 0, val.z ?? 0];
  }
  return null;
}

export class YamlModelParser {
  private links: ParsedLink[] = [];

  async parse(yamlContent: string, meshBaseUrl: string): Promise<THREE.Group> {
    const raw = yaml.load(yamlContent) as any;
    // C++ writeModelFiles format: { robot: [ {name, order, ...}, ... ] }
    // Also accept flat { name, links: [...] } from direct API
    let links: LinkConfig[];
    if (raw.robot && Array.isArray(raw.robot)) {
      links = raw.robot;
    } else if (raw.links && Array.isArray(raw.links)) {
      links = raw.links;
    } else {
      throw new Error('Robot model YAML has no links');
    }
    const sortedLinks = [...links].sort((a, b) => a.order - b.order);

    const robotGroup = new THREE.Group();
    robotGroup.name = 'robot';

    let parent: THREE.Object3D = robotGroup;

    for (const linkConfig of sortedLinks) {
      const parsed = await this.parseLink(linkConfig, meshBaseUrl);
      parent.add(parsed.jointNode);
      parent = parsed.jointNode;
      this.links.push(parsed);
    }

    return robotGroup;
  }

  private async parseLink(config: LinkConfig, meshBaseUrl: string): Promise<ParsedLink> {
    const linkType = this.resolveLinkType(config.type);

    const jointNode = new THREE.Object3D();
    jointNode.name = `joint_${config.name}`;
    jointNode.userData = { linkConfig: config, linkType };

    const translate = toVec3(config.translate);
    if (translate) {
      jointNode.position.set(...translate);
      config.translate = translate;
    }

    const rotate = toVec3(config.rotate);
    if (rotate) {
      const euler = new THREE.Euler(rotate[0], rotate[1], rotate[2], 'ZYX');
      jointNode.quaternion.setFromEuler(euler);
      config.rotate = rotate;
    }

    const meshContainer = new THREE.Object3D();
    meshContainer.name = `mesh_container_${config.name}`;

    const translateLink = toVec3(config.translateLink);
    if (translateLink) {
      meshContainer.position.set(...translateLink);
      config.translateLink = translateLink;
    }

    const rotateLink = toVec3(config.rotateLink);
    if (rotateLink) {
      const euler = new THREE.Euler(rotateLink[0], rotateLink[1], rotateLink[2], 'ZYX');
      meshContainer.quaternion.setFromEuler(euler);
      config.rotateLink = rotateLink;
    }

    // Normalize angleAxis too
    const angleAxis = toVec3(config.angleAxis);
    if (angleAxis) {
      config.angleAxis = angleAxis;
    }

    jointNode.add(meshContainer);

    let mesh: THREE.Mesh | undefined;
    if (config.mesh) {
      mesh = await this.loadMesh(config.mesh, meshBaseUrl);
      if (mesh) {
        meshContainer.add(mesh);
      }
    }

    return { config, linkType, jointNode, meshContainer, mesh };
  }

  private resolveLinkType(typeStr?: string): number {
    if (!typeStr) return LINK_TYPE.REVOLUTE;
    const upper = typeStr.toUpperCase();
    return LINK_TYPE[upper as keyof typeof LINK_TYPE] ?? LINK_TYPE.REVOLUTE;
  }

  private async loadMesh(filename: string, baseUrl: string): Promise<THREE.Mesh | undefined> {
    const ext = filename.split('.').pop()?.toLowerCase();
    const url = `${baseUrl}?path=${encodeURIComponent(filename)}`;

    try {
      const response = await fetch(url);
      const blob = await response.blob();
      const arrayBuffer = await blob.arrayBuffer();

      if (ext === 'stl') {
        return this.loadSTL(arrayBuffer);
      } else if (ext === 'dae') {
        return this.loadDAE(arrayBuffer);
      }

      console.warn(`Unsupported mesh format: ${ext}`);
      return undefined;
    } catch (error) {
      console.error(`Failed to load mesh: ${filename}`, error);
      return undefined;
    }
  }

  private loadSTL(arrayBuffer: ArrayBuffer): THREE.Mesh {
    const geometry = new THREE.BufferGeometry();
    const positions: number[] = [];

    const view = new DataView(arrayBuffer);

    if (this.isBinarySTL(arrayBuffer)) {
      const numTriangles = view.getUint32(80, true);
      let offset = 84;

      for (let i = 0; i < numTriangles; i++) {
        offset += 12;
        for (let j = 0; j < 3; j++) {
          positions.push(
            view.getFloat32(offset, true),
            view.getFloat32(offset + 4, true),
            view.getFloat32(offset + 8, true),
          );
          offset += 12;
        }
        offset += 2;
      }
    } else {
      const text = new TextDecoder().decode(arrayBuffer);
      const lines = text.split('\n');
      for (const line of lines) {
        if (line.trim().startsWith('vertex')) {
          const parts = line.trim().split(/\s+/);
          positions.push(
            parseFloat(parts[1]),
            parseFloat(parts[2]),
            parseFloat(parts[3]),
          );
        }
      }
    }

    geometry.setAttribute('position', new THREE.Float32BufferAttribute(positions, 3));
    geometry.computeVertexNormals();

    const material = new THREE.MeshStandardMaterial({
      color: 0x808080,
      metalness: 0.3,
      roughness: 0.7,
    });

    return new THREE.Mesh(geometry, material);
  }

  private isBinarySTL(arrayBuffer: ArrayBuffer): boolean {
    const view = new DataView(arrayBuffer);
    if (arrayBuffer.byteLength < 84) return false;

    const numTriangles = view.getUint32(80, true);
    const expectedSize = 84 + numTriangles * 50;
    return arrayBuffer.byteLength === expectedSize;
  }

  private loadDAE(arrayBuffer: ArrayBuffer): THREE.Mesh {
    const text = new TextDecoder().decode(arrayBuffer);
    const parser = new DOMParser();
    const doc = parser.parseFromString(text, 'text/xml');

    const geometries = doc.getElementsByTagName('geometry');
    if (geometries.length === 0) {
      console.warn('No geometry found in DAE file');
      return new THREE.Mesh(
        new THREE.BoxGeometry(0.1, 0.1, 0.1),
        new THREE.MeshStandardMaterial({ color: 0xff0000 }),
      );
    }

    const positions: number[] = [];
    const indices: number[] = [];

    for (let i = 0; i < geometries.length; i++) {
      const mesh = geometries[i].getElementsByTagName('mesh')[0];
      if (!mesh) continue;

      const source = mesh.getElementsByTagName('source')[0];
      const floatArray = source.getElementsByTagName('float_array')[0];
      const values = floatArray.textContent!.trim().split(/\s+/).map(parseFloat);

      const triangles = mesh.getElementsByTagName('triangles')[0];
      if (triangles) {
        const indexArray = triangles.getElementsByTagName('p')[0].textContent!.trim().split(/\s+/).map(parseInt);
        const stride = parseInt(triangles.getElementsByTagName('input')[0].getAttribute('offset') || '0') + 1;

        for (let j = 0; j < indexArray.length; j += stride) {
          const vertexIndex = indexArray[j];
          indices.push(vertexIndex);
        }

        for (let j = 0; j < values.length; j += 3) {
          positions.push(values[j], values[j + 1], values[j + 2]);
        }
      }
    }

    const geometry = new THREE.BufferGeometry();
    geometry.setAttribute('position', new THREE.Float32BufferAttribute(positions, 3));
    if (indices.length > 0) {
      geometry.setIndex(indices);
    }
    geometry.computeVertexNormals();

    const material = new THREE.MeshStandardMaterial({
      color: 0x808080,
      metalness: 0.3,
      roughness: 0.7,
    });

    return new THREE.Mesh(geometry, material);
  }

  getLinks(): ParsedLink[] {
    return this.links;
  }

  dispose(): void {
    for (const link of this.links) {
      if (link.mesh) {
        link.mesh.geometry.dispose();
        if (Array.isArray(link.mesh.material)) {
          link.mesh.material.forEach((m) => m.dispose());
        } else {
          link.mesh.material.dispose();
        }
      }
    }
    this.links = [];
  }
}
