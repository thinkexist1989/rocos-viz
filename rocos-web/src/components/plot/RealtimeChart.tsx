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
      axisLabel: { fontSize: 10, color: '#a0aec0' },
    },
    yAxis: {
      type: 'value' as const,
      scale: true,
      axisLabel: { fontSize: 10, color: '#a0aec0' },
    },
    series: [
      {
        name: config.title,
        type: 'line' as const,
        data: data.map((d) => [d.timestamp, d.value]),
        large: true,
        sampling: 'lttb' as const,
        animation: false,
        lineStyle: { width: 1.5, color: '#496FFF' },
        areaStyle: { color: 'rgba(73, 111, 255, 0.1)' },
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
