import { useEffect, useState } from 'react';
import { useConnectionStore } from '@/stores/connectionStore';
import { useRobotStateStore } from '@/stores/robotStateStore';
import { useT } from '@/i18n/useT';

export function StatusBar() {
  const t = useT();
  const isConnected = useConnectionStore((s) => s.isConnected);
  const robotState = useRobotStateStore((s) => s.robotState);
  const [currentTime, setCurrentTime] = useState(new Date());
  const [startTime] = useState(() => Date.now());

  useEffect(() => {
    const timer = setInterval(() => setCurrentTime(new Date()), 500);
    return () => clearInterval(timer);
  }, []);

  const formatTime = (date: Date) =>
    date.toLocaleString('zh-CN', { year: 'numeric', month: '2-digit', day: '2-digit', hour: '2-digit', minute: '2-digit', second: '2-digit' });

  const formatElapsed = (ms: number) => {
    const totalSec = Math.floor(ms / 1000);
    const h = Math.floor(totalSec / 3600);
    const m = Math.floor((totalSec % 3600) / 60);
    const s = totalSec % 60;
    return `${String(h).padStart(2, '0')}:${String(m).padStart(2, '0')}:${String(s).padStart(2, '0')}`;
  };

  return (
    <div className="app-statusbar">
      <span>
        <span className={`status-dot ${isConnected ? 'enabled' : 'disconnected'}`} />
        {isConnected ? (robotState?.robot_state ?? t('conn.connected')) : t('conn.disconnected')}
      </span>
      <span className="toolbar-spacer" />
      <span>{t('status.time')}: {formatTime(currentTime)}</span>
      <span>{t('status.running')}: {formatElapsed(Date.now() - startTime)}</span>
    </div>
  );
}
