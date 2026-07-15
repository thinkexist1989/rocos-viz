import { ConfigProvider } from 'antd';
import zhCN from 'antd/locale/zh_CN';
import enUS from 'antd/locale/en_US';
import { darkThemeConfig, lightThemeConfig } from './styles/theme';
import { AppLayout } from './components/layout/AppLayout';
import { useUIStore } from './stores/uiStore';

export default function App() {
  const themeMode = useUIStore((s) => s.themeMode);
  const language = useUIStore((s) => s.language);
  const currentTheme = themeMode === 'dark' ? darkThemeConfig : lightThemeConfig;

  return (
    <ConfigProvider locale={language === 'zh' ? zhCN : enUS} theme={currentTheme}>
      <AppLayout />
    </ConfigProvider>
  );
}
