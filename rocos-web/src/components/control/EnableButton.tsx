import { useCallback } from 'react';
import { Button, Tooltip, message } from 'antd';
import { PoweroffOutlined } from '@ant-design/icons';
import { useConnectionStore } from '@/stores/connectionStore';
import { RobotApiClient } from '@/core/RobotApiClient';

export function EnableButton() {
  const isConnected = useConnectionStore((s) => s.isConnected);
  const isEnabled = useConnectionStore((s) => s.isRobotEnabled);
  const setEnabled = useConnectionStore((s) => s.setEnabled);
  const { host, port } = useConnectionStore.getState();

  const handleToggle = useCallback(async () => {
    if (!isConnected) {
      message.warning('请先连接机器人');
      return;
    }

    const client = new RobotApiClient(host, port);

    try {
      if (isEnabled) {
        await client.disable();
        setEnabled(false);
        message.success('已禁用');
      } else {
        await client.enable();
        setEnabled(true);
        message.success('已使能');
      }
    } catch (error: any) {
      message.error(`操作失败: ${error.message}`);
    }
  }, [isConnected, isEnabled, host, port, setEnabled]);

  return (
    <Tooltip title={isEnabled ? '禁用机器人' : '使能机器人'}>
      <Button
        type={isEnabled ? 'primary' : 'default'}
        danger={isEnabled}
        icon={<PoweroffOutlined />}
        onClick={handleToggle}
        disabled={!isConnected}
        size="small"
      >
        {isEnabled ? 'Enabled' : 'Enable'}
      </Button>
    </Tooltip>
  );
}
