import { useCallback, useRef } from 'react';
import { Button } from 'antd';
import { useRobotStateStore } from '@/stores/robotStateStore';
import { useConnectionStore } from '@/stores/connectionStore';
import { useControlStore } from '@/stores/controlStore';
import { RobotApiClient } from '@/core/RobotApiClient';
import { PositionBar } from '@/components/common/PositionBar';
import { useT } from '@/i18n/useT';
import {
  JOG_COMMAND_TIMEOUT_S,
  JOG_FEED_INTERVAL_MS,
  jointMotionParams,
} from '@/core/constants';

/**
 * Null-space jog panel (SVD 维度速度点动).
 *
 * 零空间维度 = max(0, DOF - 6)。每个维度生成一对 +/- 点动按钮。
 * 通过 jogSvd 发送各维度速度标量到后端执行。
 */
export function NullSpacePanel() {
  const t = useT();
  const robotState = useRobotStateStore((s) => s.robotState);
  const { host, port } = useConnectionStore.getState();

  const dof = robotState?.joint_states?.length ?? 0;
  const nullDims = Math.max(0, dof - 6);

  if (nullDims === 0) return null;

  const dims = Array.from({ length: nullDims }, (_, i) => ({
    label: `N${i + 1}`,
    index: i,
  }));

  return (
    <div className="panel-section">
      <div className="panel-section-header">
        {t('jog.nullSpace')}
      </div>
      <div>
        {dims.map((dim) => (
          <NullSpaceJogItem
            key={dim.label}
            label={dim.label}
            dimIndex={dim.index}
            nullDims={nullDims}
            host={host}
            port={port}
          />
        ))}
      </div>
    </div>
  );
}

interface NullSpaceJogItemProps {
  label: string;
  dimIndex: number;
  nullDims: number;
  host: string;
  port: string;
}

function NullSpaceJogItem({ label, dimIndex, nullDims, host, port }: NullSpaceJogItemProps) {
  const clientRef = useRef<RobotApiClient | null>(null);
  const timerRef = useRef<number | null>(null);

  /** 构建 SVD 维度速度向量（长度 = nullDims），仅在目标维度处为非零 */
  const buildDimSpeeds = useCallback((sign: number): number[] => {
    const dimSpeeds = new Array(nullDims).fill(0);
    dimSpeeds[dimIndex] = sign * jointMotionParams(useControlStore.getState().speedFactor).speed;
    return dimSpeeds;
  }, [dimIndex, nullDims]);

  const startJogLoop = useCallback((direction: 'POSITIVE' | 'NEGATIVE') => {
    if (timerRef.current !== null) return;

    const client = new RobotApiClient(host, port);
    clientRef.current = client;

    const dimSpeeds = buildDimSpeeds(direction === 'POSITIVE' ? 1 : -1);
    const feedJog = () => client.jogSvd(dimSpeeds, JOG_COMMAND_TIMEOUT_S);

    feedJog().catch((error) => {
      console.error('SVD jog start failed:', error);
    });

    timerRef.current = window.setInterval(() => {
      if (!clientRef.current) return;
      feedJog().catch((error) => {
        console.error('SVD jog repeat failed:', error);
      });
    }, JOG_FEED_INTERVAL_MS);
  }, [buildDimSpeeds, host, port]);

  const handleStopJog = useCallback(() => {
    if (timerRef.current !== null) {
      clearInterval(timerRef.current);
      timerRef.current = null;
    }

    if (clientRef.current) {
      clientRef.current.jogStop().catch((error) => {
        console.error('SVD jog stop failed:', error);
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
        <PositionBar value={0} min={-1} max={1} />
      </div>

      <span className="jog-value">0.00</span>

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
