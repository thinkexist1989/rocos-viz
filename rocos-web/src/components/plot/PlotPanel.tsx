import { useState } from 'react';
import { usePlotStore } from '@/stores/plotStore';
import { usePlotDataCollector, getChartDataKey } from '@/hooks/usePlotDataCollector';
import { RealtimeChart } from './RealtimeChart';
import { PlotConfigDialog } from './PlotConfigDialog';

export function PlotPanel() {
  const charts = usePlotStore((s) => s.charts);
  const dataBuffers = usePlotStore((s) => s.dataBuffers);
  const showConfigDialog = usePlotStore((s) => s.showConfigDialog);
  const setShowConfigDialog = usePlotStore((s) => s.setShowConfigDialog);
  const [editingChart, setEditingChart] = useState(0);

  usePlotDataCollector();

  const handleChartClick = (index: number) => {
    setEditingChart(index);
    setShowConfigDialog(true);
  };

  return (
    <div style={{ width: '100%', height: '100%', display: 'grid', gridTemplateColumns: '1fr 1fr', gridTemplateRows: '1fr 1fr', gap: 4, padding: 4 }}>
      {charts.slice(0, 4).map((chart, index) => {
        const key = getChartDataKey(chart.dataSource, chart.jointIndex);
        const data = dataBuffers.get(key) ?? [];

        return (
          <div
            key={chart.id}
            style={{ background: '#1a1d24', borderRadius: 4, overflow: 'hidden' }}
          >
            <RealtimeChart
              config={chart}
              data={data}
              onClick={() => handleChartClick(index)}
            />
          </div>
        );
      })}

      {showConfigDialog && editingChart < charts.length && (
        <PlotConfigDialog
          open={showConfigDialog}
          onClose={() => setShowConfigDialog(false)}
          chartIndex={editingChart}
          config={charts[editingChart]}
        />
      )}
    </div>
  );
}
