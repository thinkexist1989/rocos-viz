import { useCallback, useRef } from 'react';
import { Button } from 'antd';
import { useControlStore } from '@/stores/controlStore';
import { RobotApiClient } from '@/core/RobotApiClient';
import { useConnectionStore } from '@/stores/connectionStore';
import { PositionBar } from '@/components/common/PositionBar';
import {
  JOG_COMMAND_TIMEOUT_S,
  JOG_FEED_INTERVAL_MS,
  cartesianMotionParams,
  jointMotionParams,
} from '@/core/constants';

interface CartesianJogItemProps {
  label: string;
  value: number;
  /** 参考坐标系名称: BASE / FLANGE / TOOL / OBJECT */
  frameName: string;
  /** twist 向量中的轴索引: 0=vx, 1=vy, 2=vz, 3=wx, 4=wy, 5=wz */
  freedom: number;
  unit: 'mm' | 'deg';
  isPosition: boolean;
}

/** 将坐标系名称转为后端接受的 frame 参数 */
function frameToJogFrame(frameName: string): 'BASE' | 'FLANGE' | 'TOOL' | 'OBJECT' {
  const map: Record<string, 'BASE' | 'FLANGE' | 'TOOL' | 'OBJECT'> = {
    BASE: 'BASE', FLANGE: 'FLANGE', TOOL: 'TOOL', OBJECT: 'OBJECT',
  };
  return map[frameName] ?? 'BASE';
}

export function CartesianJogItem({ label, value, frameName, freedom, unit, isPosition }: CartesianJogItemProps) {
  const isMM = useControlStore((s) => s.isMM);
  const isDegree = useControlStore((s) => s.isDegree);
  const speedFactor = useControlStore((s) => s.speedFactor);
  const { host, port } = useConnectionStore.getState();
  const clientRef = useRef<RobotApiClient | null>(null);
  const timerRef = useRef<number | null>(null);

  const displayValue = isPosition
    ? (value * (isMM ? 1000 : 1)).toFixed(2)
    : value.toFixed(2);

  const displayUnit = isPosition ? (isMM ? 'mm' : 'm') : (isDegree ? 'deg' : 'rad');

  /** 构建 6 维 twist 向量，仅在 freedom 轴处置 1 */
  const buildTwist = useCallback((sign: number): number[] => {
    const twist = [0, 0, 0, 0, 0, 0];
    twist[freedom] = sign;
    return twist;
  }, [freedom]);

  const startJogLoop = useCallback((direction: 'POSITIVE' | 'NEGATIVE') => {
    if (timerRef.current !== null) return;

    const client = new RobotApiClient(host, port);
    clientRef.current = client;

    const twist = buildTwist(direction === 'POSITIVE' ? 1 : -1);
    const jogFrame = frameToJogFrame(frameName);
    // 平动按笛卡尔上限 (m/s)，转动按关节上限 (rad/s)
    const speed = (isPosition ? cartesianMotionParams : jointMotionParams)(speedFactor).speed;
    const feedJog = () => client.jogCartesian(twist, jogFrame, speed, JOG_COMMAND_TIMEOUT_S);

    feedJog().catch((error) => {
      console.error('Cartesian jog start failed:', error);
    });

    timerRef.current = window.setInterval(() => {
      if (!clientRef.current) return;
      feedJog().catch((error) => {
        console.error('Cartesian jog repeat failed:', error);
      });
    }, JOG_FEED_INTERVAL_MS);
  }, [buildTwist, frameName, host, port, speedFactor, isPosition]);

  const handleStopJog = useCallback(() => {
    if (timerRef.current !== null) {
      clearInterval(timerRef.current);
      timerRef.current = null;
    }

    if (clientRef.current) {
      clientRef.current.jogStop().catch((error) => {
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
