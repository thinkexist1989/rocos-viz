import { useCallback, useRef } from 'react';
import { Button, Tooltip } from 'antd';
import { useControlStore } from '@/stores/controlStore';
import { RobotApiClient } from '@/core/RobotApiClient';
import { useConnectionStore } from '@/stores/connectionStore';
import { PositionBar } from '@/components/common/PositionBar';
import { useT } from '@/i18n/useT';
import type { JointState } from '@/core/types';
import { DIRECTION, jointMotionParams } from '@/core/constants';

const JOG_INTERVAL_MS = 80;

interface JointJogItemProps {
  index: number;
  joint: JointState;
}

export function JointJogItem({ index, joint }: JointJogItemProps) {
  const t = useT();
  const isDegree = useControlStore((s) => s.isDegree);
  const { host, port } = useConnectionStore.getState();
  const clientRef = useRef<RobotApiClient | null>(null);
  const timerRef = useRef<number | null>(null);

  const displayValue = isDegree
    ? ((joint.position * 180) / Math.PI).toFixed(2)
    : joint.position.toFixed(4);

  const unit = isDegree ? 'deg' : 'rad';

  const startJogLoop = useCallback((direction: typeof DIRECTION[keyof typeof DIRECTION]) => {
    const client = new RobotApiClient(host, port);
    clientRef.current = client;

    const flag = `J${index}`;
    const { speed, acceleration } = jointMotionParams(useControlStore.getState().speedFactor);
    client.dragStart(flag, direction, speed, acceleration).catch((error) => {
      console.error('Joint jog start failed:', error);
    });

    timerRef.current = window.setInterval(() => {
      if (!clientRef.current) return;
      clientRef.current.dragStart(flag, direction, speed, acceleration).catch((error) => {
        console.error('Joint jog repeat failed:', error);
      });
    }, JOG_INTERVAL_MS);
  }, [index, host, port]);

  const handleStopJog = useCallback(() => {
    if (timerRef.current !== null) {
      clearInterval(timerRef.current);
      timerRef.current = null;
    }

    if (clientRef.current) {
      clientRef.current.dragStop().catch((error) => {
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
