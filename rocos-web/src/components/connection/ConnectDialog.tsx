import { useState, useCallback } from 'react';
import { Modal, Input, Button, Checkbox, message, Space, Typography } from 'antd';
import { useConnectionStore } from '@/stores/connectionStore';
import { RobotApiClient } from '@/core/RobotApiClient';

const { Text } = Typography;

interface ConnectDialogProps {
  open: boolean;
  onClose: () => void;
}

export function ConnectDialog({ open, onClose }: ConnectDialogProps) {
  const [host, setHost] = useState(() => useConnectionStore.getState().host);
  const [port, setPort] = useState(() => useConnectionStore.getState().port);
  const [autoLoad, setAutoLoad] = useState(true);
  const [loading, setLoading] = useState(false);

  const setConnected = useConnectionStore((s) => s.setConnected);
  const setConnectionStore = useConnectionStore((s) => s.setConnection);

  const handleConnect = useCallback(async () => {
    if (!host || !port) {
      message.error('请输入 IP 地址和端口号');
      return;
    }

    setLoading(true);
    try {
      const client = new RobotApiClient(host, port);
      await client.connect();

      setConnectionStore(host, port);
      setConnected(true);

      message.success('连接成功');
      onClose();
    } catch (error: any) {
      message.error(`连接失败: ${error.message || '未知错误'}`);
    } finally {
      setLoading(false);
    }
  }, [host, port, setConnectionStore, setConnected, onClose]);

  return (
    <Modal
      title="连接机器人控制器"
      open={open}
      onCancel={onClose}
      footer={null}
      width={400}
      className="connect-dialog"
      maskClosable={false}
    >
      <div style={{ padding: '16px 0' }}>
        <Space direction="vertical" style={{ width: '100%' }} size="middle">
          <div>
            <Text style={{ display: 'block', marginBottom: 4, color: 'var(--color-text-secondary)' }}>IP Address</Text>
            <Input
              value={host}
              onChange={(e) => setHost(e.target.value)}
              placeholder="192.168.0.194"
              size="large"
            />
          </div>

          <div>
            <Text style={{ display: 'block', marginBottom: 4, color: 'var(--color-text-secondary)' }}>Port</Text>
            <Input
              value={port}
              onChange={(e) => setPort(e.target.value)}
              placeholder="8080"
              size="large"
            />
          </div>

          <Checkbox checked={autoLoad} onChange={(e) => setAutoLoad(e.target.checked)}>
            自动加载机器人模型
          </Checkbox>

          <div style={{ display: 'flex', gap: 8, justifyContent: 'flex-end' }}>
            <Button onClick={onClose}>取消</Button>
            <Button
              type="primary"
              onClick={handleConnect}
              loading={loading}
              icon={<span>🔗</span>}
            >
              连接
            </Button>
          </div>
        </Space>
      </div>
    </Modal>
  );
}
