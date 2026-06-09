import type { ThemeConfig } from 'antd';

export const themeConfig: ThemeConfig = {
  token: {
    colorPrimary: '#496FFF',
    colorBgContainer: '#1a1a2e',
    colorBgLayout: '#0f0f1a',
    colorBgElevated: '#16213e',
    colorText: '#e2e8f0',
    colorTextSecondary: '#a0aec0',
    colorBorder: '#2d3748',
    borderRadius: 6,
    fontFamily: '"Alibaba PuHuiTi 3.0", "PingFang SC", "Microsoft YaHei", sans-serif',
  },
  components: {
    Layout: {
      siderBg: '#1a1a2e',
      headerBg: '#0f0f1a',
      bodyBg: '#0f0f1a',
    },
    Button: {
      colorPrimary: '#496FFF',
    },
    Slider: {
      colorPrimaryBorder: '#496FFF',
    },
    Select: {
      colorBgContainer: '#16213e',
    },
    Input: {
      colorBgContainer: '#16213e',
    },
    InputNumber: {
      colorBgContainer: '#16213e',
    },
  },
};
