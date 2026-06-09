import { useState } from 'react';
import { Modal, Tree, Button, Space, InputNumber, Select } from 'antd';
import { usePlotStore, ChartConfig, DataSourceKey } from '@/stores/plotStore';

const DATA_SOURCES = [
  {
    title: '关节数据',
    key: 'joint_group',
    children: [
      { title: '位置 (J1-J7)', key: 'joint_position' },
      { title: '速度 (J1-J7)', key: 'joint_velocity' },
      { title: '力矩 (J1-J7)', key: 'joint_torque' },
      { title: '负载 (J1-J7)', key: 'joint_load' },
    ],
  },
  {
    title: '末端位姿',
    key: 'flange_group',
    children: [
      { title: 'X', key: 'flange_x' },
      { title: 'Y', key: 'flange_y' },
      { title: 'Z', key: 'flange_z' },
    ],
  },
];

interface PlotConfigDialogProps {
  open: boolean;
  onClose: () => void;
  chartIndex: number;
  config: ChartConfig;
}

export function PlotConfigDialog({ open, onClose, chartIndex, config }: PlotConfigDialogProps) {
  const [selectedSource, setSelectedSource] = useState<DataSourceKey>(config.dataSource);
  const [jointIndex, setJointIndex] = useState(config.jointIndex ?? 0);
  const updateChart = usePlotStore((s) => s.updateChart);

  const handleOk = () => {
    updateChart(chartIndex, { dataSource: selectedSource, jointIndex });
    onClose();
  };

  return (
    <Modal
      open={open}
      title={`图表 ${chartIndex + 1} 配置`}
      onOk={handleOk}
      onCancel={onClose}
      width={400}
    >
      <Space direction="vertical" style={{ width: '100%' }}>
        <div>
          <div style={{ marginBottom: 8 }}>数据源：</div>
          <Tree
            treeData={DATA_SOURCES}
            defaultExpandAll
            onSelect={(keys) => {
              const key = keys[0] as DataSourceKey;
              if (key && !key.endsWith('_group')) {
                setSelectedSource(key);
              }
            }}
            selectedKeys={[selectedSource]}
          />
        </div>

        {selectedSource.startsWith('joint_') && (
          <div>
            <div style={{ marginBottom: 8 }}>关节索引：</div>
            <InputNumber
              min={0}
              max={6}
              value={jointIndex}
              onChange={(v) => setJointIndex(v ?? 0)}
            />
          </div>
        )}
      </Space>
    </Modal>
  );
}
