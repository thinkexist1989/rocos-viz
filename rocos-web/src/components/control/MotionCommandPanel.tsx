import { useState, useCallback } from 'react';
import { Button, InputNumber, Space, Typography, message } from 'antd';
import { useControlStore } from '@/stores/controlStore';
import { RobotApiClient } from '@/core/RobotApiClient';
import { useConnectionStore } from '@/stores/connectionStore';
import * as THREE from 'three';

const { Text } = Typography;

export function MotionCommandPanel() {
  const speedFactor = useControlStore((s) => s.speedFactor);
  const isDegree = useControlStore((s) => s.isDegree);
  const isMM = useControlStore((s) => s.isMM);
  const isConnected = useConnectionStore((s) => s.isConnected);
  const { host, port } = useConnectionStore.getState();

  const [cartesian, setCartesian] = useState({ x: 0, y: 0, z: 0, roll: 0, pitch: 0, yaw: 0 });
  const [joints, setJoints] = useState([0, 0, 0, 0, 0, 0, 0]);

  const handleMoveJ_IK = useCallback(async () => {
    if (!isConnected) { message.warning('请先连接机器人'); return; }

    const x = cartesian.x / (isMM ? 1000 : 1);
    const y = cartesian.y / (isMM ? 1000 : 1);
    const z = cartesian.z / (isMM ? 1000 : 1);
    const roll = (cartesian.roll * Math.PI) / (isDegree ? 180 : 1);
    const pitch = (cartesian.pitch * Math.PI) / (isDegree ? 180 : 1);
    const yaw = (cartesian.yaw * Math.PI) / (isDegree ? 180 : 1);

    const euler = new THREE.Euler(roll, pitch, yaw, 'ZYX');
    const q = new THREE.Quaternion().setFromEuler(euler);

    try {
      const client = new RobotApiClient(host, port);
      await client.moveJ_IK(
        { position: { x, y, z }, orientation: { x: q.x, y: q.y, z: q.z, w: q.w } },
        speedFactor,
      );
      message.success('MoveJ_IK 已发送');
    } catch (error: any) {
      message.error(error.message);
    }
  }, [cartesian, speedFactor, isDegree, isMM, isConnected, host, port]);

  const handleMoveL = useCallback(async () => {
    if (!isConnected) { message.warning('请先连接机器人'); return; }

    const x = cartesian.x / (isMM ? 1000 : 1);
    const y = cartesian.y / (isMM ? 1000 : 1);
    const z = cartesian.z / (isMM ? 1000 : 1);
    const roll = (cartesian.roll * Math.PI) / (isDegree ? 180 : 1);
    const pitch = (cartesian.pitch * Math.PI) / (isDegree ? 180 : 1);
    const yaw = (cartesian.yaw * Math.PI) / (isDegree ? 180 : 1);

    const euler = new THREE.Euler(roll, pitch, yaw, 'ZYX');
    const q = new THREE.Quaternion().setFromEuler(euler);

    try {
      const client = new RobotApiClient(host, port);
      await client.moveL(
        { position: { x, y, z }, orientation: { x: q.x, y: q.y, z: q.z, w: q.w } },
        speedFactor,
      );
      message.success('MoveL 已发送');
    } catch (error: any) {
      message.error(error.message);
    }
  }, [cartesian, speedFactor, isDegree, isMM, isConnected, host, port]);

  const handleMoveJ = useCallback(async () => {
    if (!isConnected) { message.warning('请先连接机器人'); return; }

    const rads = joints.map((j) => (j * Math.PI) / (isDegree ? 180 : 1));

    try {
      const client = new RobotApiClient(host, port);
      await client.moveJ(rads, speedFactor);
      message.success('MoveJ 已发送');
    } catch (error: any) {
      message.error(error.message);
    }
  }, [joints, speedFactor, isDegree, isConnected, host, port]);

  const handleMoveL_FK = useCallback(async () => {
    if (!isConnected) { message.warning('请先连接机器人'); return; }

    const rads = joints.map((j) => (j * Math.PI) / (isDegree ? 180 : 1));

    try {
      const client = new RobotApiClient(host, port);
      await client.moveL_FK(rads, speedFactor);
      message.success('MoveL_FK 已发送');
    } catch (error: any) {
      message.error(error.message);
    }
  }, [joints, speedFactor, isDegree, isConnected, host, port]);

  const updateCartesian = (key: string, value: number | null) => {
    setCartesian((prev) => ({ ...prev, [key]: value || 0 }));
  };

  const updateJoint = (index: number, value: number | null) => {
    setJoints((prev) => {
      const newJoints = [...prev];
      newJoints[index] = value || 0;
      return newJoints;
    });
  };

  return (
    <div className="panel-section">
      <div className="panel-section-header">Precise Control</div>

      {/* Cartesian Control */}
      <div className="precise-control" style={{ marginBottom: 12 }}>
        <Text style={{ fontSize: 12, color: 'var(--color-text-secondary)', marginBottom: 8, display: 'block' }}>
          Cartesian Space
        </Text>
        <div className="input-row">
          {(['x', 'y', 'z', 'roll', 'pitch', 'yaw'] as const).map((key) => (
            <div className="input-group" key={key}>
              <label>{key.toUpperCase()}</label>
              <InputNumber
                size="small"
                value={cartesian[key]}
                onChange={(v) => updateCartesian(key, v)}
                step={key === 'x' || key === 'y' || key === 'z' ? 1 : 1}
                style={{ width: '100%' }}
              />
            </div>
          ))}
        </div>
        <div className="btn-row">
          <Button size="small" type="primary" onClick={handleMoveJ_IK} disabled={!isConnected}>
            MoveJ_IK
          </Button>
          <Button size="small" type="primary" onClick={handleMoveL} disabled={!isConnected}>
            MoveL
          </Button>
        </div>
      </div>

      {/* Joint Control */}
      <div className="precise-control">
        <Text style={{ fontSize: 12, color: 'var(--color-text-secondary)', marginBottom: 8, display: 'block' }}>
          Joint Space
        </Text>
        <div className="input-row" style={{ flexWrap: 'wrap' }}>
          {joints.map((val, i) => (
            <div className="input-group" key={i} style={{ minWidth: 60 }}>
              <label>J{i + 1}</label>
              <InputNumber
                size="small"
                value={val}
                onChange={(v) => updateJoint(i, v)}
                step={1}
                min={-180}
                max={180}
                style={{ width: '100%' }}
              />
            </div>
          ))}
        </div>
        <div className="btn-row">
          <Button size="small" type="primary" onClick={handleMoveJ} disabled={!isConnected}>
            MoveJ
          </Button>
          <Button size="small" type="primary" onClick={handleMoveL_FK} disabled={!isConnected}>
            MoveL_FK
          </Button>
        </div>
      </div>
    </div>
  );
}
