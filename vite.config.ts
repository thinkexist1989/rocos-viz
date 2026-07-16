import { defineConfig } from 'vite';
import react from '@vitejs/plugin-react';
import path from 'path';

const ROBOT_HOST = process.env.ROBOT_HOST || '127.0.0.1';
const ROBOT_PORT = process.env.ROBOT_PORT || '8080';
const VITE_PORT = parseInt(process.env.VITE_PORT || '5173', 10);

export default defineConfig({
  plugins: [react()],
  resolve: {
    alias: {
      '@': path.resolve(__dirname, './src'),
    },
  },
  server: {
    host: '0.0.0.0',
    port: VITE_PORT,
    open: true,
    watch: {
      usePolling: true,
    },
    proxy: {
      '/api': {
        target: `http://${ROBOT_HOST}:${ROBOT_PORT}`,
        changeOrigin: true,
      },
      '/ws': {
        target: `ws://${ROBOT_HOST}:${ROBOT_PORT}`,
        ws: true,
      },
    },
  },
});
