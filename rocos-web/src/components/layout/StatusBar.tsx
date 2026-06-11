import { useEffect, useState } from 'react';
import { useConnectionStore } from '@/stores/connectionStore';
import { useControlStore } from '@/stores/controlStore';

export function StatusBar() {
  const isConnected = useConnectionStore((s) => s.isConnected);
  const isRobotEnabled = useConnectionStore((s) => s.isRobotEnabled);
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
      {isConnected && (
        <span>
          <span className={`status-dot ${isRobotEnabled ? 'enabled' : ''}`} />
          {isRobotEnabled ? 'Enabled' : 'Disabled'}
        </span>
      )}
      <span className="toolbar-spacer" />
      <span>Time: {formatTime(currentTime)}</span>
      <span>Running: {formatElapsed(Date.now() - startTime)}</span>
    </div>
  );
}
