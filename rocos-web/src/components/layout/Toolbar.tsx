import { useConnectionStore } from '@/stores/connectionStore';

interface ToolbarProps {
  onShowConnect: () => void;
  onShowAbout: () => void;
}

export function Toolbar({ onShowConnect, onShowAbout }: ToolbarProps) {
  const isConnected = useConnectionStore((s) => s.isConnected);

  return (
    <div className="app-toolbar">
      <div className="logo">ROCOS-Viz</div>
      <button
        onClick={onShowConnect}
        style={{
          background: isConnected ? '#496FFF' : 'transparent',
          color: '#e2e8f0',
          border: '1px solid #496FFF',
          borderRadius: 4,
          padding: '4px 12px',
          cursor: 'pointer',
          fontSize: 13,
        }}
      >
        {isConnected ? '● Connected' : '○ Connect'}
      </button>
      <div style={{ flex: 1 }} />
      <button
        onClick={onShowAbout}
        style={{
          background: 'transparent',
          color: '#a0aec0',
          border: '1px solid #4a5568',
          borderRadius: 4,
          padding: '4px 8px',
          cursor: 'pointer',
          fontSize: 12,
        }}
      >
        About
      </button>
    </div>
  );
}
