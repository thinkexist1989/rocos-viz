interface PositionBarProps {
  value: number;
  min: number;
  max: number;
  color?: string;
}

export function PositionBar({ value, min, max, color = '#496FFF' }: PositionBarProps) {
  const range = max - min;
  const percentage = Math.max(0, Math.min(100, ((value - min) / range) * 100));

  return (
    <div className="position-bar">
      <div
        className="bar-fill"
        style={{ width: `${percentage}%`, background: color }}
      />
      <div
        className="bar-indicator"
        style={{ left: `${percentage}%` }}
      />
    </div>
  );
}
