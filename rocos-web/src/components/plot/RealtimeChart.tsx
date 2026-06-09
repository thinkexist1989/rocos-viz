import ReactECharts from 'echarts-for-react';
import { ChartConfig, ChartDataPoint } from '@/stores/plotStore';

interface RealtimeChartProps {
  config: ChartConfig;
  data: ChartDataPoint[];
  onClick?: () => void;
}

export function RealtimeChart({ config, data, onClick }: RealtimeChartProps) {
  const option = {
    title: {
      text: config.title,
      left: 'center',
      textStyle: { fontSize: 12, color: '#e0e0e0' },
    },
    tooltip: {
      trigger: 'axis' as const,
    },
    grid: {
      left: 40,
      right: 10,
      top: 30,
      bottom: 25,
    },
    xAxis: {
      type: 'time' as const,
      axisLabel: { fontSize: 10, color: '#858585' },
    },
    yAxis: {
      type: 'value' as const,
      scale: true,
      axisLabel: { fontSize: 10, color: '#858585' },
    },
    series: [
      {
        name: config.title,
        type: 'line' as const,
        data: data.map((d) => [d.timestamp, d.value]),
        large: true,
        sampling: 'lttb' as const,
        animation: false,
        lineStyle: { width: 1.5, color: '#3b82f6' },
        areaStyle: { color: 'rgba(59, 130, 246, 0.1)' },
      },
    ],
    dataZoom: [
      { type: 'inside' as const, xAxisIndex: 0 },
    ],
  };

  return (
    <div
      onClick={onClick}
      style={{ width: '100%', height: '100%', cursor: onClick ? 'pointer' : 'default' }}
    >
      <ReactECharts
        option={option}
        style={{ width: '100%', height: '100%' }}
        opts={{ renderer: 'canvas' }}
        notMerge
      />
    </div>
  );
}
