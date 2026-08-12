import { useState } from 'react';
import { Modal, Menu } from 'antd';
import {
  DashboardOutlined,
  ControlOutlined,
  SafetyOutlined,
  EnvironmentOutlined,
  ApiOutlined,
  ToolOutlined,
} from '@ant-design/icons';
import { useT } from '@/i18n/useT';
import { ImpedancePanel } from './ImpedancePanel';

interface SystemSettingsModalProps {
  open: boolean;
  onClose: () => void;
}

const NAV_ITEMS = [
  { key: 'system-status', icon: <DashboardOutlined /> },
  { key: 'impedance', icon: <ControlOutlined /> },
  { key: 'safety', icon: <SafetyOutlined /> },
  { key: 'coordinate', icon: <EnvironmentOutlined /> },
  { key: 'io', icon: <ApiOutlined /> },
  { key: 'maintenance', icon: <ToolOutlined /> },
] as const;

type NavKey = (typeof NAV_ITEMS)[number]['key'];

export function SystemSettingsModal({ open, onClose }: SystemSettingsModalProps) {
  const t = useT();
  const [activeKey, setActiveKey] = useState<NavKey>('system-status');

  const menuItems = NAV_ITEMS.map((item) => ({
    key: item.key,
    icon: item.icon,
    label: t(`settings.nav.${item.key}` as Parameters<typeof t>[0]),
  }));

  return (
    <Modal
      open={open}
      onCancel={onClose}
      footer={null}
      width="80vw"
      style={{ top: 40 }}
      styles={{ body: { padding: 0, height: '75vh', display: 'flex', overflow: 'hidden' } }}
      title={t('settings.title')}
      destroyOnHide
    >
      <div style={{ display: 'flex', height: '100%', width: '100%' }}>
        {/* Left navigation */}
        <div
          style={{
            width: 220,
            borderRight: '1px solid var(--color-border)',
            flexShrink: 0,
            overflowY: 'auto',
          }}
        >
          <Menu
            mode="inline"
            selectedKeys={[activeKey]}
            items={menuItems}
            style={{ height: '100%', borderRight: 'none' }}
            onClick={({ key }) => setActiveKey(key as NavKey)}
          />
        </div>

        {/* Content area */}
        <div style={{ flex: 1, padding: '24px', overflowY: 'auto' }}>
          {activeKey === 'impedance' && <ImpedancePanel />}
        </div>
      </div>
    </Modal>
  );
}
