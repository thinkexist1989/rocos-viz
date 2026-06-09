import { ConfigProvider } from 'antd';
import zhCN from 'antd/locale/zh_CN';
import { themeConfig } from './styles/theme';
import { AppLayout } from './components/layout/AppLayout';

export default function App() {
  return (
    <ConfigProvider locale={zhCN} theme={themeConfig}>
      <AppLayout />
    </ConfigProvider>
  );
}
