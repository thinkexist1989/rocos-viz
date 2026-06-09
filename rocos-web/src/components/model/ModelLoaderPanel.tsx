import { useState } from 'react';
import { Button, Progress, message, Card } from 'antd';
import { DownloadOutlined, DeleteOutlined } from '@ant-design/icons';
import { useConnectionStore } from '@/stores/connectionStore';
import { useModelStore } from '@/stores/modelStore';
import { downloadModelWithCache, ModelDownloadProgress } from '@/core/ModelDownloader';
import { YamlModelParser } from '@/core/YamlModelParser';

export function ModelLoaderPanel() {
  const [downloading, setDownloading] = useState(false);
  const [progress, setProgress] = useState<ModelDownloadProgress | null>(null);

  const host = useConnectionStore((s) => s.host);
  const port = useConnectionStore((s) => s.port);
  const setModel = useModelStore((s) => s.setModel);

  const handleDownload = async () => {
    setDownloading(true);
    setProgress({ total: 0, loaded: 0, currentFile: '初始化...' });

    try {
      const { yaml, meshes } = await downloadModelWithCache(host, port, setProgress);

      const parser = new YamlModelParser();
      const baseUrl = `/api/robot/model/mesh`;
      const group = await parser.parse(yaml, baseUrl);

      setModel(group, { name: 'downloaded', links: [] });
      message.success('模型下载并加载完成');
    } catch (error) {
      message.error('下载失败: ' + (error instanceof Error ? error.message : String(error)));
    } finally {
      setDownloading(false);
    }
  };

  const handleClearCache = async () => {
    try {
      const db = await new Promise<IDBDatabase>((resolve, reject) => {
        const req = indexedDB.open('rocos-model-cache', 1);
        req.onupgradeneeded = () => {
          req.result.createObjectStore('meshes', { keyPath: 'key' });
        };
        req.onsuccess = () => resolve(req.result);
        req.onerror = () => reject(req.error);
      });

      const tx = db.transaction('meshes', 'readwrite');
      tx.objectStore('meshes').clear();
      tx.oncomplete = () => {
        message.success('缓存已清除');
        db.close();
      };
    } catch {
      message.error('清除缓存失败');
    }
  };

  const percent = progress && progress.total > 0
    ? Math.round((progress.loaded / progress.total) * 100)
    : 0;

  return (
    <div style={{ padding: 16 }}>
      <Card title="远程模型下载" size="small">
        <div style={{ marginBottom: 16 }}>
          <p style={{ color: 'var(--color-text-secondary)', marginBottom: 12 }}>
            从控制器 {host}:{port} 下载机器人模型和网格文件
          </p>
          <Button
            type="primary"
            icon={<DownloadOutlined />}
            onClick={handleDownload}
            loading={downloading}
            style={{ marginRight: 8 }}
          >
            下载模型
          </Button>
          <Button
            icon={<DeleteOutlined />}
            onClick={handleClearCache}
          >
            清除缓存
          </Button>
        </div>

        {downloading && progress && (
          <div>
            <Progress percent={percent} size="small" />
            <p style={{ color: 'var(--color-text-secondary)', fontSize: 12, marginTop: 8 }}>
              {progress.loaded}/{progress.total} - {progress.currentFile}
            </p>
          </div>
        )}
      </Card>
    </div>
  );
}
