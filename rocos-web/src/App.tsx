import { ConfigProvider } from 'antd';
import zhCN from 'antd/locale/zh_CN';
import { darkThemeConfig, lightThemeConfig } from './styles/theme';
import { AppLayout } from './components/layout/AppLayout';
import { useUIStore } from './stores/uiStore';

export default function App() {
  const themeMode = useUIStore((s) => s.themeMode);
  const currentTheme = themeMode === 'dark' ? darkThemeConfig : lightThemeConfig;

  return (
    <ConfigProvider locale={zhCN} theme={currentTheme}>
      <AppLayout />
    </ConfigProvider>
  );
}
