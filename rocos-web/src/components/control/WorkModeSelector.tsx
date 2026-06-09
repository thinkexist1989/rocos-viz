import { Select, message } from 'antd';
import { useConnectionStore } from '@/stores/connectionStore';
import { RobotApiClient } from '@/core/RobotApiClient';
import { WORK_MODES } from '@/core/constants';

export function WorkModeSelector() {
  const isConnected = useConnectionStore((s) => s.isConnected);
  const { host, port } = useConnectionStore.getState();

  const handleChange = async (mode: string) => {
    if (!isConnected) {
      message.warning('请先连接机器人');
      return;
    }

    const client = new RobotApiClient(host, port);
    try {
      await client.setWorkMode(mode);
      message.success(`工作模式已切换为: ${WORK_MODES.find((m) => m.value === mode)?.label}`);
    } catch (error: any) {
      message.error(`切换失败: ${error.message}`);
    }
  };

  return (
    <div style={{ display: 'flex', alignItems: 'center', gap: 6 }}>
      <span style={{ fontSize: 12, color: 'var(--color-text-secondary)' }}>Mode:</span>
      <Select
        defaultValue="position"
        onChange={handleChange}
        options={WORK_MODES.map((m) => ({ value: m.value, label: m.label }))}
        size="small"
        style={{ width: 140 }}
        disabled={!isConnected}
      />
    </div>
  );
}
