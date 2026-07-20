import { useCallback, useRef } from 'react';
import { Button, Tooltip } from 'antd';
import { useControlStore } from '@/stores/controlStore';
import { RobotApiClient } from '@/core/RobotApiClient';
import { useConnectionStore } from '@/stores/connectionStore';
import { PositionBar } from '@/components/common/PositionBar';
import { useT } from '@/i18n/useT';
import type { JointState } from '@/core/types';
import {
  JOG_COMMAND_TIMEOUT_S,
  JOG_FEED_INTERVAL_MS,
  jointMotionParams,
  MAX_JOINTS,
} from '@/core/constants';

interface JointJogItemProps {
  index: number;
  joint: JointState;
  jointCount?: number;
}

export function JointJogItem({ index, joint, jointCount = MAX_JOINTS }: JointJogItemProps) {
  const t = useT();
  const isDegree = useControlStore((s) => s.isDegree);
  const speedFactor = useControlStore((s) => s.speedFactor);
  const { host, port } = useConnectionStore.getState();
  const clientRef = useRef<RobotApiClient | null>(null);
  const timerRef = useRef<number | null>(null);

  const displayValue = isDegree
    ? ((joint.position * 180) / Math.PI).toFixed(2)
    : joint.position.toFixed(4);

  const unit = isDegree ? 'deg' : 'rad';

  /** 构建关节方向向量，仅在目标关节处置 1 */
  const buildJoints = useCallback((sign: number): number[] => {
    const joints = new Array(jointCount).fill(0);
    joints[index] = sign;
    return joints;
  }, [index, jointCount]);

  const startJogLoop = useCallback((direction: 'POSITIVE' | 'NEGATIVE') => {
    if (timerRef.current !== null) return;

    const client = new RobotApiClient(host, port);
    clientRef.current = client;

    const joints = buildJoints(direction === 'POSITIVE' ? 1 : -1);
    const speed = jointMotionParams(speedFactor).speed;
    const feedJog = () => client.jogJoint(joints, speed, JOG_COMMAND_TIMEOUT_S);

    feedJog().catch((error) => {
      console.error('Joint jog start failed:', error);
    });

    timerRef.current = window.setInterval(() => {
      if (!clientRef.current) return;
      feedJog().catch((error) => {
        console.error('Joint jog repeat failed:', error);
      });
    }, JOG_FEED_INTERVAL_MS);
  }, [buildJoints, host, port, speedFactor]);

  const handleStopJog = useCallback(() => {
    if (timerRef.current !== null) {
      clearInterval(timerRef.current);
      timerRef.current = null;
    }

    if (clientRef.current) {
      clientRef.current.jogStop().catch((error) => {
        console.error('Joint jog stop failed:', error);
      });
      clientRef.current = null;
    }
  }, []);

  return (
    <div className="jog-item">
      <span className="jog-label">J{index + 1}</span>

      <Tooltip title={t('jog.negative')}>
        <Button
          className="jog-btn"
          onMouseDown={(e) => { e.preventDefault(); startJogLoop('NEGATIVE'); }}
          onMouseUp={handleStopJog}
          onMouseLeave={handleStopJog}
          onTouchStart={(e) => { e.preventDefault(); startJogLoop('NEGATIVE'); }}
          onTouchEnd={handleStopJog}
        >
          −
        </Button>
      </Tooltip>

      <div className="position-bar" style={{ flex: 1 }}>
        <PositionBar value={joint.position} min={-3.14} max={3.14} />
      </div>

      <span className="jog-value">
        {displayValue} {unit}
      </span>

      <Tooltip title={t('jog.positive')}>
        <Button
          className="jog-btn"
          onMouseDown={(e) => { e.preventDefault(); startJogLoop('POSITIVE'); }}
          onMouseUp={handleStopJog}
          onMouseLeave={handleStopJog}
          onTouchStart={(e) => { e.preventDefault(); startJogLoop('POSITIVE'); }}
          onTouchEnd={handleStopJog}
        >
          +
        </Button>
      </Tooltip>
    </div>
  );
}
