import { useState, useCallback } from 'react';
import { Modal, Input, Button, Checkbox, message, Space, Typography } from 'antd';
import { useConnectionStore } from '@/stores/connectionStore';
import { RobotApiClient } from '@/core/RobotApiClient';
import { useT } from '@/i18n/useT';

const { Text } = Typography;

interface ConnectDialogProps {
  open: boolean;
  onClose: () => void;
}

export function ConnectDialog({ open, onClose }: ConnectDialogProps) {
  const t = useT();
  const [host, setHost] = useState(() => useConnectionStore.getState().host || window.location.hostname);
  const [port, setPort] = useState(() => useConnectionStore.getState().port);
  const [autoLoad, setAutoLoad] = useState(true);
  const [loading, setLoading] = useState(false);

  const setConnected = useConnectionStore((s) => s.setConnected);
  const setConnectionStore = useConnectionStore((s) => s.setConnection);

  const handleConnect = useCallback(async () => {
    if (!host || !port) {
      message.error(t('conn.needIpPort'));
      return;
    }

    setLoading(true);
    try {
      const client = new RobotApiClient(host, port);
      await client.connect();

      setConnectionStore(host, port);
      setConnected(true);

      message.success(t('conn.success'));
      onClose();
    } catch (error: any) {
      message.error(t('conn.failed', { msg: error.message || '' }));
    } finally {
      setLoading(false);
    }
  }, [host, port, setConnectionStore, setConnected, onClose, t]);

  return (
    <Modal
      title={t('conn.dialogTitle')}
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
            <Text style={{ display: 'block', marginBottom: 4, color: 'var(--color-text-secondary)' }}>{t('conn.ipAddress')}</Text>
            <Input
              value={host}
              onChange={(e) => setHost(e.target.value)}
              placeholder="192.168.0.194"
              size="large"
            />
          </div>

          <div>
            <Text style={{ display: 'block', marginBottom: 4, color: 'var(--color-text-secondary)' }}>{t('conn.port')}</Text>
            <Input
              value={port}
              onChange={(e) => setPort(e.target.value)}
              placeholder="8080"
              size="large"
            />
          </div>

          <Checkbox checked={autoLoad} onChange={(e) => setAutoLoad(e.target.checked)}>
            {t('conn.autoLoadModel')}
          </Checkbox>

          <div style={{ display: 'flex', gap: 8, justifyContent: 'flex-end' }}>
            <Button onClick={onClose}>{t('conn.cancel')}</Button>
            <Button
              type="primary"
              onClick={handleConnect}
              loading={loading}
              icon={<span>🔗</span>}
            >
              {t('conn.connect')}
            </Button>
          </div>
        </Space>
      </div>
    </Modal>
  );
}
