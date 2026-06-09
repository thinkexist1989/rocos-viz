import type { ThemeConfig } from 'antd';
import { theme } from 'antd';

export const themeConfig: ThemeConfig = {
  algorithm: theme.darkAlgorithm,
  token: {
    colorPrimary: '#3b82f6',
    colorBgContainer: '#1a1d24',
    colorBgLayout: '#111318',
    colorBgElevated: '#21242d',
    colorText: '#e0e0e0',
    colorTextSecondary: '#858585',
    colorBorder: '#262930',
    borderRadius: 4,
    fontSize: 13,
    fontFamily: '"Alibaba PuHuiTi 3.0", "PingFang SC", "Microsoft YaHei", sans-serif',
  },
  components: {
    Layout: {
      siderBg: '#1a1d24',
      headerBg: '#111318',
      bodyBg: '#111318',
    },
    Button: {
      colorPrimary: '#3b82f6',
      algorithm: true,
    },
    Slider: {
      railSize: 4,
      handleSize: 10,
      handleSizeHover: 12,
      colorPrimaryBorder: '#3b82f6',
      colorPrimaryHover: '#60a5fa',
    },
    Select: {
      colorBgContainer: '#21242d',
    },
    Input: {
      colorBgContainer: '#21242d',
    },
    InputNumber: {
      colorBgContainer: '#21242d',
      controlWidth: 80,
      handleWidth: 18,
    },
    Tooltip: {
      colorBgSpotlight: '#262930',
    },
  },
};
