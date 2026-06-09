# Three.js + urdf-loader 替代 ROCOS-Viz Qt/VTK 方案可行性评估

> 评估日期：2026-06-09
> 评估范围：ROCOS-Viz 现有全部功能与 Web 技术替代方案的逐项对比

---

## 1. 执行摘要

**总体结论：技术上可行，但需要大量定制开发工作，且在模型格式兼容性方面存在核心障碍。**

Three.js + urdf-loader 能够覆盖 ROCOS-Viz 约 **85%** 的功能需求。在 HTTP 通信层（当前已采用 REST API）、UI 控件实现、实时数据图表等方面，Web 方案具有天然优势。然而，以下三个领域需要重点关注：

1. **模型格式不兼容**：当前 ROCOS-Viz 使用自定义 YAML 格式描述机器人运动学链（含 `translate`/`rotate`/`angleAxis`/`translateLink`/`rotateLink`），而 urdf-loader 严格要求标准 URDF XML 格式。两者之间没有直接映射关系，需要编写格式转换器。
2. **20ms 轮询的实时性风险**：HTTP 轮询在浏览器环境下受连接池限制（HTTP/1.1 同域 6 连接）和浏览器调度影响，端到端延迟不确定性高于 Qt 原生应用。
3. **离线能力与部署约束**：浏览器应用天然受限于同源策略、CORS 配置、文件系统访问权限，无法像桌面应用一样自由读写本地文件（如 Lua 脚本、模型缓存、数据录制文件）。

---

## 2. 功能对照评估表

### 2.1 3D 机器人模型渲染

| 维度 | 现有实现 (Qt/VTK) | Web 方案 (Three.js) | 可行性 |
|------|-------------------|---------------------|--------|
| **STL 加载** | `vtkSTLReader` 直接读取本地 STL 文件 | Three.js `STLLoader` 通过 HTTP/fetch 加载 | 完全替代 |
| **DAE 加载** | VTK 支持（但当前代码仅用 STL） | Three.js `ColladaLoader` 原生支持 | 完全替代 |
| **关节层级构建** | `Model.cpp` 手动解析 YAML，用 Eigen 矩阵链式相乘计算 FK | 需编写自定义 loader 解析 YAML，用 `THREE.Matrix4` 链式变换 | 完全替代（需自定义） |
| **正运动学变换** | `Model::updateModel()` 中 Eigen `Affine` 变换累加 | Three.js `Object3D` 层级结构 + `rotation.set()` | 完全替代 |
| **关节轴显示** | `vtkAxesActor` 绘制每个关节坐标系 | Three.js `AxesHelper` + 自定义着色器 | 完全替代 |
| **Mesh/Wireframe 切换** | `vtkProperty::SetRepresentationToWireframe()` | `material.wireframe = true/false` | 完全替代 |
| **渐变背景** | `renderer->GradientBackgroundOn()` | `scene.background = new THREE.Color()` + CSS 或自定义 shader | 完全替代 |
| **姿态指示 Marker** | `vtkOrientationMarkerWidget`（左下角小坐标系） | overlay HTML/CSS + 独立 renderer，或 Three.js CSS2DRenderer | 完全替代 |
| **地面网格** | `vtkPlaneSource` wireframe | `THREE.GridHelper` | 完全替代 |
| **Logo 显示** | `vtkImageActor` 加载 PNG 并放置在场景中 | `THREE.Sprite` 或 `THREE.PlaneGeometry` + texture | 完全替代 |
| **Trackball 交互** | `MouseInteractorChooseActor`（自定义 trackball） | Three.js `TrackballControls` 或 `OrbitControls` | 完全替代 |

**实现复杂度**：中等。STL 加载和场景搭建有成熟的 Three.js 库支持。核心工作量在于编写 YAML 模型解析器和自定义 FK 变换链（见第 5 节）。

### 2.2 实时关节角度同步更新（20ms 间隔）

| 维度 | 现有实现 | Web 方案 | 可行性 |
|------|---------|---------|--------|
| **轮询机制** | `QTimer` 以 20ms 间隔触发 `getRobotState()`，通过 `QNetworkAccessManager` HTTP GET | `setInterval` 或 `requestAnimationFrame` + `fetch()` | 完全替代 |
| **状态解析** | `ConnectDialog::parseRobotState()` 解析 JSON | 原生 `JSON.parse()` | 完全替代（更简单） |
| **3D 更新** | `updateJointPos()` -> `Model::updateModel()` -> `renderWindow->Render()` | 直接设置 `Object3D.rotation` + `requestAnimationFrame` 驱动渲染循环 | 完全替代 |

**性能对比分析**：

| 指标 | Qt/VTK | Three.js/Web | 备注 |
|------|--------|-------------|------|
| HTTP 请求往返时间 | 本地同机 ~1-5ms | 本地同机 ~2-10ms | 浏览器 HTTP 栈略重 |
| JSON 解析 | `QJsonDocument::fromJson()` | `JSON.parse()` | 性能相当 |
| 渲染触发 | `QTimer` 50ms 间隔显式 Render | `requestAnimationFrame` ~16.7ms 被动渲染 | Web 更平滑 |
| 帧率上限 | 20 FPS (50ms timer) | 60 FPS (rAF) | Web 更优 |
| 最大轮询频率 | 50 Hz (POLLING_INTERVAL_MS=20) | 50 Hz 可行 | 需测试浏览器实际调度 |

**风险**：
- **连接池限制**：HTTP/1.1 浏览器同域最多 6 个并发连接。50Hz 轮询意味着每秒 50 个请求，如果单次请求 >40ms 可能导致排队。
- **方案优化**：考虑 WebSocket 或 SSE 替代 HTTP 轮询以消除连接池限制和请求开销。

### 2.3 关节点动控制 UI

| 维度 | 现有实现 | Web 方案 | 可行性 |
|------|---------|---------|--------|
| **进度条显示** | `PositionBar`（继承 `QProgressBar`），双击精度浮点显示 | 自定义 React/Vue 组件 + CSS 进度条 | 完全替代 |
| **+/- 按钮** | `JointPositionWidget` 的 `pressed()`/`released()` 信号 | `mousedown`/`mouseup` 事件绑定 | 完全替代 |
| **点动逻辑** | `pressed` 发送 `jointJogging(id, +1)`，`released` 发送 `jointJogging(id, 0)` | 同理，fetch POST `/api/drag/start` 和 `/api/drag/stop` | 完全替代 |
| **速度缩放** | 全局 `speedSlider` (0-100%) 传递到 `setFactor()` | HTML `<input type="range">` + state 管理 | 完全替代 |
| **7 轴支持** | 硬编码 7 个 `JointPositionWidget` | 动态渲染，根据机器人关节数自适应 | Web 更优 |

**实现复杂度**：低。Web UI 控件实现比 Qt 更直观。

### 2.4 笛卡尔点动控制

| 维度 | 现有实现 | Web 方案 | 可行性 |
|------|---------|---------|--------|
| **6 自由度显示** | X/Y/Z/Roll/Pitch/Yaw 各一个 `CartesianPositionWidget` | 同理 6 个自定义组件 | 完全替代 |
| **坐标系切换** | `frameBox` 下拉框：BASE/FLANGE/TOOL/OBJECT，通过偏移量编码 | 下拉框 + 坐标系枚举，发送不同的 flag 字符串 | 完全替代 |
| **角度/单位切换** | `actionAngleRep` / `actionPosRep` 切换度/弧度、mm/m | 切换按钮/开关，前端做单位转换 | 完全替代 |
| **点动发送** | `cartesianJogging(currentFrame + freedom, id, dir)` | 同理，POST `/api/drag/start` with flag="BASE_X" 等 | 完全替代 |

### 2.5 运动指令发送与异步状态追踪

| 维度 | 现有实现 | Web 方案 | 可行性 |
|------|---------|---------|--------|
| **MoveJ** | POST `/api/move/joint` with joints array | 同上 | 完全替代 |
| **MoveL** | POST `/api/move/linear` with pose (position + quaternion) | 同上 | 完全替代 |
| **MoveJ_IK** | POST `/api/move/joint_ik` | 同上 | 完全替代 |
| **MoveL_FK** | POST `/api/move/linear_fk` | 同上 | 完全替代 |
| **异步状态追踪** | `timer_move_status_` 每 200ms 查询 `/api/move/status?task_id=xxx` | `setInterval` + fetch 查询 | 完全替代 |
| **RPY 到四元数转换** | 使用 KDL 库 `Rotation::RPY().GetQuaternion()` | 需使用 THREE.Quaternion 或 math.js 或 gl-matrix | 需引入库（~5KB） |

**实现复杂度**：低。API 层已经是 REST/JSON，直接对接。

### 2.6 多种工作模式切换

| 维度 | 现有实现 | Web 方案 | 可行性 |
|------|---------|---------|--------|
| **模式列表** | 下拉框 5 种：position, ee_admit_teach, jnt_admit_teach, jnt_imp, cart_imp | 同理下拉框 | 完全替代 |
| **发送** | POST `/api/robot/workmode` with `{mode: "xxx"}` | 同上 | 完全替代 |

### 2.7 实时数据图表

| 维度 | 现有实现 | Web 方案 | 可行性 |
|------|---------|---------|--------|
| **图表引擎** | `QtCharts`（QChartView, QLineSeries） | Chart.js / ECharts / plotly.js / D3.js | 完全替代 |
| **最多 4 个并行图表** | 2x2 布局 `QChartView` 列表，最多 4 个 | CSS Grid 2x2 布局，动态挂载 4 个 canvas | 完全替代 |
| **数据源配置** | `PlotConfigDialog` 树形选择器，选择要显示的关节/笛卡尔变量 | 树形 UI 组件（如 react-treeview） | 完全替代 |
| **实时追加数据** | 每次 `handleNewState()` 调用时追加新数据点到 series | 同理 push 数据点 + chart.update() | 完全替代 |
| **录制与导出** | `QFile` 写入 CSV（jntPosFile, cartPoseFile） | Blob + download API 导出 CSV | Web 更优（用户可直接下载） |
| **自动缩放** | `isAutoScale` 标志控制 Y 轴自动范围 | ECharts/Chart.js 内置 auto-scale | 完全替代 |

**Web 方案优势**：Web 图表库功能远超 QtCharts，支持缩放、平移、tooltip、数据点标记等丰富交互。

### 2.8 Lua 脚本编辑与执行

| 维度 | 现有实现 | Web 方案 | 可行性 |
|------|---------|---------|--------|
| **脚本编辑** | `ScriptDialog` 中的 `QComboBox` 选择脚本路径 | 下拉框 + 文本输入 | 完全替代 |
| **脚本执行** | `startScript(path)` -> HTTP POST（当前实现为空） | 同理 fetch POST | 完全替代 |
| **脚本控制** | 运行/停止/暂停/继续 4 个按钮 | 同理 4 个按钮 | 完全替代 |

**注意**：当前 `ConnectDialog::startScript()` 等函数体为空（未实现），因此这部分在桌面版也未完成。

### 2.9 连接管理与状态显示

| 维度 | 现有实现 | Web 方案 | 可行性 |
|------|---------|---------|--------|
| **连接对话框** | `ConnectDialog` 无边框弹窗，输入 IP/Port | 模态弹窗/侧边栏，输入 IP/Port | 完全替代 |
| **连接测试** | GET `/api/robot/info`，成功则连接 | 同理 | 完全替代 |
| **状态图标** | 连接/断开图标切换 | 状态指示灯/图标（SVG） | 完全替代 |
| **自动加载模型** | 连接时可选自动下载模型 | 同理 checkbox | 完全替代 |
| **断线处理** | 10 次失败后 `shutdown()` | 同理重试逻辑 + 断线提示 | 完全替代 |
| **失焦关闭** | `event()` 重写，`ActivationChange` 事件 | `blur` 事件监听 | 完全替代 |

**Web 方案优势**：可利用 WebSocket 实现连接状态的实时推送，替代 HTTP 轮询检测。

### 2.10 相机视图切换

| 维度 | 现有实现 | Web 方案 | 可行性 |
|------|---------|---------|--------|
| **轴测图** | `camera->SetPosition(3,3,3), SetFocalPoint(0,0,0.5)` | `camera.position.set(3,3,3)` + controls.update() | 完全替代 |
| **俯视图 (Z)** | `camera->SetPosition(0,0,3)` | `camera.position.set(0,0,3)` | 完全替代 |
| **前视图 (X)** | `camera->SetPosition(3,0,0.5)` | `camera.position.set(3,0,0.5)` | 完全替代 |
| **右视图 (Y)** | `camera->SetPosition(0,3,0.5)` | `camera.position.set(0,3,0.5)` | 完全替代 |
| **动画过渡** | 直接设置（无动画） | 可用 TWEEN.js 添加平滑过渡（Web 方案增强） | Web 更优 |

### 2.11 轨迹显示

| 维度 | 现有实现 | Web 方案 | 可行性 |
|------|---------|---------|--------|
| **轨迹点收集** | `vtkPoints` 收集末端位置，最多 500 点 | `THREE.BufferGeometry` + 动态 attribute | 完全替代 |
| **轨迹渲染** | `vtkPolyLine` + `vtkPolyDataMapper` | `THREE.Line` 或 `THREE.LineSegments` | 完全替代 |
| **轨迹颜色/线宽** | 蓝色 `RGB(73,111,255)`，线宽 3 | `LineBasicMaterial({color: 0x496fff, linewidth: 3})` | 完全替代 |
| **轨迹清除** | `_points->SetNumberOfPoints(0)` | `geometry.setDrawRange(0, 0)` 或重新创建 | 完全替代 |

**注意**：Three.js 的 `linewidth` 在大多数 WebGL 实现中仅支持 1px。若需要可变宽度线，需使用 `Line2`（three/examples 中的 fat lines）。

### 2.12 关节轴可视化

| 维度 | 现有实现 | Web 方案 | 可行性 |
|------|---------|---------|--------|
| **坐标轴显示** | 每个关节的 `vtkAxesActor`，可隐藏/显示 | 每个关节的 `THREE.AxesHelper`，`visible = true/false` | 完全替代 |
| **轴长** | 0.2 单位 | `new THREE.AxesHelper(0.2)` | 完全替代 |

### 2.13 角度/位置单位切换

| 维度 | 现有实现 | Web 方案 | 可行性 |
|------|---------|---------|--------|
| **角度单位** | 弧度/度切换，影响 r2d 系数 | 前端状态切换，显示时乘以 180/PI 或 1 | 完全替代 |
| **位置单位** | mm/m 切换，影响 m2mm 系数 | 前端状态切换，显示时乘以 1000 或 1 | 完全替代 |

### 2.14 远程模型下载与加载

| 维度 | 现有实现 | Web 方案 | 可行性 |
|------|---------|---------|--------|
| **获取模型结构** | GET `/api/robot/model` -> JSON 链接信息 | 同上 | 完全替代 |
| **下载 Mesh 文件** | GET `/api/robot/model/mesh?path=xxx` -> 二进制 STL | fetch blob -> blob URL 或 ArrayBuffer | 完全替代 |
| **写入本地文件** | `QTemporaryFile` + `QFile::rename` 保存到 `models/` 目录 | **无法写入本地文件系统**（受限于浏览器安全策略） | **部分替代** |
| **生成 config.yaml** | `QTextStream` 写入 YAML | 可生成但无法保存到本地 | **部分替代** |
| **内存缓存模型** | 无（每次从磁盘加载） | blob URL + 内存缓存 | Web 更优（但需注意内存） |

**关键差异**：桌面版将远程下载的模型文件保存到本地磁盘（`models/robot_name/`），后续可离线加载。Web 版无法写入本地文件系统，只能在内存中缓存或使用 IndexedDB。这意味着每次刷新页面可能需要重新下载模型。

### 2.15 标定与工件坐标系设置

| 维度 | 现有实现 | Web 方案 | 可行性 |
|------|---------|---------|--------|
| **标定 API** | 当前桌面版**未实现**标定 UI（API 已定义但 ConnectDialog 中无相关调用） | 可在 Web 版中实现 | Web 可补充 |
| **相关 API** | `/api/calibration/pose`, `/api/calibration/tool`, `/api/calibration/object`, `/api/calibration/run` | 直接调用 | 完全替代 |

---

## 3. 性能风险分析

### 3.1 20ms 轮询延迟分析

```
当前桌面版链路：
  QTimer(20ms) -> getRobotState() -> QNetworkManager GET -> JSON.parse -> 
  newStateComming signal -> updateJointPos() -> Model::updateModel() -> Render()
  典型延迟：5-15ms (同机 localhost)

Web 版链路：
  setInterval(20ms) -> fetch(GET) -> JSON.parse -> setState() -> 
  useEffect -> Three.js updateMatrixWorld() -> requestAnimationFrame -> render()
  典型延迟：10-30ms (同机 localhost)
```

**风险等级**：中等。在同机 localhost 场景下延迟可接受，但如果 Web 应用和机器人控制器不在同一台机器上（通过网络访问），网络延迟叠加可能使总延迟超过 20ms，导致数据陈旧。

### 3.2 渲染性能

| 场景 | Qt/VTK | Three.js | 备注 |
|------|--------|----------|------|
| 7 轴机器人 STL 模型 | ~50K 三角面，60 FPS 轻松 | 同等面数，WebGL 60 FPS | 相当 |
| 轨迹线 500 点 | 几乎无负担 | 动态 BufferGeometry 更新，几乎无负担 | 相当 |
| 4 个实时图表 | QtCharts 可能卡顿 | ECharts/D3 高性能 canvas 渲染 | Web 更优 |
| 大模型（>500K 三角面） | VTK 原生性能更好 | 需 LOD（Level of Detail）优化 | 桌面更优 |

### 3.3 内存管理

- **桌面版**：VTK 智能指针自动管理内存，STL 文件加载后常驻内存。
- **Web 版**：Three.js 的 `BufferGeometry` 和 `Mesh` 需手动 `dispose()` 释放 GPU 资源。频繁加载/卸载模型时需注意内存泄漏。

### 3.4 并发连接

当前 `ConnectDialog` 在 20ms 轮询期间还可能同时发送运动指令、查询 move status。浏览器 HTTP/1.1 同域限制 6 个并发连接，高频轮询可能阻塞命令发送。

**解决方案**：
1. 使用 WebSocket 替代 HTTP 轮询（推荐）
2. 使用 HTTP/2 多路复用
3. 降低轮询频率（如 50ms），接受稍低的更新率

---

## 4. 模型格式兼容性分析

### 4.1 现有 YAML 格式

当前 ROCOS-Viz 的 `config.yaml` 格式定义了以下字段：

```yaml
robot:
  - name: link_0          # 连杆名称
    order: 0               # 序号
    mesh: base_link.STL    # 网格文件（相对路径）
    # 以下为可选字段
    translate: [x, y, z]           # 关节相对前一关节的平移
    rotate: [roll, pitch, yaw]     # 关节相对前一关节的旋转（欧拉角 Rz*Ry*Rx）
    angleAxis: [x, y, z]           # 关节旋转轴方向
    translateLink: [x, y, z]       # 连杆网格的额外平移
    rotateLink: [roll, pitch, yaw] # 连杆网格的额外旋转
    type: continuous|revolute|fixed|prismatic  # 关节类型
```

### 4.2 urdf-loader 要求的 URDF 格式

urdf-loader 要求标准的 URDF XML 格式：

```xml
<robot name="myrobot">
  <link name="base_link">
    <visual>
      <geometry><mesh filename="base_link.STL"/></geometry>
    </visual>
  </link>
  <joint name="joint_1" type="revolute">
    <parent link="base_link"/>
    <child link="link_1"/>
    <origin xyz="0 0 0.15" rpy="0 0 0"/>
    <axis xyz="0 0 1"/>
  </joint>
  ...
</robot>
```

### 4.3 格式差异对比

| 特性 | ROCOS YAML | URDF XML | 兼容性 |
|------|-----------|----------|--------|
| **关节定义** | 扁平列表，隐式父子关系（按 order 顺序） | 显式 `<parent>` / `<child>` 标签 | **不兼容** |
| **平移** | `translate: [x, y, z]` | `<origin xyz="x y z"/>` | 字段名不同但语义相同 |
| **旋转** | `rotate: [roll, pitch, yaw]`（欧拉角） | `<origin rpy="r p y"/>` | 语义相同 |
| **旋转轴** | `angleAxis: [x, y, z]` | `<axis xyz="x y z"/>` | 语义相同 |
| **连杆偏移** | `translateLink` + `rotateLink`（网格相对于关节的额外变换） | URDF 中无直接对应，需通过 `<visual><origin>` 实现 | **需转换** |
| **关节类型** | `continuous` / `revolute` / `fixed` / `prismatic` | 同名类型 | 完全兼容 |
| **网格路径** | 相对路径 | 相对路径（需 resolve） | 需路径处理 |
| **父连杆** | 隐式（前一个 link） | 显式声明 | 需推导 |

### 4.4 转换方案

**方案 A：编写 YAML-to-URDF 转换器**（推荐）

在 Web 端编写 JavaScript 转换函数，将下载的 YAML 解析后动态生成 URDF 字符串，再用 urdf-loader 加载。

```javascript
function yamlToUrdf(yamlData, baseUrl) {
  // 解析 YAML（可用 js-yaml 库）
  // 构建 URDF XML 字符串
  // 注意 translateLink/rotateLink -> visual origin
  // 注意隐式父子关系 -> 显式 parent/child
  // resolve 网格文件路径为绝对 URL
}
```

关键转换逻辑：
- 按 `order` 排序 links
- 每个 link 的前一个 link 作为 parent
- `translate` + `rotate` -> joint `<origin>`
- `angleAxis` -> joint `<axis>`
- `translateLink` + `rotateLink` -> link `<visual><origin>`
- 相对 mesh 路径 -> 绝对 URL（基于 baseUrl）

预估工作量：2-3 天（含测试）。

**方案 B：使用 ROS urdf_parser**

如果有 ROS 环境，可以使用 `xacro` 或自定义脚本预处理为 URDF。但这增加了构建依赖。

**方案 C：改造 urdf-loader 直接解析 YAML**

fork urdf-loader，扩展其 loader 支持自定义 YAML 格式。维护成本较高，不推荐。

---

## 5. 建议的技术方案与实施路径

### 5.1 推荐技术栈

| 层次 | 推荐方案 | 备选方案 | 理由 |
|------|---------|---------|------|
| **3D 渲染** | Three.js r160+ | Babylon.js | Three.js 生态更大，urdf-loader 仅支持 Three.js |
| **模型加载** | urdf-loader + 自定义 YAML-to-URDF 转换器 | robogui-loader | urdf-loader 是最成熟的 Three.js 机器人模型加载器 |
| **UI 框架** | React + TypeScript | Vue 3 + TypeScript | 组件化 UI 开发效率高 |
| **状态管理** | Zustand 或 Jotai | Redux | 轻量级，适合实时状态频繁更新 |
| **图表** | ECharts | Chart.js, plotly.js | ECharts 性能好，支持大数据流，中文文档丰富 |
| **代码编辑器** | Monaco Editor | CodeMirror 6 | Monaco 功能最强（VS Code 内核），但包体较大；若仅用于简单 Lua 编辑可选 CodeMirror |
| **构建工具** | Vite | Webpack | Vite 开发体验好，HMR 快 |
| **通信层** | WebSocket (主通道) + fetch REST (命令) | 纯 HTTP 轮询 | WebSocket 低延迟，适合状态推送；命令用 REST 更直观 |

### 5.2 实施路径

```
Phase 1: 基础框架（2-3 周）
├── 项目初始化 (Vite + React + TypeScript)
├── WebSocket 通信层封装
├── 连接管理对话框
├── 状态轮询/推送机制
└── 基本布局（3D 视口 + 侧边栏 + 工具栏）

Phase 2: 3D 渲染核心（2-3 周）
├── Three.js 场景搭建
├── YAML 模型解析器 + URDF 转换器
├── urdf-loader 集成
├── 正运动学变换链
├── 关节轴/地面/背景/Marker 等辅助元素
└── 相机视图切换

Phase 3: 控制 UI（2 周）
├── 关节点动控制（7 轴 + 进度条 + 按钮）
├── 笛卡尔点动控制（6 DOF + 坐标系切换）
├── 速度滑块
├── 运动指令面板（MoveJ/MoveL/MoveJ_IK/MoveL_FK）
├── 工作模式切换
└── 使能/禁用控制

Phase 4: 高级功能（2 周）
├── 实时数据图表（ECharts, 2x2 布局）
├── 轨迹显示
├── 角度/位置单位切换
├── 远程模型下载与缓存
└── Lua 脚本编辑器

Phase 5: 打磨与测试（1 周）
├── 响应式布局适配
├── 错误处理与离线提示
├── 性能优化（20ms 轮询稳定性测试）
└── 文档与部署
```

**总预估工时**：9-11 周（1 人全栈开发者）

### 5.3 项目结构建议

```
rocos-web/
├── src/
│   ├── core/
│   │   ├── RobotClient.ts          # WebSocket + REST 通信层
│   │   ├── YamlModelParser.ts      # YAML 模型解析
│   │   ├── YamlToUrdf.ts           # YAML -> URDF XML 转换
│   │   ├── ForwardKinematics.ts    # 正运动学（可选，或用 urdf-loader 内置）
│   │   └── StateManager.ts         # 机器人状态管理
│   ├── scene/
│   │   ├── RobotScene.ts           # Three.js 场景管理
│   │   ├── CameraController.ts     # 相机视图切换
│   │   ├── TrajectoryRenderer.ts   # 轨迹渲染
│   │   └── AxesIndicator.ts        # 姿态指示器
│   ├── components/
│   │   ├── JointJogPanel.tsx        # 关节点动面板
│   │   ├── CartesianJogPanel.tsx    # 笛卡尔点动面板
│   │   ├── MotionCommandPanel.tsx   # 运动指令面板
│   │   ├── PlotPanel.tsx            # 实时图表面板
│   │   ├── ConnectDialog.tsx        # 连接对话框
│   │   ├── ScriptEditor.tsx         # Lua 脚本编辑器
│   │   └── StatusBar.tsx            # 状态栏
│   └── App.tsx
├── package.json
└── vite.config.ts
```

---

## 6. 需要特别注意的风险点

### 6.1 高风险

| 风险 | 影响 | 缓解措施 |
|------|------|---------|
| **YAML -> URDF 转换正确性** | FK 变换结果不正确，模型显示姿态错误 | 编写单元测试，与桌面版逐关节对比验证 |
| **20ms HTTP 轮询在浏览器中的稳定性** | 更新卡顿、数据延迟 | 改用 WebSocket；或接受 50ms 间隔降级 |
| **跨域 CORS 配置** | 无法访问机器人控制器 API | 要求控制器端添加 CORS 头，或部署反向代理 |

### 6.2 中等风险

| 风险 | 影响 | 缓解措施 |
|------|------|---------|
| **STL 加载性能** | 大模型首次加载慢 | 预加载 + IndexedDB 缓存 + 压缩传输 |
| **浏览器内存限制** | 大模型 + 图表导致 OOM | 及时 dispose，图表数据滑动窗口 |
| **WebGL 兼容性** | 低版本浏览器不支持 | 明确支持矩阵：Chrome 90+, Firefox 90+, Edge 90+ |
| **无离线能力** | 断网后无法使用 | Service Worker + PWA 缓存策略 |

### 6.3 低风险

| 风险 | 影响 | 缓解措施 |
|------|------|---------|
| **Lua 脚本编辑器体积** | Monaco Editor ~5MB | 改用 CodeMirror (~500KB)，或动态加载 |
| **轨迹线宽** | Three.js 默认 linewidth=1 | 使用 Line2（fat lines） |
| **文件录制导出** | 浏览器无法直接写文件 | Blob + download API，用户手动保存 |

---

## 7. Web 方案能做到但桌面版做不到的（增强功能）

1. **跨平台零安装**：任何设备的浏览器即可访问，无需安装 Qt/VTK 运行环境
2. **多人协作**：多个浏览器同时连接同一机器人控制器
3. **响应式布局**：自适应不同屏幕尺寸，支持平板/手机查看
4. **丰富的图表交互**：缩放、平移、数据点 tooltip、图表导出为图片
5. **PWA 离线缓存**：首次加载后可缓存应用资源
6. **一键分享**：通过 URL 分享特定视图配置
7. **版本管理**：前端代码部署更新无需用户手动升级
8. **扩展性**：可轻松集成 ROS2 Web 工具（roslibjs）、数字孪生平台

## 8. 桌面版能做到但 Web 方案做不到或难以实现的

1. **本地文件系统直接读写**：Lua 脚本文件、模型文件、录制数据无法直接写入磁盘
2. **系统级通知**：运动完成/报警等系统通知（可通过 Web Notification API 部分弥补）
3. **USB/串口设备通信**：如果未来需要直接连接硬件（当前未使用）
4. **原生性能上限**：VTK 的大模型渲染和 C++ 后端处理性能高于 JavaScript
5. **确定性延迟**：Qt 事件循环的确定性调度优于浏览器的任务调度

---

## 9. 结论

Three.js + urdf-loader 方案**可以替代 ROCOS-Viz 的所有核心功能**，主要障碍是模型格式转换（需编写 YAML-to-URDF 转换器）和 HTTP 通信的实时性保障（建议用 WebSocket 替代轮询）。

建议采用 **Phase-by-Phase 增量式开发**，先搭建 3D 渲染核心验证模型加载和 FK 变换的正确性，再逐步实现控制 UI 和高级功能。在 Phase 2 完成后即可与桌面版进行对比验证，确保关键路径无误后再投入后续开发。
