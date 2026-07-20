import { useCallback } from 'react';
import { Button, Tooltip, message } from 'antd';
import {
  CaretRightOutlined,
  PauseOutlined,
  StepForwardOutlined,
  StopFilled,
} from '@ant-design/icons';
import { useConnectionStore } from '@/stores/connectionStore';
import { useRobotStateStore } from '@/stores/robotStateStore';
import { RobotApiClient } from '@/core/RobotApiClient';
import { useT } from '@/i18n/useT';

export function MotionControl() {
  const t = useT();
  const isConnected = useConnectionStore((s) => s.isConnected);
  const robotState = useRobotStateStore((s) => s.robotState);
  const host = useConnectionStore((s) => s.host);
  const port = useConnectionStore((s) => s.port);

  const isRunning = robotState?.robot_state === 'RUNNING';

  const getClient = useCallback(() => new RobotApiClient(host, port), [host, port]);

  const handleStartPause = useCallback(async () => {
    if (!isConnected) {
      message.warning(t('common.connectFirst'));
      return;
    }
    try {
      const client = getClient();
      if (isRunning) {
        await client.pauseMotion();
        message.success(t('motion.paused'));
      } else {
        await client.resumeMotion();
        message.success(t('motion.started'));
      }
    } catch (error: any) {
      message.error(t('enable.opFailed', { msg: error.message }));
    }
  }, [isConnected, isRunning, getClient, t]);

  const handleNextStep = useCallback(async () => {
    if (!isConnected) {
      message.warning(t('common.connectFirst'));
      return;
    }
    try {
      await getClient().stepScript();
      message.success(t('motion.stepped'));
    } catch (error: any) {
      message.error(t('enable.opFailed', { msg: error.message }));
    }
  }, [isConnected, getClient, t]);

  const handleStop = useCallback(async () => {
    if (!isConnected) {
      message.warning(t('common.connectFirst'));
      return;
    }
    try {
      await getClient().stopMove();
      message.success(t('motion.stopped'));
    } catch (error: any) {
      message.error(t('enable.opFailed', { msg: error.message }));
    }
  }, [isConnected, getClient, t]);

  return (
    <div className="panel-section">
      <div className="panel-section-header">{t('motion.label')}</div>
      <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'center', gap: 20, padding: '8px 0' }}>
        <Tooltip title={isRunning ? t('motion.pause') : t('motion.start')}>
          <Button
            className="motion-btn"
            icon={isRunning ? <PauseOutlined /> : <CaretRightOutlined />}
            onClick={handleStartPause}
            disabled={!isConnected}
            shape="circle"
            size="large"
          />
        </Tooltip>
        <Tooltip title={t('motion.nextStep')}>
          <Button
            className="motion-btn"
            icon={<StepForwardOutlined />}
            onClick={handleNextStep}
            disabled={!isConnected}
            shape="circle"
            size="large"
          />
        </Tooltip>
        <Tooltip title={t('motion.stop')}>
          <Button
            className="motion-btn motion-btn-stop"
            icon={<StopFilled />}
            onClick={handleStop}
            disabled={!isConnected}
            shape="circle"
            size="large"
          />
        </Tooltip>
      </div>
    </div>
  );
}
