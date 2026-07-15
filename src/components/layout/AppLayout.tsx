import { useState, useEffect, useCallback, useRef } from 'react';
import { useConnectionStore } from '@/stores/connectionStore';
import { useUIStore, RIGHT_PANEL_MIN_WIDTH, RIGHT_PANEL_MAX_WIDTH } from '@/stores/uiStore';
import { useRobotStateStore } from '@/stores/robotStateStore';
import { RobotApiClient } from '@/core/RobotApiClient';
import { RobotViewer } from '@/scene/RobotViewer';
import { SceneErrorBoundary } from '@/scene/SceneErrorBoundary';
import { AxesIndicator } from '@/scene/AxesIndicator';
import { RobotModel } from '@/scene/RobotModel';
import { TrajectoryLine } from '@/scene/TrajectoryLine';
import { setCameraPreset } from '@/scene/CameraPresets';
import { ConnectDialog } from '@/components/connection/ConnectDialog';
import { RightPanel } from '@/components/layout/RightPanel';
import { StatusBar } from '@/components/layout/StatusBar';
import { PlotPanel } from '@/components/plot/PlotPanel';
import { EnableButton } from '@/components/control/EnableButton';
import { useRobotConnection } from '@/hooks/useRobotConnection';
import { useT } from '@/i18n/useT';
import * as THREE from 'three';
import { MAX_TRAJECTORY_POINTS } from '@/core/constants';
import type { RobotModelConfig } from '@/core/types';
import {
  Button,
  Tooltip,
  Modal,
  Popconfirm,
  message,
} from 'antd';
import {
  LineChartOutlined,
  SettingOutlined,
  QuestionCircleOutlined,
  BulbOutlined,
  TranslationOutlined,
  LeftOutlined,
  RightOutlined,
  CodepenOutlined,
  BorderTopOutlined,
  BorderOutlined,
  BorderRightOutlined,
  BorderBottomOutlined,
  BlockOutlined,
  DeploymentUnitOutlined,
  NodeIndexOutlined,
  ApiOutlined,
  DisconnectOutlined,
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

/** Reactive scene-toggle buttons – uses hooks so state changes are reflected immediately */
function SceneToggles({ style }: { style?: React.CSSProperties }) {
  const t = useT();
  const showJointFrames = useUIStore((s) => s.showJointFrames);
  const showWireframe = useUIStore((s) => s.showWireframe);
  const showGround = useUIStore((s) => s.showGround);
  const showTrajectory = useUIStore((s) => s.showTrajectory);
  const toggleJointFrames = useUIStore((s) => s.toggleJointFrames);
  const toggleWireframe = useUIStore((s) => s.toggleWireframe);
  const toggleGround = useUIStore((s) => s.toggleGround);
  const toggleTrajectory = useUIStore((s) => s.toggleTrajectory);

  const toggles = [
    { key: 'axes', label: t('scene.axes'), icon: <DeploymentUnitOutlined />, active: showJointFrames, onClick: toggleJointFrames },
    { key: 'mesh', label: t('scene.mesh'), icon: <BlockOutlined />, active: showWireframe, onClick: toggleWireframe },
    { key: 'ground', label: t('scene.ground'), icon: <BorderBottomOutlined />, active: showGround, onClick: toggleGround },
    { key: 'traj', label: t('scene.trajectory'), icon: <NodeIndexOutlined />, active: showTrajectory, onClick: toggleTrajectory },
  ];

  return (
    <div className="scene-toggles" style={style}>
      {toggles.map((t) => (
        <Tooltip key={t.key} title={t.label}>
          <button
            className={`scene-toggle-pill${t.active ? ' active' : ''}`}
            onClick={t.onClick}
          >
            <span className="pill-icon">{t.icon}</span>
            <span className="pill-label">{t.label}</span>
          </button>
        </Tooltip>
      ))}
    </div>
  );
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
  const t = useT();
  const [showConnectDialog, setShowConnectDialog] = useState(true);
  const [showAbout, setShowAbout] = useState(false);
  const [yamlContent, setYamlContent] = useState<string | null>(null);
  const [, setModelLoading] = useState(false);

  const isConnected = useConnectionStore((s) => s.isConnected);
  const currentView = useUIStore((s) => s.currentView);
  const language = useUIStore((s) => s.language);
  const rightPanelCollapsed = useUIStore((s) => s.rightPanelCollapsed);
  const toggleRightPanel = useUIStore((s) => s.toggleRightPanel);
  const rightPanelWidth = useUIStore((s) => s.rightPanelWidth);
  const setRightPanelWidth = useUIStore((s) => s.setRightPanelWidth);
  const plotFloatPos = useUIStore((s) => s.plotFloatPos);
  const setPlotFloatPos = useUIStore((s) => s.setPlotFloatPos);
  const host = useConnectionStore((s) => s.host);
  const port = useConnectionStore((s) => s.port);

  // The right panel is an absolute overlay over the viewport. Shift the
  // top/bottom floating overlays left by half the panel's covered width so they
  // stay centered within the *visible* 3D area, not the full viewport.
  const COLLAPSED_PEEK = 32; // px the collapsed panel tab still covers
  const coveredWidth = rightPanelCollapsed ? COLLAPSED_PEEK : rightPanelWidth;
  const sceneOverlayStyle = {
    transform: `translateX(calc(-50% - ${coveredWidth / 2}px))`,
  } as const;

  // --- Right panel resize via left-edge drag handle (mouse + touch) ---
  // The panel width is driven directly via DOM during the drag — we do NOT write
  // to the (persisted) store on every mousemove. Writing per-frame serialized the
  // whole store to localStorage synchronously on each move, stalling the main
  // thread and starving the 3D render loop (canvas blanked until the drag ended).
  // We commit the final width to the store once, on drag-end.
  const rightPanelRef = useRef<HTMLDivElement>(null);

  const beginResize = useCallback(
    (startX: number) => {
      const startWidth = useUIStore.getState().rightPanelWidth;
      let latestWidth = startWidth;

      const clamp = (w: number) =>
        Math.min(RIGHT_PANEL_MAX_WIDTH, Math.max(RIGHT_PANEL_MIN_WIDTH, w));

      // Panel is anchored to the right edge, so dragging left widens it.
      const apply = (clientX: number) => {
        latestWidth = clamp(startWidth + (startX - clientX));
        if (rightPanelRef.current) rightPanelRef.current.style.width = `${latestWidth}px`;
      };

      const onMouseMove = (ev: MouseEvent) => apply(ev.clientX);
      const onTouchMove = (ev: TouchEvent) => {
        if (ev.touches[0]) { ev.preventDefault(); apply(ev.touches[0].clientX); }
      };
      const onEnd = () => {
        window.removeEventListener('mousemove', onMouseMove);
        window.removeEventListener('mouseup', onEnd);
        window.removeEventListener('touchmove', onTouchMove);
        window.removeEventListener('touchend', onEnd);
        document.body.style.userSelect = '';
        // Commit once — triggers the single re-render / persist write.
        setRightPanelWidth(latestWidth);
      };
      document.body.style.userSelect = 'none';
      window.addEventListener('mousemove', onMouseMove);
      window.addEventListener('mouseup', onEnd);
      window.addEventListener('touchmove', onTouchMove, { passive: false });
      window.addEventListener('touchend', onEnd);
    },
    [setRightPanelWidth],
  );

  const handleResizeMouseDown = useCallback(
    (e: React.MouseEvent) => { e.preventDefault(); beginResize(e.clientX); },
    [beginResize],
  );
  const handleResizeTouchStart = useCallback(
    (e: React.TouchEvent) => { if (e.touches[0]) beginResize(e.touches[0].clientX); },
    [beginResize],
  );

  // --- Floating plot window drag via its header (mouse + touch) ---
  const beginPlotDrag = useCallback(
    (startX: number, startY: number) => {
      const start = useUIStore.getState().plotFloatPos ?? { x: 8, y: 8 };

      const apply = (clientX: number, clientY: number) =>
        setPlotFloatPos({
          x: Math.max(0, start.x + (clientX - startX)),
          y: Math.max(0, start.y + (clientY - startY)),
        });

      const onMouseMove = (ev: MouseEvent) => apply(ev.clientX, ev.clientY);
      const onTouchMove = (ev: TouchEvent) => {
        if (ev.touches[0]) { ev.preventDefault(); apply(ev.touches[0].clientX, ev.touches[0].clientY); }
      };
      const onEnd = () => {
        window.removeEventListener('mousemove', onMouseMove);
        window.removeEventListener('mouseup', onEnd);
        window.removeEventListener('touchmove', onTouchMove);
        window.removeEventListener('touchend', onEnd);
        document.body.style.userSelect = '';
      };
      document.body.style.userSelect = 'none';
      window.addEventListener('mousemove', onMouseMove);
      window.addEventListener('mouseup', onEnd);
      window.addEventListener('touchmove', onTouchMove, { passive: false });
      window.addEventListener('touchend', onEnd);
    },
    [setPlotFloatPos],
  );

  const handlePlotDragMouseDown = useCallback(
    (e: React.MouseEvent) => { e.preventDefault(); beginPlotDrag(e.clientX, e.clientY); },
    [beginPlotDrag],
  );
  const handlePlotDragTouchStart = useCallback(
    (e: React.TouchEvent) => { if (e.touches[0]) beginPlotDrag(e.touches[0].clientX, e.touches[0].clientY); },
    [beginPlotDrag],
  );

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
            message.warning(t('model.noLinks'));
          }
        }
      } catch (error) {
        console.error('Failed to fetch robot model:', error);
        if (!cancelled) {
          message.error(t('model.fetchFailed', { msg: error instanceof Error ? error.message : String(error) }));
        }
      } finally {
        if (!cancelled) setModelLoading(false);
      }
    }

    fetchModel();

    return () => { cancelled = true; };
  }, [isConnected, host, port, t]);

  return (
    <div className="app-layout">
      {/* Toolbar */}
      <div className="app-toolbar">
        <img className="logo" src="/icons/rocos.png" alt="ROCOS-Viz" />

        <div className="toolbar-cameras">
          <Tooltip title={t('view.axonometric')}>
            <Button size="middle" icon={<CodepenOutlined />} onClick={() => setCameraPreset('axonometric')} />
          </Tooltip>
          <Tooltip title={t('view.top')}>
            <Button size="middle" icon={<BorderTopOutlined />} onClick={() => setCameraPreset('top')} />
          </Tooltip>
          <Tooltip title={t('view.front')}>
            <Button size="middle" icon={<BorderOutlined />} onClick={() => setCameraPreset('front')} />
          </Tooltip>
          <Tooltip title={t('view.right')}>
            <Button size="middle" icon={<BorderRightOutlined />} onClick={() => setCameraPreset('right')} />
          </Tooltip>
        </div>

        {/* Centered connection status pill */}
        <div className="toolbar-center">
          {isConnected ? (
            <Popconfirm
              title={t('conn.disconnectTitle')}
              description={t('conn.disconnectConfirm', { target: `${host}:${port}` })}
              okText={t('conn.disconnect')}
              cancelText={t('conn.cancel')}
              okButtonProps={{ danger: true }}
              onConfirm={() => useConnectionStore.getState().reset()}
            >
              <button className="conn-pill connected" title={t('conn.clickToDisconnect')}>
                <span className="conn-dot" />
                <ApiOutlined className="conn-icon" />
                <span className="conn-text">{host}:{port}</span>
                <DisconnectOutlined className="conn-action" />
              </button>
            </Popconfirm>
          ) : (
            <button
              className="conn-pill disconnected"
              title={t('conn.clickToOpen')}
              onClick={() => setShowConnectDialog(true)}
            >
              <span className="conn-dot" />
              <ApiOutlined className="conn-icon" />
              <span className="conn-text">{t('conn.clickToConnect')}</span>
            </button>
          )}

          {/* Enable toggle — sits right beside the connect pill */}
          <EnableButton />
        </div>

        <div className="toolbar-spacer" />

        <Tooltip title={t('app.chart')}>
          <Button
            size="middle"
            type={currentView === 'plot' ? 'primary' : 'default'}
            icon={<LineChartOutlined />}
            onClick={() => useUIStore.getState().setView(currentView === 'scene' ? 'plot' : 'scene')}
          />
        </Tooltip>
        <Tooltip title={language === 'en' ? t('app.lang.toZh') : t('app.lang.toEn')}>
          <Button
            size="middle"
            icon={<TranslationOutlined />}
            onClick={() => useUIStore.getState().toggleLanguage()}
          >
            {language === 'en' ? '中' : 'EN'}
          </Button>
        </Tooltip>
        <Tooltip title={t('app.settings')}>
          <Button size="middle" icon={<SettingOutlined />} />
        </Tooltip>
        <Tooltip title={useUIStore.getState().themeMode === 'dark' ? t('app.theme.toLight') : t('app.theme.toDark')}>
          <Button
            size="middle"
            icon={<BulbOutlined />}
            onClick={() => useUIStore.getState().toggleTheme()}
          />
        </Tooltip>
        <Tooltip title={t('app.about')}>
          <Button size="middle" icon={<QuestionCircleOutlined />} onClick={() => setShowAbout(true)} />
        </Tooltip>
      </div>

      {/* Main Content */}
      <div className="app-main">
        {/* Full-width 3D Viewport */}
        <div className="app-left-panel">
          <div className="scene-viewport">
            <SceneErrorBoundary>
              <RobotViewer>
                {isConnected && (
                  <>
                    <RobotModel yamlContent={yamlContent} meshBaseUrl={`/api/robot/model/mesh`} />
                    <AxesIndicator />
                    <TrajectoryLineWrapper />
                  </>
                )}
              </RobotViewer>
            </SceneErrorBoundary>

            {/* Floating Plot Panel */}
            {currentView === 'plot' && (
              <div
                className="plot-float"
                style={
                  plotFloatPos
                    ? { left: plotFloatPos.x, top: plotFloatPos.y, bottom: 'auto' }
                    : undefined
                }
              >
                <div
                  className="plot-float-header"
                  onMouseDown={handlePlotDragMouseDown}
                  onTouchStart={handlePlotDragTouchStart}
                >
                  <span className="plot-float-title">{t('plot.realtimeCurves')}</span>
                </div>
                <div className="plot-float-body">
                  <PlotPanel />
                </div>
              </div>
            )}

            {/* Floating scene toggles (bottom-center of the visible 3D area) */}
            <SceneToggles style={sceneOverlayStyle} />
          </div>
        </div>

        {/* Right Panel - Overlay Controls */}
        <div
          ref={rightPanelRef}
          className={`app-right-panel${rightPanelCollapsed ? ' collapsed' : ''}`}
          style={{ width: rightPanelWidth }}
        >
          {/* Collapse toggle tab */}
          <button
            className="right-panel-toggle"
            onClick={toggleRightPanel}
            title={rightPanelCollapsed ? t('panel.expand') : t('panel.collapse')}
          >
            {rightPanelCollapsed ? <LeftOutlined /> : <RightOutlined />}
          </button>
          {/* Resize handle — drag to change panel width */}
          {!rightPanelCollapsed && (
            <div
              className="right-panel-resize"
              onMouseDown={handleResizeMouseDown}
              onTouchStart={handleResizeTouchStart}
              title={t('panel.resizeTip')}
            />
          )}
          <div className="right-panel-content">
            <RightPanel />
          </div>
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
          <p style={{ color: 'var(--color-text-secondary)' }}>Version 0.1.0</p>
          <p style={{ color: 'var(--color-text-secondary)', marginTop: 8 }}>
            React + Three.js + ECharts
          </p>
          <p style={{ color: 'var(--color-text-secondary)', fontSize: 12, marginTop: 16 }}>
            Shenyang Institute of Automation<br />
            Chinese Academy of Sciences
          </p>
        </div>
      </Modal>
    </div>
  );
}
