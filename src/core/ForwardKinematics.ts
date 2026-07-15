import * as THREE from 'three';
import type { ParsedLink } from './YamlModelParser';
import { LINK_TYPE } from './constants';

export class ForwardKinematics {
  private links: ParsedLink[] = [];

  setLinks(links: ParsedLink[]): void {
    this.links = links;
  }

  update(jointAngles: number[]): void {
    let angleIndex = 0;

    for (const link of this.links) {
      const { linkType, jointNode, config } = link;

      if (linkType === LINK_TYPE.UNKNOWN || linkType === LINK_TYPE.FIXED) {
        continue;
      }

      if (linkType === LINK_TYPE.REVOLUTE || linkType === LINK_TYPE.CONTINUOUS) {
        if (angleIndex < jointAngles.length) {
          const angle = jointAngles[angleIndex];
          angleIndex++;

          const axisArr = config.angleAxis ?? [0, 0, 1];
          const axis = Array.isArray(axisArr)
            ? new THREE.Vector3(axisArr[0], axisArr[1], axisArr[2])
            : new THREE.Vector3(axisArr.x, axisArr.y, axisArr.z);
          axis.normalize();
          const q = new THREE.Quaternion().setFromAxisAngle(axis, angle);

          const baseRotation = new THREE.Quaternion();
          const rotateArr = config.rotate;
          if (rotateArr) {
            const r = Array.isArray(rotateArr)
              ? rotateArr
              : [rotateArr.x, rotateArr.y, rotateArr.z];
            const euler = new THREE.Euler(r[0], r[1], r[2], 'ZYX');
            baseRotation.setFromEuler(euler);
          }

          jointNode.quaternion.copy(baseRotation.multiply(q));
        }
      } else if (linkType === LINK_TYPE.PRISMATIC) {
        if (angleIndex < jointAngles.length) {
          const displacement = jointAngles[angleIndex];
          angleIndex++;

          const axisArr = config.angleAxis ?? [0, 0, 1];
          const axisVec = Array.isArray(axisArr)
            ? new THREE.Vector3(axisArr[0], axisArr[1], axisArr[2]).normalize()
            : new THREE.Vector3(axisArr.x, axisArr.y, axisArr.z).normalize();

          const baseArr = config.translate ?? [0, 0, 0];
          const baseTranslation = Array.isArray(baseArr)
            ? baseArr
            : [baseArr.x, baseArr.y, baseArr.z];

          jointNode.position.set(
            baseTranslation[0] + axisVec.x * displacement,
            baseTranslation[1] + axisVec.y * displacement,
            baseTranslation[2] + axisVec.z * displacement,
          );
        }
      }
    }
  }

  getEndEffectorPosition(): THREE.Vector3 {
    if (this.links.length === 0) {
      return new THREE.Vector3(0, 0, 0);
    }

    const lastLink = this.links[this.links.length - 1];
    const worldPos = new THREE.Vector3();
    lastLink.jointNode.getWorldPosition(worldPos);
    return worldPos;
  }

  getJointPositions(): THREE.Vector3[] {
    return this.links.map((link) => {
      const pos = new THREE.Vector3();
      link.jointNode.getWorldPosition(pos);
      return pos;
    });
  }
}
