# ROCOS-Viz Web App — 需求理解与开发细节文档

> 技术方案：React 18 + TypeScript + Vite + Three.js (R3F) + ECharts + Ant Design
> 日期：2026-06-09
> 基于现有 Qt/VTK 桌面应用功能的 1:1 Web 重写

---

## 1. 项目定位

将现有 ROCOS-Viz（Qt 5/6 + VTK 9 C++ 桌面应用）的全部 UI 功能用 Web 技术重写，实现对 ROCOS 机器人控制器的远程可视化与控制。目标是 **浏览器内运行，零安装，跨平台**，未来可通过 Tauri 打包为桌面应用。

现有实现细节参见 [existing-implementation-details.md](existing-implementation-details.md)。

---

## 2. 技术栈确认

| 层 | 技术选择 | 版本 | 说明 |
|---|---|---|---|
| 前端框架 | React 18 + TypeScript | React 18.x | 声明式 UI，3D 生态最丰富 |
| 构建工具 | Vite 5 | 5.x | 快速 HMR，原生 ESM |
| 3D 渲染 | Three.js + @react-three/fiber + @react-three/drei | three r160+, r3f 8.x | 声明式 Three.js，OrbitControls/Grid/AxesHelper 等现成组件 |
| 图表 | ECharts 5 + echarts-for-react | echarts 5.x | 实时数据流，2x2 布局 |
| UI 组件库 | Ant Design 5.0 | antd 5.x | 企业级组件，按钮/表单/下拉框/滑块/布局 |
| 代码编辑 | CodeMirror 6 | @codemirror/* | Lua 语法高亮，轻量（~150KB） |
| 状态管理 | Zustand | zustand 4.x | 轻量全局状态（连接状态、机器人状态、UI 偏好） |
| 通信 | WebSocket（状态流）+ fetch REST（命令） | - | 混合方案 |
| HTTP 客户端 | fetch API + 自定义封装 | - | 替代 axios，减少依赖 |
| YAML 解析 | js-yaml | 3.x | 解析机器人模型配置 |
| 数学运算 | gl-matrix | 4.x | 四元数、矩阵运算（FK/IK） |
| 测试 | Vitest + Playwright | - | 单元测试 + E2E |
| 桌面化（Phase 6） | Tauri v2 | 2.x | 轻量打包 |

### 核心依赖清单

```json
{
  "dependencies": {
    "react": "^18.3.0",
    "react-dom": "^18.3.0",
    "three": "^0.160.0",
    "@react-three/fiber": "^8.15.0",
    "@react-three/drei": "^9.88.0",
    "antd": "^5.12.0",
    "@ant-design/icons": "^5.2.0",
    "echarts": "^5.5.0",
    "echarts-for-react": "^3.0.2",
    "zustand": "^4.4.0",
    "js-yaml": "^4.1.0",
    "@codemirror/lang-lua": "^6.0.0",
    "@codemirror/view": "^6.0.0",
    "gl-matrix": "^3.4.0"
  }
}
```

---

## 3. 功能需求全景

### 3.1 功能到现有 Qt 组件的映射

| # | 功能 | Qt 组件 | Web 组件 | 优先级 |
|---|---|---|---|---|
| F1 | 连接管理 | ConnectDialog | ConnectDialog | P0 |
| F2 | 3D 机器人渲染 | SceneWidget + Model | RobotViewer (R3F) | P0 |
| F3 | 关节点动控制 | JointPositionWidget ×7 | JointJogPanel | P0 |
| F4 | 笛卡尔点动控制 | CartesianPositionWidget ×6 | CartesianJogPanel | P0 |
| F5 | 速度缩放 | QSlider (1-999) | SpeedSlider | P0 |
| F6 | 工作模式切换 | QComboBox (5 种) | WorkModeSelector | P0 |
| F7 | 参考坐标系切换 | QComboBox (4 种) | FrameSelector | P0 |
| F8 | 运动指令（精确控制） | MoveJ/MoveL/MoveJ_IK/MoveL_FK 按钮 | MotionCommandPanel | P0 |
| F9 | 使能/禁用 | 工具栏按钮 | EnableButton | P0 |
| F10 | 相机视图切换 | Axo/Front/Top/Right 按钮 | CameraToolbar | P1 |
| F11 | 实时数据图表 | PlotWidget + PlotConfigDialog | PlotPanel (ECharts) | P1 |
| F12 | Lua 脚本编辑 | ScriptDialog | ScriptEditor (CodeMirror) | P2 |
| F13 | 远程模型下载 | ModelLoaderDialog | ModelLoaderPanel | P1 |
| F14 | 轨迹显示 | SceneWidget 轨迹渲染 | TrajectoryLine (R3F) | P1 |
| F15 | 关节轴可视化 | AxesHelper toggle | AxesToggle | P1 |
| F16 | 网格/Mesh 切换 | 工具栏按钮 | DisplayToggle | P1 |
| F17 | 地面显示 | 工具栏按钮 | GroundToggle | P1 |
| F18 | 角度/单位切换 | 度/弧度、mm/m | UnitToggle | P1 |
| F19 | 关于对话框 | AboutDialog | AboutModal | P2 |
| F20 | 时间显示 | timerLabel / runningLabel | StatusBar | P2 |

---

## 4. 页面布局设计

### 4.1 整体布局

现有 Qt 窗口为 1920x1080，左侧 40% / 右侧 60%。Web 版采用相同的主布局，但使用响应式设计。

```
┌─────────────────────────────────────────────────────────────────┐
│  Logo + 连接状态指示灯 + 工具栏（使能/视图/图表/脚本/模型/关于） │
├───────────────────────────────────┬─────────────────────────────┤ Mode  │ Ref Frame   │
│                                   ├───────────────┴─────────────┤
│       3D 视口 / 图表视口           │ Speed Scaling [====o=====]   │
│       (Stacked: Scene | Plot)      ├─────────────────────────────┤
│                                   │ CARTESIAN SPACE (BASE)       │
│                                   │ [X] [====o===] 100.00 mm    │
│                                   │ [Y] [====o===] -50.00 mm    │
│                                   │ [Z] [====o===] 200.00 mm    │
│                                   │ [R] [====o===] 0.00 deg     │
│                                   │ [P] [====o===] 90.00 deg    │
│                                   │ [Y] [====o===] 0.00 deg     │
│                                   ├─────────────────────────────┤
│                                   │ JOINT SPACE                  │
│                                   │ J-1 [====o===] 0.00 deg     │
│                                   │ J-2 [====o===] -30.00 deg   │
│                                   │ ...                          │
│                                   │ J-7 [====o===] 0.00 deg     │
│                                   ├─────────────────────────────┤
│ [Axes][Mesh][Ground][Trajectory]  │ Precise Control              │
│ Time: 2026-06-09 14:30:00        │ x[___] y[___] z[___]        │
│ Running: 00:05:23                │ roll[___] pitch[___] yaw[___]│
│                                   │ [MoveJ_IK] [MoveL]           │
│                                   │ j1[___] j2[___] ... j7[___] │
│                                   │ [MoveJ] [MoveL_FK]           │
└───────────────────────────────────┴─────────────────────────────┘
```

### 4.2 响应式断点

| 断点 | 宽度 | 布局 |
|---|---|---|
| Desktop Large | >= 1400px | 左右分栏（40/60），与 Qt 版一致 |
| Desktop | >= 1024px | 左右分栏（50/50），右侧面板缩小 |
| Tablet | >= 768px | 上下堆叠：3D 视口在上，控制面板在下 |
| Mobile | < 768px | 单列布局，控制面板折叠为手风琴 |

---

## 5. 各功能模块详细设计

### 5.1 连接管理 (F1)

**现有行为**：ConnectDialog 为无边框弹窗，输入 IP/Port，点击 Connect 后发起 GET `/api/robot/info`，成功后启动 20ms 状态轮询，10 次连续失败后自动断开。

**Web 实现**：

```
组件：ConnectDialog.tsx
状态管理：useConnectionStore (Zustand)
```

**交互流程**：

1. 应用启动 → 检查 localStorage 缓存的上次连接地址
2. 显示连接弹窗（Ant Design Modal），预填 IP/Port
3. 用户点击连接：
   - 发送 `GET http://{ip}:{port}/api/robot/info`
   - 超时：1000ms
   - 成功：缓存地址到 localStorage，关闭弹窗，启动 WebSocket 连接
   - 失败：显示错误提示
4. "自动加载模型"复选框：连接成功后自动调用模型下载
5. 连接状态指示灯（绿色/红色圆点）显示在工具栏

**WebSocket 连接**（替代 HTTP 轮询）：

```
WebSocket URL: ws://{ip}:{port}/ws/robot_state
协议：JSON 或 MessagePack
推送频率：20ms (50Hz)
断线检测：心跳间隔 2s，3 次未响应 → 重连
重连策略：指数退避，最大间隔 5s，最多重试 10 次
```

**Zustand Store 设计**：

```typescript
interface ConnectionStore {
  host: string;
  port: number;
  isConnected: boolean;
  isRobotEnabled: boolean;
  wsConnected: boolean;

  connect: (host: string, port: string, autoLoad: boolean) => Promise<void>;
  disconnect: () => void;
  enable: () => Promise<void>;
  disable: () => Promise<void>;
}
```

### 5.2 3D 机器人渲染 (F2)

**现有行为**：VTK 渲染器加载 YAML 配置，解析每个 Link 的 mesh 文件（STL），按关节层级构建场景树。每 20ms 更新关节角度，通过 Eigen 矩阵链式相乘计算正运动学变换。

**Web 实现**：

```
组件：RobotViewer.tsx (R3F Canvas)
子组件：RobotModel, GroundGrid, AxesIndicator, TrajectoryLine
核心逻辑：useYamlModelParser + useForwardKinematics
```

#### 5.2.1 YAML 模型解析器

现有模型格式定义了每个 Link 的平移、旋转、旋转轴、网格偏移等参数。需要编写 TypeScript 解析器直接读取 YAML，构建 Three.js Object3D 层级。

**字段映射规则**（来自源码分析）：

```typescript
interface LinkConfig {
  name: string;           // 连杆名称
  order: number;          // 序号（决定父子关系：order N 的 parent 是 order N-1）
  mesh: string;           // 网格文件路径（相对于 config.yaml）
  translate?: [number, number, number];      // 关节相对于父连杆的平移
  rotate?: [number, number, number];         // 关节相对于父连杆的旋转 (RPY 弧度)
  angleAxis?: [number, number, number];      // 关节旋转轴方向
  translateLink?: [number, number, number];  // 连杆网格相对于关节的额外平移
  rotateLink?: [number, number, number];     // 连杆网格相对于关节的额外旋转 (RPY)
  type?: 'revolute' | 'continuous' | 'fixed' | 'prismatic';
}
```

**解析流程**：

```typescript
// 伪代码
async function parseYamlModel(yamlContent: string, baseUrl: string): Promise<Group> {
  const config = yaml.load(yamlContent);
  const robot = new Group();
  const sortedLinks = config.robot.sort((a, b) => a.order - b.order);

  for (const link of sortedLinks) {
    // 1. 创建关节 Object3D（作为父连杆的子节点）
    const jointObj = new Object3D();
    jointObj.name = link.name;

    // 2. 应用关节变换（translate + rotate/angleAxis）
    if (link.translate) {
      jointObj.position.set(...link.translate);
    }
    if (link.rotate) {
      // RPY 弧度 → 欧拉角 (ZYX 顺序，与源码一致)
      jointObj.rotation.set(link.rotate[2], link.rotate[1], link.rotate[0], 'ZYX');
    }

    // 3. 加载网格
    const mesh = await loadMesh(link.mesh, baseUrl);

    // 4. 应用连杆偏移（translateLink + rotateLink）
    if (link.translateLink) {
      mesh.position.set(...link.translateLink);
    }
    if (link.rotateLink) {
      mesh.rotation.set(link.rotateLink[2], link.rotateLink[1], link.rotateLink[0], 'ZYX');
    }

    // 5. 添加到层级
    jointObj.add(mesh);

    // 6. 父子关系：按 order 顺序，每个节点的父是上一个节点
    //    第一个节点（order=0）直接添加到 robot Group
  }

  return robot;
}
```

#### 5.2.2 正运动学更新

**现有算法**（Model::updateModel）：

```
对每个关节 i：
  transform = translate(translate[i]) × rotate(rotate[i])
  if (angleAxis 存在):
    transform = transform × rotate(angleAngle, angleAxis[i])
  cumulativeTransform *= transform应用 cumulativeTransform 到 link 的 vtkActor
```

**Web 实现**：利用 Three.js Object3D 的父子层级天然继承变换的特性：

```typescript
function updateJointAngles(robot: Group, jointAngles: number[]) {
  robot.children.forEach((jointObj, i) => {
    if (i < jointAngles.length && jointObj.userData.type !== 'fixed') {
      // 只需设置绕旋转轴的角度，其余变换（translate/rotate）已在模型加载时设置
      // rotation 是累加在已有的 translate+rotate 基础上的
      const axis = jointObj.userData.angleAxis || [0, 0, 1];
      const q = new Quaternion().setFromAxisAngle(
        new Vector3(...axis), jointAngles[i]
      );
      jointObj.quaternion.multiply(q); // 注意：需要维护增量，避免累积
    }
  });
}
```

**关键注意点**：
- 源码中 `angleAxis` 是关节的旋转轴方向，`rotate` 是固定偏移，`angleAngle` 是运行时角度
- 需要在模型加载时记录初始 transform，在更新时只改变关节角度分量
- 保持与桌面版完全一致的 Z-Y-X 旋转顺序

#### 5.2.3 3D 场景组件结构

```tsx
<Canvas camera={{ position: [3, 3, 3], fov: 50 }}>
  {/* 地面网格 */}
  <Grid
    args={[20, 20]}
    cellSize={0.5}
    cellThickness={0.5}
    cellColor="#6e6e6e"
    sectionSize={1}
    fadeDistance={15}
    fadeStrength={1}
  />

  {/* 机器人模型 */}
  <RobotModel
    configUrl={modelUrl}
    jointAngles={jointAngles}
    showAxes={showAxes}
    showWireframe={showWireframe}
  />

  {/* 轨迹线 */}
  <TrajectoryLine points={trajectoryPoints} visible={showTrajectory} />

  {/* 全局坐标轴指示器 (左下角) */}
  <AxesIndicator />

  {/* 轨道控制器 */}
  <OrbitControls
    target={[0, 0, 0.5]}    // 焦点略高于地面
    enableDamping
    dampingFactor={0.1}
  />

  {/* 环境光 + 平行光 */}
  <ambientLight intensity={0.5} />
  <directionalLight position={[5, 5, 5]} intensity={0.8} />
</Canvas>
```

#### 5.2.4 相机预设

| 视图 | 相机位置 | 焦点 |
|---|---|---|
| 轴测图 | (3, 3, 3) | (0, 0, 0.5) |
| 俯视图 (Z) | (0, 0, 3) | (0, 0, 0.5) |
| 前视图 (X) | (3, 0, 0.5) | (0, 0, 0.5) |
| 右视图 (Y) | (0, 3, 0.5) | (0, 0, 0.5) |

使用 `@react-three/drei` 的 `CameraControls` 或手动设置 `camera.position` + `controls.target`，配合 `useFrame` 实现平滑过渡动画。

### 5.3 关节点动控制 (F3)

**现有行为**：7 个 JointPositionWidget，每个包含名称标签、进度条（PositionBar）、±按钮。按下按钮发送 `jointJogging(id, dir)`，松开发送 `jointJogging(id, 0)`。进度条范围由关节最大位置决定。

**Web 实现**：

```tsx
// 单个关节控制组件
interface JointJogItemProps {
  index: number;       // 0-6
  name: string;        // "J-1" ~ "J-7"
  position: number;    // 当前角度（弧度）
  min: number;         // 最小角度（弧度）
  max: number;         // 最大角度（弧度）
  isDegree: boolean;   // 是否显示度数
  factor: number;      // 速度系数 (0-1)
  onJog: (index: number, direction: number) => void;
}
```

**点动交互**：

```typescript
// mousedown → 持续发送 drag/start
// mouseup → 发送 drag/stop
// 使用 useRef 标记当前是否按下，防止重复发送

const handleMouseDown = () => {
  onJog(index, +1);  // 正方向
};

const handleMouseUp = () => {
  onJog(index, 0);   // 停止
};

// 同时支持 touch 事件（移动端）
```

**进度条**：自定义组件，使用 Ant Design `Slider` 的反向模式或纯 CSS 实现，显示关节当前位置在 [min, max] 范围中的百分比。双击显示精确数值（与现有 PositionBar 行为一致）。

### 5.4 笛卡尔点动控制 (F4)

**现有行为**：6 个 CartesianPositionWidget（X/Y/Z/Roll/Pitch/Yaw），坐标系切换（BASE=400/FLANGE=200/TOOL=100/OBJECT=300），点动时将坐标系常量 + 自由度索引组合为 API flag。

**Web 实现**：

**坐标系映射**（来自 Protocol.h）：

```typescript
const FRAME = {
  BASE: 400,
  FLANGE: 200,
  TOOL: 100,
  OBJECT: 300,
} as const;

const FREEDOM = {
  X: 0, Y: 1, Z: 2,
  ROLL: 3, PITCH: 4, YAW: 5,
} as const;

// flag 计算：currentFrame + freedomIndex
// 例如：FRAME_BASE(400) + FREEDOM_X(0) = 400 → API flag "BASE_X"
// 例如：FRAME_TOOL(100) + FREEDOM_ROLL(3) = 103 → API flag "TOOL_ROLL"
```

**单位切换**：
- X/Y/Z：默认 mm，切换时乘/除 1000
- Roll/Pitch/Yaw：默认 degree，切换时乘/除 (180/PI)
- 单位状态存入 Zustand store

### 5.5 速度缩放 (F5)

**现有行为**：滑块范围 1-999，默认 250。factor = value / 1000.0。更新所有 13 个位置控件的 factor 属性。

**Web 实现**：

```tsx
<Slider
  min={1} max={999} defaultValue={250}
  tooltip={{ formatter: (v) => `${(v / 10).toFixed(1)}%` }}
  onChange={(v) => setSpeedFactor(v / 1000)}
/>
```

speedFactor 存入 Zustand store，被所有关节/笛卡尔控制组件读取。

### 5.6 运动指令 (F8)

**现有行为**：精确控制面板包含两组输入框和按钮：
- 笛卡尔输入：x/y/z (mm) + roll/pitch/yaw (deg) → MoveJ_IK / MoveL
- 关节输入：j1-j7 (deg) → MoveJ / MoveL_FK

**API 调用格式**：

```typescript
// MoveJ：关节空间运动
await fetch(`${baseUrl}/api/move/joint`, {
  method: 'POST',
  headers: { 'Content-Type': 'application/json' },
  body: JSON.stringify({
    joints: [j1_rad, j2_rad, ..., j7_rad],
    speed: speedFactor
  })
});

// MoveL：笛卡尔空间直线运动
await fetch(`${baseUrl}/api/move/linear`, {
  method: 'POST',
  headers: { 'Content-Type': 'application/json' },
  body: JSON.stringify({
    position: [x_m, y_m, z_m],
    orientation: { qx, qy, qz, qw },  // 四元数
    speed: speedFactor
  })
});

// MoveJ_IK：笛卡尔 → 关节空间（控制器端做 IK）
await fetch(`${baseUrl}/api/move/joint_ik`, {
  method: 'POST',
  body: JSON.stringify({
    position: [x_m, y_m, z_m],
    orientation: { qx, qy, qz, qw },
    speed: speedFactor
  })
});

// MoveL_FK：关节空间 → 直线运动（控制器端做 FK → 直线插补）
await fetch(`${baseUrl}/api/m`, {
  method: 'POST',
  body: JSON.stringify({
    joints: [j1_rad, ..., j7_rad],
    speed: speedFactor
  })
});
```

**RPY → 四元数转换**：使用 gl-matrix 的 `quat.fromEuler` 或 Three.js 的 `Quaternion.setFromEuler(new Euler(roll, pitch, yaw, 'ZYX'))`。

**异步运动状态追踪**：

```typescript
// 发送运动后获得 task_id
const { task_id } = await moveJ(joints, speed);

// 轮询状态（200ms 间隔）
const pollMoveStatus = async (taskId: string) => {
  const interval = setInterval(async () => {
    const res = await fetch(`${baseUrl}/api/move/status?task_id=${taskId}`);
    const { status } = await res.json();
    if (status === 'completed' || status === 'failed') {
      clearInterval(interval);
      // 更新 UI 状态
    }
  }, 200);
};
```

### 5.7 工作模式切换 (F6)

```typescript
const WORK_MODES = [
  { value: 'position', label: '位置模式' },
  { value: 'ee_admit_teach', label: '末端执行器导纳示教' },
  { value: 'jnt_admit_teach', label: '关节导纳示教' },
  { value: 'jnt_imp', label: '关节阻抗' },
  { value: 'cart_imp', label: '笛卡尔阻抗' },
];

// POST /api/robot/workmode { mode: "position" }
```

### 5.8 实时数据图表 (F11)

**现有行为**：PlotWidget 显示最多 4 个图表（2x2 布局），每个图表可配置显示哪些关节的哪些数据（位置/速度/力矩/电流）。数据窗口 10 秒，每 20ms 追加新数据点。

**Web 实现**：

```tsx
// 图表面板：2x2 CSS Grid 布局
<div className="plot-grid">
  {charts.map((chart, i) => (
    <EChartsChart key={i} config={chart} data={realtimeData} />
  ))}
</div>
```

**ECharts 配置**：

```typescript
const createLineChartOption = (title: string, series: SeriesConfig[]) => ({
  title: { text: title, left: 'center', textStyle: { fontSize: 12 } },
  tooltip: { trigger: 'axis' },
  legend: { bottom: 0, textStyle: { fontSize: 10 } },
  xAxis: { type: 'time' },
  yAxis: { type: 'value', scale: true },
  series: series.map(s => ({
    name: s.name,
    type: 'line',
    data: s.data,  // [[timestamp, value], ...]
    large: true,
    sampling: 'lttb',  // 大数据量降采样
    animation: false,
  })),
  dataZoom: [
    { type: 'inside', xAxisIndex: 0 },  // 内部缩放
  ],
});
```

**数据源配置**（对应 PlotConfigDialog）：

| 图表 | 可选数据源 |
|---|---|
| 图表 1 | J1-J7 position, Flange X/Y/Z |
| 图表 2 | J1-J7 velocity |
| 图表 3 | J1-J7 torque |
| 图表 4 | J1-J7 current / Flange RPY |

使用 Ant Design `Tree` 组件实现数据源选择器。

**数据管理**：使用环形缓冲区存储最近 10s 的数据（500 点 × 最多 7 轴 = 3500 点/图表），通过 Zustand store 管理。

### 5.9 Lua 脚本编辑器 (F12)

**现有行为**：ScriptDialog 包含脚本路径下拉框、执行/停止/暂停/继续按钮。当前桌面版的脚本执行函数体为空（未完整实现）。

**Web 实现**：

```tsx
<div className="script-editor-panel">
  {/* 工具栏 */}
  <Space>
    <Button icon={<PlayCircleOutlined />} onClick={runScript}>运行</Button>
    <Button icon={<PauseCircleOutlined />} onClick={pauseScript}>暂停</Button>
    <Button icon={<StopOutlined />} onClick={stopScript}>停止</Button>
    <Button icon={<ForwardOutlined />} onClick={continueScript}>继续</Button>
  </Space>

  {/* CodeMirror 编辑器 */}
  <CodeMirror
    value={scriptContent}
    extensions={[lua(), oneDark]}
    onChange={(v) => setScriptContent(v)}
    height="400px"
  />

  {/* 输出控制台 */}
  <pre className="script-console">{scriptOutput}</pre>
</div>
```

**API 调用**：

```typescript
// POST /api/script/run   { script: "lua code string" }
// POST /api/script/stop  { task_id: "..." }
// POST /api/script/pause { task_id: "..." }
// POST /api/script/continue { task_id: "..." }
```

### 5.10 远程模型下载 (F13)

**现有行为**：连接时可选自动下载模型。从控制器获取模型结构 JSON，逐个下载 STL/DAE mesh 文件，保存到本地 models/ 目录。

**Web 实现**：

```typescript
async function downloadModel(baseUrl: string): Promise<Group> {
  // 1. 获取模型结构
  const modelRes = await fetch(`${baseUrl}/api/robot/model`);
  const { links } = await modelRes.json();

  // 2. 并发下载所有 mesh 文件
  const meshPromises = links.map(async (link) => {
    const meshRes = await fetch(`${baseUrl}/api/robot/model/mesh?path=${link.mesh}`);
    const blob = await meshRes.blob();
    return { name: link.name, blob, url: URL.createObjectURL(blob) };
  });

  const meshes = await Promise.all(meshPromises);

  // 3. 解析 YAML 配置（如果有 config 部分）并构建 Object3D 层级
  // 4. 缓存到 IndexedDB（可选，避免重复下载）
  return buildRobotModel(modelConfig, meshes);
}
```

**缓存策略**：
- 使用 IndexedDB 存储下载的 mesh blob
- 缓存 key: `${baseUrl}_model_${modelName}_${hash}`
- 有效期：会话级别（刷新清除）或可配置

### 5.11 轨迹显示 (F14)

**现有行为**：收集末端执行器位置点（最多 500 个），使用 VTK PolyLine 渲染蓝色折线。

**Web 实现**：

```tsx
function TrajectoryLine({ points, visible }: Props) {
  const geometryRef = useRef<BufferGeometry>(null);

  useEffect(() => {
    if (geometryRef.current && points.length > 0) {
      const positions = new Float32Array(points.flat());
      geometryRef.current.setAttribute('position', new BufferAttribute(positions, 3));
      geometryRef.current.setDrawRange(0, points.length);
    }
  }, [points]);

  if (!visible || points.length < 2) return null;

  return (
    <line>
      <bufferGeometry ref={geometryRef} />
      <lineBasicMaterial color={0x496fff} linewidth={2} />
    </line>
  );
}
```

注意：Three.js `linewidth` 在 WebGL 中默认只支持 1px。如需粗线，使用 `Line2` from `three/examples/jsm/lines/Line2`。

### 5.12 显示切换控制 (F15-F18)

所有开关状态存入 Zustand UI store：

```typescript
interface UiStore {
  showAxes: boolean;        可视化
  showWireframe: boolean;    // 网格/实体切换
  showGround: boolean;       // 地面网格
  showTrajectory: boolean;   // 轨迹线
  isDegree: boolean;         // 角度单位：度/弧度
  isMM: boolean;             // 位置单位：mm/m
  currentView: 'scene' | 'plot';  // 3D 视口 vs 图表视口

  toggleAxes: () => void;
  toggleWireframe: () => void;
  toggleGround: () => void;
  toggleTrajectory: () => void;
  toggleUnit: () => void;
  setView: (view: 'scene' | 'plot') => void;
}
```

---

## 6. 通信层设计

### 6.1 架构

```
┌─────────────┐     WebSocket      ┌──────────────────┐
│   Web App   │ ←───────────────→  │  ROCOS Controller │
│             │     (状态推送 50Hz)  │   (192.168.0.x)  │
│  fetch()    │ ─────────────────→ │                    │
│             │   HTTP REST (命令)  │  Port: 30001      │
└─────────────┘                    └──────────────────┘
```

### 6.2 HTTP REST 客户端

```typescript
class RobotHttpClient {
  private baseUrl: string;

  constructor(host: string, port: number) {
    this.baseUrl = `http://${host}:${port}`;
  }

  // 通用请求封装
  async request<T>(method: string, path: string, body?: any): Promise<T> {
    const res = await fetch(`${this.baseUrl}${path}`, {
      method,
      headers: {
        'Accept': 'application/json',
        'Content-Type': 'application/json',
      },
      body: body ? JSON.stringify(body) : undefined,
      signal: AbortSignal.timeout(5000),  // 5s 超时
    });

    const json = await res.json();
    if (!json.success) {
      throw new RobotError(json.code, json.message);
    }
    return json.data;
  }

  // 机器人信息
  async getRobotInfo(): Promise<RobotInfo> {
    return this.request('GET', '/api/robot/info');
  }

  // 使能/禁用
  async enable(): Promise<void> { return this.request('POST', '/api/robot/enable'); }
  async disable(): Promise<void> { return this.request('POST', '/api/robot/disable'); }

  // 工作模式
  async setWorkMode(mode: string): Promise<void> {
    return this.request('POST', '/api/robot/workmode', { mode });
  }

  // 运动命令
  async moveJ(joints: number[], speed: number): Promise<MoveResult> {
    return this.request('POST', '/api/move/joint', { joints, speed });
  }

  async moveL(position: number[], orientation: Quaternion, speed: number): Promise<MoveResult> {
    return this.request('POST', '/api/move/linear', { position, orientation, speed });
  }

  async moveJ_IK(position: number[], orientation: Quaternion, speed: number): Promise<MoveResult> {
    return this.request('POST', '/api/move/joint_ik', { position, orientation, speed });
  }

  async moveL_FK(joints: number[], speed: number): Promise<MoveResult> {
    return this.request('POST', '/api/move/linear_fk', { joints, speed });
  }

  async stopMotion(): Promise<void> { return this.request('POST', '/api/move/stop'); }

  // 点动
  async startDrag(flag: string, speed: number): Promise<void> {
    return this.request('POST', '/api/drag/start', { flag, speed });
  }

  async stopDrag(): Promise<void> { return this.request('POST', '/api/drag/stop'); }

  // 模型
  async getModel(): Promise<ModelInfo> { return this.request('GET', '/api/robot/model'); }

  async getMesh(path: string): Promise<Blob> {
    const res = await fetch(`${this.baseUrl}/api/robot/model/mesh?path=${path}`);
    return res.blob();
  }

  // 脚本
  async runScript(script: string): Promise<ScriptResult> {
    return this.request('POST', '/api/script/run', { script });
  }
}
```

### 6.3 WebSocket 状态流

```typescript
class RobotWebSocket {
  private ws: WebSocket | null = null;
  private reconnectTimer: number | null = null;
  private reconnectDelay = 1000;
  private maxReconnectDelay = 5000;
  private heartbeatTimer: number | null = null;

  connect(host: string, port: number, onState: (state: RobotState) => void) {
    this.ws = new WebSocket(`ws://${host}:${port}/ws/robot_state`);

    this.ws.onopen = () => {
      this.reconnectDelay = 1000;  // 重置退避
      this.startHeartbeat();
    };

    this.ws.onmessage = (event) => {
      const state = JSON.parse(event.data);
      onState(state);
    };

    this.ws.onclose = () => {
      this.stopHeartbeat();
      this.scheduleReconnect(host, port, onState);
    };

    this.ws.onerror = () => {
      this.ws?.close();
    };
  }

  private scheduleReconnect(host, port, onState) {
    this.reconnectTimer = window.setTimeout(() => {
      this.reconnectDelay = Math.min(this.reconnectDelay * 2, this.maxReconnectDelay);
      this.connect(host, port, onState);
    }, this.reconnectDelay);
  }

  private startHeartbeat() {
    this.heartbeatTimer = window.setInterval(() => {
      if (this.ws?.readyState === WebSocket.OPEN) {
        this.ws.send(JSON.stringify({ type: 'ping' }));
      }
    }, 2000);
  }

  send(data: any) {
    if (this.ws?.readyState === WebSocket.OPEN) {
      this.ws.send(JSON.stringify(data));
    }
  }

  disconnect() {
    this.stopHeartbeat();
    if (this.reconnectTimer) clearTimeout(this.reconnectTimer);
    this.ws?.close();
  }
}
```

### 6.4 降级方案

如果控制器不支持 WebSocket，自动降级为 HTTP 轮询：

```typescript
class StatePoller {
  private timer: number | null = null;

  start(host: string, port: number, interval: number, onState: (s: RobotState) => void) {
    this.timer = window.setInterval(async () => {
      try {
        const res = await fetch(`http://${host}:${port}/api/robot/state`);
        const { data } = await res.json();
        onState(data);
      } catch (e) {
        console.warn('State poll failed:', e);
      }
    }, interval);
  }

  stop() {
    if (this.timer) clearInterval(this.timer);
  }
}
```

---

## 7. 状态管理（Zustand）

### 7.1 Store 划分

| Store | 职责 | 持久化 |
|---|---|---|
| `useConnectionStore` | 连接状态、host/port、isConnected、isRobotEnabled | localStorage (host/port) |
| `useRobotStateStore` | 关节状态、位姿、硬件信息（实时更新） | 否 |
| `useModelStore` | 机器人模型 Three.js 对象、config、加载状态 | IndexedDB (mesh 缓存) |
| `useControlStore` | speedFactor、currentFrame、isDegree、isMM、workMode | 否 |
| `useUiStore` | showAxes/Wireframe/Ground/Trajectory、currentView | 否 |
| `usePlotStore` | 图表配置、实时数据缓冲区 | 否 |

### 7.2 机器人状态数据结构

```typescript
interface RobotState {
  joint_states: JointState[];
  flange_pose: Pose;
  tool_pose: Pose;
  object_pose: Pose;
  hardware: HardwareInfo;
}

interface JointState {
  name: string;
  position: number;   // 弧度
  velocity: number;
  acceleration: number;
  torque: number;
  load: number;
  status: 'DISABLED' | 'ENABLED' | 'FAULT';
}

interface Pose {
  x: number; y: number; z: number;
  qx: number; qy: number; qz: number; qw: number;
}

interface HardwareInfo {
  type: 'SIMULATION' | 'ETHERCAT' | 'UNKNOWN';
  cycle_time_min: number;
  cycle_time_avg: number;
  cycle_time_max: number;
  slave_count: number;
}
```

---
## 8. 项目结构

```
rocos-web/
├── public/
│   ├── fonts/                          # Alibaba PuHuiTi 字体
│   └── icons/                          # 应用图标
├── src/
│   ├── main.tsx                        # 入口
│   ├── App.tsx                         # 根组件，布局
│   ├── vite-env.d.ts
│   │
│   ├── core/                           # 框架无关的核心逻辑
│   │   ├── RobotHttpClient.ts          # HTTP REST 客户端
│   │   ├── RobotWebSocket.ts           # WebSocket 状态流
│   │   ├── StatePoller.ts             # HTTP 轮询降级
│   │   ├── YamlModelParser.ts          # YAML 模型解析器
│   │   ├── ForwardKinematics.ts        # 正运动学计算
│   │   └── constants.ts               # FRAME/FREEDOM/WORK_MODE 常量
│   │
│   ├── stores/                         # Zustand 状态管理
│   │   ├── connectionStore.ts
│   │   ├── robotStateStore.ts
│   │   ├── modelStore.ts
│   │   ├── controlStore.ts
│   │   ├── uiStore.ts
│   │   └── plotStore.ts
│   │
│   ├── scene/                          # 3D 场景组件 (R3F)
│   │   ├── RobotViewer.tsx             # R3F Canvas 包装器
│   │   ├── RobotModel.tsx              # 机器人模型组件
│   │   ├── LinkComponent.tsx           # 单个连杆渲染
│   │   ├── TrajectoryLine.tsx          # 轨迹渲染
│   │   ├── AxesIndicator.tsx           # 姿态坐标轴指示器
│   │   ├── CameraPresets.tsx           # 相机预设控制器
│   │   └── SceneHelpers.tsx            # Grid、环境光等辅助元素
│   │
│   ├── components/                     # UI 组件
│   │   ├── layout/
│   │   │   ├── AppLayout.tsx           # 主布局
│   │   │   ├── Toolbar.tsx            # 顶部工具栏
│   │   │   ├── StatusBar.tsx          # 底部状态栏
│   │   │   └── RightPanel.tsx         # 右侧控制面板
│   │   │
│   │   ├── connection/
│   │   │   └── ConnectDialog.tsx      # 连接弹窗
│   │   │
│   │   ├── control/
│   │   │   ├── JointJogPanel.tsx       # 关节点动面板
│   │   │   ├── JointJogItem.tsx        # 单个关节控件
│   │   │   ├── CartesianJogPanel.tsx   # 笛卡尔点动面板
│   │   │   ├── CartesianJogItem.tsx    # 单个笛卡尔轴控件
│   │   │   ├── SpeedSlider.tsx         # 速度滑块
│   │   │   ├── WorkModeSelector.tsx    # 工作模式下拉框
│   │   │   ├── FrameSelector.tsx       # 参考坐标系下拉框
│   │   │   ├── MotionCommandPanel.tsx  # 精确运动指令
│   │   │   └── EnableButton.tsx        # 使能/禁用按钮
│   │   │
│   │   ├── plot/
│   │   │   ├── PlotPanel.tsx           # 图表面板 (2x2)
│   │   │   ├── RealtimeChart.tsx       # 单个实时图表
│   │   │   └── PlotConfigDialog.tsx    # 图表配置对话框
│   │   │
│   │   ├── script/
│   │   │   └── ScriptEditor.tsx        # Lua 脚本编辑器
│   │   │
│   │   ├── model/
│   │   │   └── ModelLoaderPanel.tsx    # 模型加载面板
│   │   │
│   │   └── common/
│   │       ├── PositionBar.tsx         # 关节位置进度条
│   │       ├── UnitToggle.tsx          # 单位切换按钮组
│   │       └── DisplayToggles.tsx      # 显示开关按钮组
│   │
│   ├── hooks/                          # 自定义 Hooks
│   │   ├── useRobotConnection.ts       # 连接管理 hook
│   │   ├── useRobotState.ts           # 状态订阅 hook
│   │   ├── useJointJog.ts            # 关节点动 hook
│   │   ├── useCartesianJog.ts         # 笛卡尔点动 hook
│   │   ├── useMotionCommand.ts        # 运动指令 hook
│   │   └── useModelLoader.ts          # 模型加载 hook
│   │
│   └── styles/
│       ├── global.css                 # 全局样式
│       └── theme.ts                   # Ant Design 主题配置
│
├── tests/
│   ├── unit/
│   │   ├── YamlModelParser.test.ts
│   │   ├── ForwardKinematics.test.ts
│   │   └── RobotHttpClient.test.ts
│   └── e2e/
│       └── connection.spec.ts
│
├── index.html
├── package.json
├── tsconfig.json
├── vite.config.ts
└── .eslintrc.cjs
```

---

## 9. 开发阶段与里程碑

### Phase 1：基础框架（2-3 周）

**目标**：项目骨架可运行，能连接控制器并显示连接状态。

- [ ] Vite + React + TypeScript 项目初始化
- [ ] Ant Design 集成 + 主题配置（深色/浅色）
- [ ] 主布局实现（左右分栏 + 工具栏 + 状态栏）
- [ ] Zustand stores 基础结构
- [ ] HTTP REST 客户端封装（RobotHttpClient）
- [ ] WebSocket 状态流 + HTTP 降级
- [ ] 连接弹窗（ConnectDialog）
- [ ] 连接/断开/重连逻辑
- [ ] 连接状态指示灯

**验收标准**：输入 IP/Port 能连接到控制器，状态栏显示连接状态，断线自动重连。

### Phase 2：3D 渲染核心（2-3 周）

**目标**：机器人模型正确加载和渲染，关节角度实时更新。

- [ ] R3F Canvas 基础场景（灯光、背景、网格）
- [ ] YAML 模型解析器（YamlModelParser）
- [ ] STL / DAE mesh 加载（Three.js STLLoader / ColladaLoader）
- [ ] 关节层级构建（Object3D 父子关系）
- [ ] 正运动学更新逻辑
- [ ] OrbitControls 轨道控制
- [ ] 4 个相机预设 + 切换
- [ ] 关节轴显示/隐藏
- [ ] 网格/实体显示切换
- [ ] 地面网格显示/隐藏
- [ ] 姿态指示器（左下角小坐标轴）

**验收标准**：加载 iiwa / ur5e / talon 任一模型，关节角度更新时 3D 模型实时跟随运动，姿态与桌面版一致。逐关节对比验证 FK 正确性。

### Phase 3：控制面板（2 周）

**目标**：所有，能控制机器人运动。

- [ ] 关节点动面板（7 轴 × 进度条 + ±按钮）
- [ ] 笛卡尔点动面板（6 轴 × 进度条 + ±按钮）
- [ ] 速度滑块（1-999）
- [ ] 参考坐标系切换（BASE/FLANGE/TOOL/OBJECT）
- [ ] 工作模式切换（5 种）
- [ ] 使能/禁用按钮
- [ ] 精确笛卡尔控制（输入框 + MoveJ_IK / MoveL 按钮）
- [ ] 精确关节控制（输入框 + MoveJ / MoveL_FK 按钮）
- [ ] 运动状态异步追踪
- [ ] 单位切换（度/弧度，mm/m）

**验收标准**：通过 Web 界面可完成与桌面版相同的所有控制操作。

### Phase 4：高级功能（2 周）

**目标**：图表、轨迹、模型下载等辅助功能完整。

- [ ] ECharts 实时图表组件
- [ ] 2x2 图表布局
- [ ] 图表数据源配置对话框（树形选择器）
- [ ] 10s 滑动窗口 + 数据追加
- [ ] 轨迹线渲染（Line2 粗线支持）
- [ ] 远程模型下载 + IndexedDB 缓存
- [ ] Lua 脚本编辑器（CodeMirror 6 + Lua 语法）
- [ ] 脚本执行/停止/暂停/继续控制

**验收标准**：图表实时显示关节数据，轨迹正确渲染，模型可从控制器下载加载。

### Phase 5：打磨与测试（1-2 周）

**目标**：产品级质量，响应式适配，充分测试。

- [ ] 响应式布局（Desktop / Tablet / Mobile 断点）
- [ ] 错误处理（网络异常、API 错误、模型加载失败）
- [ ] 断线提示与恢复
- [ ] 性能优化（WebSocket 50Hz 稳定性测试、大模型渲染优化）
- [ ] 单元测试（YamlModelParser、ForwardKinematics、RobotHttpClient）
- [ ] E2E 测试（连接、控制、图表基本流程）
- [ ] 暗色主题适配
- [ ] 关于对话框
- [ ] 时间显示（当前时间 + 运行时间）

---

## 10. 风险与缓解措施

| 风险 | 严重性 | 缓解措施 |
|---|---|---|
| YAML 模型 FK 变换与桌面版不一致 | 高 | 编写 FK 单元测试，与桌面版逐关节输出对比验证 |
| WebSocket 控制器端未实现 | 高 | 保留 HTTP 轮询降级方案，优先保证 REST 通信可用 |
| CORS 跨域限制 | 高 | 要求控制器端添加 CORS 头；或开发时使用 Vite 代理 |
| 20ms 状态更新在浏览器中不稳定 | 中 | WebSocket 推送替代轮询；必要时降低到 50ms |
| Three.js linewidth 仅支持 1px | 低 | 使用 Line2（fat lines）实现粗线轨迹 |
| 浏览器内存限制 | 中 | 及时 dispose 不用的几何体/材质；图表数据滑动窗口 |
| Lua 脚本编辑器体积 | 低 | CodeMirror 6 仅 ~150KB；可按需动态加载 |

---

## 11. 与现有桌面版的差异说明

### Web 版新增能力
- 跨平台零安装，任何设备浏览器即可访问
- 多人可同时连接同一控制器
- 响应式布局，支持平板/手机查看状态
- 丰富的图表交互（缩放、平移、tooltip）
- URL 分享特定视图配置

### Web 版受限能力
- 无法直接写入本地文件系统（Lua 脚本、录制数据导出为下载）
- 无系统级通知（可通过 Web Notification API 弥补）
- 原生性能上限略低于 VTK（大模型 >500K 面数需 LOD 优化）
- 需要控制器端配合 CORS 配置

---

## 附录 A：关键参考项目

| 项目 | 参考价值 |
|---|---|
| [Foxglove Studio](https://github.com/foxglove/studio) | React + Three.js 面板系统架构 |
| [urdf-loader](https://github.com/gkjohnson/urdf-loader) | Three.js URDF 模型加载器 |
| [@react-three/fiber](https://github.com/pmndrs/react-three-fiber) | React 声明式 3D 开发 |
| [@react-three/drei](https://github.com/pmndrs/drei) | R3F 实用组件库 |
| [ECharts](https://echarts.apache.org/) | 实时数据可视化 |
| [CodeMirror 6](https://codemirror.net/) | 轻量代码编辑器 |

## 附录 B：ROCOS API 端点速查

| 方法 | 端点 | 说明 |
|---|---|---|
| GET | `/api/robot/info` | 机器人信息 |
| GET | `/api/robot/state` | 实时状态（20ms） |
| GET | `/api/robot/enabled` | 上电状态 |
| GET | `/api/robot/model` | 模型结构 |
| GET | `/api/robot/model/mesh?path=` | 下载 mesh |
| POST | `/api/robot/enable` | 上电 |
| POST | `/api/robot/disable` | 下电 |
| POST | `/api/robot/workmode` | 工作模式 |
| POST | `/api/drag/start` | 点动开始 |
| POST | `/api/drag/stop` | 点动停止 |
| POST | `/api/move/joint` | 关节运动 |
| POST | `/api/move/joint_ik` | IK 运动 |
| POST | `/api/move/linear` | 直线运动 |
| POST | `/api/move/linear_fk` | FK 直线运动 |
| POST | `/api/move/stop` | 停止运动 |
| GET | `/api/move/status?task_id=` | 运动状态 |
| POST | `/api/script/run` | 运行脚本 |
| POST | `/api/script/stop` | 停止脚本 |
| POST | `/api/script/pause` | 暂停脚本 |
| POST | `/api/script/continue` | 继续脚本 |
