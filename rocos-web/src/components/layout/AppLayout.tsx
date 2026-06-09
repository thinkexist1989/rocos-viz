import { useState, useEffect, useCallback, useRef } from 'react';
import { useConnectionStore } from '@/stores/connectionStore';
import { useControlStore } from '@/stores/controlStore';
import { useUIStore } from '@/stores/uiStore';
import { useRobotStateStore } from '@/stores/robotStateStore';
import { RobotApiClient } from '@/core/RobotApiClient';
import { RobotViewer } from '@/scene/RobotViewer';
import { AxesIndicator } from '@/scene/AxesIndicator';
import { RobotModel } from '@/scene/RobotModel';
import { TrajectoryLine } from '@/scene/TrajectoryLine';
import { setCameraPreset } from '@/scene/CameraPresets';
import { ConnectDialog } from '@/components/connection/ConnectDialog';
import { RightPanel } from '@/components/layout/RightPanel';
import { StatusBar } from '@/components/layout/StatusBar';
import { PlotPanel } from '@/components/plot/PlotPanel';
import { useRobotConnection } from '@/hooks/useRobotConnection';
import { useModelStore } from '@/stores/modelStore';
import * as THREE from 'three';
import { MAX_TRAJECTORY_POINTS } from '@/core/constants';
import * as yaml from 'js-yaml';
import type { RobotModelConfig } from '@/core/types';
import {
  Button,
  Space,
  Tooltip,
  Modal,
  message,
} from 'antd';
import {
  ApiOutlined,
  AppstoreOutlined,
  LineChartOutlined,
  SettingOutlined,
  QuestionCircleOutlined,
  PauseCircleOutlined,
  ZoomInOutlined,
  ZoomOutOutlined,
  GlobalOutlined,
} from '@ant-design/icons';

/** Convert the API JSON RobotModelConfig to YAML matching C++ writeModelFiles format */
function modelConfigToYaml(model: RobotModelConfig): string {
  const lines: string[] = ['robot:'];
  for (const link of model.links) {
    lines.push(`  - name: ${link.name}`);
    lines.push(`    order: ${link.order}`);
    lines.push(`    type: ${(link.type ?? 'unknown').toLowerCase()}`);
    if (link.translate) {
      const t = link.translate;
      const x = Array.isArray(t) ? t[0] : (t as any).x ?? 0;
      const y = Array.isArray(t) ? t[1] : (t as any).y ?? 0;
      const z = Array.isArray(t) ? t[2] : (t as any).z ?? 0;
      lines.push(`    translate: [${x}, ${y}, ${z}]`);
    }
    if (link.rotate) {
      const r = link.rotate;
      const x = Array.isArray(r) ? r[0] : (r as any).x ?? 0;
      const y = Array.isArray(r) ? r[1] : (r as any).y ?? 0;
      const z = Array.isArray(r) ? r[2] : (r as any).z ?? 0;
      lines.push(`    rotate: [${x}, ${y}, ${z}]`);
    }
    if (link.axis) {
      const a = link.axis;
      const x = Array.isArray(a) ? a[0] : (a as any).x ?? 0;
      const y = Array.isArray(a) ? a[1] : (a as any).y ?? 0;
      const z = Array.isArray(a) ? a[2] : (a as any).z ?? 0;
      lines.push(`    angleAxis: [${x}, ${y}, ${z}]`);
    }
    if (link.translateLink) {
      const t = link.translateLink;
      const x = Array.isArray(t) ? t[0] : (t as any).x ?? 0;
      const y = Array.isArray(t) ? t[1] : (t as any).y ?? 0;
      const z = Array.isArray(t) ? t[2] : (t as any).z ?? 0;
      lines.push(`    translateLink: [${x}, ${y}, ${z}]`);
    }
    if (link.rotateLink) {
      const r = link.rotateLink;
      const x = Array.isArray(r) ? r[0] : (r as any).x ?? 0;
      const y = Array.isArray(r) ? r[1] : (r as any).y ?? 0;
      const z = Array.isArray(r) ? r[2] : (r as any).z ?? 0;
      lines.push(`    rotateLink: [${x}, ${y}, ${z}]`);
    }
    if (link.mesh) {
      lines.push(`    mesh: ${link.mesh}`);
    }
  }
  return lines.join('\n');
}

function TrajectoryLineWrapper() {
  const showTrajectory = useUIStore((s) => s.showTrajectory);
  const robotState = useRobotStateStore((s) => s.robotState);
  const pointsRef = useRef<THREE.Vector3[]>([]);

  useEffect(() => {
    if (!showTrajectory || !robotState) return;

    const pose = robotState.tool ?? robotState.tool_pose;
    if (!pose) return;
    const point = new THREE.Vector3(pose.position.x, pose.position.y, pose.position.z);
    pointsRef.current = [...pointsRef.current.slice(-(MAX_TRAJECTORY_POINTS - 1)), point];
  }, [robotState, showTrajectory]);

  if (!showTrajectory) return null;

  return <TrajectoryLine points={pointsRef.current} visible={showTrajectory} />;
}

export function AppLayout() {
  const [showConnectDialog, setShowConnectDialog] = useState(true);
  const [showAbout, setShowAbout] = useState(false);
  const [yamlContent, setYamlContent] = useState<string | null>(null);
  const [modelLoading, setModelLoading] = useState(false);

  const isConnected = useConnectionStore((s) => s.isConnected);
  const currentView = useUIStore((s) => s.currentView);
  const host = useConnectionStore((s) => s.host);
  const port = useConnectionStore((s) => s.port);

  // Mount the connection hook so polling / WebSocket starts when isConnected changes
  useRobotConnection();

  // Fetch robot model from controller and convert to YAML for the parser
  useEffect(() => {
    if (!isConnected) {
      setYamlContent(null);
      return;
    }

    let cancelled = false;

    async function fetchModel() {
      setModelLoading(true);
      try {
        const client = new RobotApiClient(host, port);
        const model = await client.getRobotModel();

        console.log('[AppLayout] getRobotModel response:', JSON.stringify(model).slice(0, 200));

        if (cancelled) return;

        // The API returns { name, links: [...] }
        // We need to convert it to YAML for YamlModelParser
        if (model && model.links && model.links.length > 0) {
          const yamlStr = modelConfigToYaml(model);
          console.log('[AppLayout] Converted model to YAML, links:', model.links.length);
          if (!cancelled) setYamlContent(yamlStr);
        } else {
          console.warn('[AppLayout] getRobotModel returned no links:', model);
          if (!cancelled) {
            message.warning('机器人模型无 links，请检查控制器配置');
          }
        }
      } catch (error) {
        console.error('Failed to fetch robot model:', error);
        if (!cancelled) {
          message.error('获取机器人模型失败: ' + (error instanceof Error ? error.message : String(error)));
        }
      } finally {
        if (!cancelled) setModelLoading(false);
      }
    }

    fetchModel();

    return () => { cancelled = true; };
  }, [isConnected, host, port]);

  return (
    <div className="app-layout">
      {/* Toolbar */}
      <div className="app-toolbar">
        <div className="logo">ROCOS-Viz</div>

        <Tooltip title={isConnected ? '已连接' : '未连接'}>
          <Button
            type={isConnected ? 'primary' : 'default'}
            size="small"
            icon={<ApiOutlined />}
            onClick={() => setShowConnectDialog(true)}
          >
            {isConnected ? 'Connected' : 'Connect'}
          </Button>
        </Tooltip>

        <Tooltip title="轴测图">
          <Button size="small" icon={<AppstoreOutlined />} onClick={() => setCameraPreset('axonometric')} />
        </Tooltip>
        <Tooltip title="俯视图">
          <Button size="small" icon={<ZoomInOutlined />} onClick={() => setCameraPreset('top')} />
        </Tooltip>
        <Tooltip title="前视图">
          <Button size="small" icon={<GlobalOutlined />} onClick={() => setCameraPreset('front')} />
        </Tooltip>
        <Tooltip title="右视图">
          <Button size="small" icon={<PauseCircleOutlined />} onClick={() => setCameraPreset('right')} />
        </Tooltip>

        <div className="toolbar-spacer" />

        <Tooltip title="图表">
          <Button
            size="small"
            icon={<LineChartOutlined />}
            onClick={() => useUIStore.getState().setView(currentView === 'scene' ? 'plot' : 'scene')}
          />
        </Tooltip>
        <Tooltip title="设置">
          <Button size="small" icon={<SettingOutlined />} />
        </Tooltip>
        <Tooltip title="关于">
          <Button size="small" icon={<QuestionCircleOutlined />} onClick={() => setShowAbout(true)} />
        </Tooltip>
      </div>

      {/* Main Content */}
      <div className="app-main">
        {/* Left Panel - 3D Viewport */}
        <div className="app-left-panel">
          {currentView === 'scene' ? (
            <RobotViewer>
              {isConnected && (
                <>
                  <RobotModel yamlContent={yamlContent} meshBaseUrl={`/api/robot/model/mesh`} />
                  <AxesIndicator />
                  <TrajectoryLineWrapper />
                </>
              )}
            </RobotViewer>
          ) : (
            <PlotPanel />
          )}

          {/* Bottom toggles */}
          <div className="scene-toggles">
            <Button
              size="small"
              type={useUIStore.getState().showAxes ? 'primary' : 'default'}
              onClick={() => useUIStore.getState().toggleAxes()}
            >
              Axes
            </Button>
            <Button
              size="small"
              type={useUIStore.getState().showWireframe ? 'primary' : 'default'}
              onClick={() => useUIStore.getState().toggleWireframe()}
            >
              Mesh
            </Button>
            <Button
              size="small"
              type={useUIStore.getState().showGround ? 'primary' : 'default'}
              onClick={() => useUIStore.getState().toggleGround()}
            >
              Ground
            </Button>
            <Button
              size="small"
              type={useUIStore.getState().showTrajectory ? 'primary' : 'default'}
              onClick={() => useUIStore.getState().toggleTrajectory()}
            >
              Trajectory
            </Button>
          </div>
        </div>

        {/* Right Panel - Controls */}
        <div className="app-right-panel">
          <RightPanel />
        </div>
      </div>

      {/* Status Bar */}
      <StatusBar />

      {/* Connect Dialog */}
      <ConnectDialog
        open={showConnectDialog}
        onClose={() => setShowConnectDialog(false)}
      />

      {/* About Dialog */}
      <Modal
        open={showAbout}
        onCancel={() => setShowAbout(false)}
        footer={null}
        width={400}
      >
        <div style={{ textAlign: 'center', padding: '20px 0' }}>
          <h2 style={{ color: '#3b82f6', marginBottom: 8 }}>ROCOS-Viz Web</h2>
          <p style={{ color: '#858585' }}>Version 0.1.0</p>
          <p style={{ color: '#858585', marginTop: 8 }}>
            React + Three.js + ECharts
          </p>
          <p style={{ color: '#858585', fontSize: 12, marginTop: 16 }}>
            Shenyang Institute of Automation<br />
            Chinese Academy of Sciences
          </p>
        </div>
      </Modal>
    </div>
  );
}
