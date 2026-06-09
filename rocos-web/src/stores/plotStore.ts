import { create } from 'zustand';

export interface ChartDataPoint {
  timestamp: number;
  value: number;
}

export interface ChartConfig {
  id: number;
  title: string;
  dataSource: DataSourceKey;
  jointIndex?: number;
}

export type DataSourceKey =
  | 'joint_position'
  | 'joint_velocity'
  | 'joint_torque'
  | 'joint_load'
  | 'flange_x'
  | 'flange_y'
  | 'flange_z';

const MAX_POINTS = 500;
const WINDOW_DURATION_MS = 10000;

interface PlotStore {
  chartCount: number;
  charts: ChartConfig[];
  dataBuffers: Map<string, ChartDataPoint[]>;
  showConfigDialog: boolean;

  setChartCount: (count: number) => void;
  updateChart: (index: number, config: Partial<ChartConfig>) => void;
  appendData: (key: string, value: number) => void;
  clearData: () => void;
  setShowConfigDialog: (show: boolean) => void;
}

const defaultCharts: ChartConfig[] = [
  { id: 0, title: '关节位置', dataSource: 'joint_position', jointIndex: 0 },
  { id: 1, title: '关节速度', dataSource: 'joint_velocity', jointIndex: 0 },
  { id: 2, title: '关节力矩', dataSource: 'joint_torque', jointIndex: 0 },
  { id: 3, title: '末端位置 X', dataSource: 'flange_x' },
];

export const usePlotStore = create<PlotStore>((set, get) => ({
  chartCount: 4,
  charts: defaultCharts,
  dataBuffers: new Map(),
  showConfigDialog: false,

  setChartCount: (count) => set({ chartCount: count }),

  updateChart: (index, config) =>
    set((state) => ({
      charts: state.charts.map((c, i) => (i === index ? { ...c, ...config } : c)),
    })),

  appendData: (key, value) => {
    const buffers = get().dataBuffers;
    const now = Date.now();
    const cutoff = now - WINDOW_DURATION_MS;

    let points = buffers.get(key);
    if (!points) {
      points = [];
    }

    points.push({ timestamp: now, value });

    while (points.length > MAX_POINTS) {
      points.shift();
    }
    while (points.length > 0 && points[0].timestamp < cutoff) {
      points.shift();
    }

    buffers.set(key, points);
    set({ dataBuffers: new Map(buffers) });
  },

  clearData: () => set({ dataBuffers: new Map() }),

  setShowConfigDialog: (show) => set({ showConfigDialog: show }),
}));
