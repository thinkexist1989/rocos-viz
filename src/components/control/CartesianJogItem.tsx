import { useCallback, useRef } from 'react';
import { Button, Tooltip } from 'antd';
import { useControlStore } from '@/stores/controlStore';
import { RobotApiClient } from '@/core/RobotApiClient';
import { useConnectionStore } from '@/stores/connectionStore';
import { PositionBar } from '@/components/common/PositionBar';
import { FREEDOM_NAMES } from '@/core/constants';

const JOG_INTERVAL_MS = 80;

interface CartesianJogItemProps {
  label: string;
  value: number;
  frame: number;
  freedom: number;
  unit: 'mm' | 'deg';
  isPosition: boolean;
}

export function CartesianJogItem({ label, value, frame, freedom, unit, isPosition }: CartesianJogItemProps) {
  const isMM = useControlStore((s) => s.isMM);
  const isDegree = useControlStore((s) => s.isDegree);
  const { host, port } = useConnectionStore.getState();
  const clientRef = useRef<RobotApiClient | null>(null);
  const timerRef = useRef<number | null>(null);

  const displayValue = isPosition
    ? (value * (isMM ? 1000 : 1)).toFixed(2)
    : value.toFixed(2);

  const displayUnit = isPosition ? (isMM ? 'mm' : 'm') : (isDegree ? 'deg' : 'rad');

  const getFlag = useCallback(() => {
    const framePrefix = {
      100: 'TOOL',
      200: 'FLANGE',
      300: 'OBJECT',
      400: 'BASE',
    }[frame] || 'BASE';
    return `${framePrefix}_${FREEDOM_NAMES[freedom]}`;
  }, [frame, freedom]);

  const startJogLoop = useCallback((direction: 'POSITIVE' | 'NEGATIVE') => {
    const client = new RobotApiClient(host, port);
    clientRef.current = client;

    const flag = getFlag();
    client.dragStart(flag, direction).catch((error) => {
      console.error('Cartesian jog start failed:', error);
    });

    timerRef.current = window.setInterval(() => {
      if (!clientRef.current) return;
      clientRef.current.dragStart(flag, direction).catch((error) => {
        console.error('Cartesian jog repeat failed:', error);
      });
    }, JOG_INTERVAL_MS);
  }, [getFlag, host, port]);

  const handleStopJog = useCallback(() => {
    if (timerRef.current !== null) {
      clearInterval(timerRef.current);
      timerRef.current = null;
    }

    if (clientRef.current) {
      clientRef.current.dragStop().catch((error) => {
        console.error('Cartesian jog stop failed:', error);
      });
      clientRef.current = null;
    }
  }, []);

  return (
    <div className="jog-item">
      <span className="jog-label" style={{ width: 24 }}>{label}</span>

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

      <div className="position-bar" style={{ flex: 1 }}>
        <PositionBar value={value} min={-1} max={1} />
      </div>

      <span className="jog-value">
        {displayValue} {displayUnit}
      </span>

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
    </div>
  );
}
