# Web 端机器人可视化与控制界面 -- 技术调研报告

> **项目**: rocos-viz (ROCOS Visualization Application)
> **目标**: 为工业机器人控制器开发 Web 端可视化与远程控制界面，替代现有 Qt/VTK 桌面应用
> **日期**: 2026-06-09
> **当前技术栈**: Qt 5/6 + VTK 9 + Eigen3 + yaml-cpp + orocos_kdl

---

## 1. 现有系统分析

### 1.1 当前架构

rocos-viz 是一个基于 Qt + VTK 的桌面应用，核心功能模块包括：

| 模块       | 实现                         | 说明                             |
| ---------- | ---------------------------- | -------------------------------- |
| 3D 渲染    | VTK 9 + QOpenGLWidgets       | 机器人模型场景渲染、鼠标交互     |
| 关节控制   | JointPositionWidget          | 6-7 轴关节角度显示与点动控制     |
| 笛卡尔控制 | CartesianPositionWidget      | 基/工具/法兰坐标系下的笛卡尔点动 |
| 数据图表   | QChart (Qt Charts)           | 关节/笛卡尔空间的实时波形图      |
| 脚本编辑   | ScriptDialog                 | Lua 脚本的运行/暂停/停止         |
| 通信层     | HttpRobotClient (Qt Network) | HTTP/JSON REST API 客户端        |
| 模型加载   | ModelLoaderDialog            | 自定义 YAML 格式 + DAE/STL mesh  |

### 1.2 通信协议

当前已迁移到 HTTP/JSON REST API：

- 状态轮询: `GET /api/robot/state` (20ms 周期)
- 控制命令: `POST /api/drag/start|stop`, `POST /api/move/joint|linear|...`
- 模型下载: `GET /api/robot/model`, `GET /api/robot/model/mesh`
- Lua 脚本: `POST /api/script/run|stop|pause|continue`

### 1.3 机器人模型格式

当前使用自定义 YAML 配置 + 分离 mesh 文件 (DAE/STL)：

```yaml
robot:
  - name: link_0
    mesh: base.dae
  - name: link_1
    translate: [0.0, 0.0, 0.1519]
    angleAxis: [0, 0, 1]
    mesh: shoulder.dae
```

**注意**: 这与标准 URDF 格式不同。Web 端需要开发自定义模型解析器或转换为 URDF/glTF 格式。

---

## 2. 前端框架选型

### 2.1 方案对比

| 特性                     | React                             | Vue 3                          | Svelte                      | 原生 JS         |
| ------------------------ | --------------------------------- | ------------------------------ | --------------------------- | --------------- |
| **类型系统**       | JSX + TypeScript                  | SFC + TypeScript               | Svelte TS ( runes )         | 原生 TypeScript |
| **3D 生态**        | @react-three/fiber (极丰富)       | @tresjs/core (成熟)            | 有限 (需手动集成)           | 直接 Three.js   |
| **状态管理**       | Zustand/Jotai/Redux Toolkit       | Pinia (官方推荐)               | Svelte Store (内置)         | 手动管理        |
| **UI 组件库**      | Ant Design/Material UI/Chakra UI  | Element Plus/Arco Design       | Skeleton UI/Flowbite Svelte | 无              |
| **学习曲线**       | 中等                              | 低                             | 低                          | 高              |
| **社区规模**       | 最大                              | 大 (中国社区强)                | 中等                        | 分散            |
| **代码编辑器集成** | Monaco/CodeMirror 均有 React 绑定 | Monaco/CodeMirror Vue 绑定     | 需手动封装                  | 直接使用        |
| **构建工具**       | Vite/Next.js                      | Vite (官方)                    | SvelteKit/Vite              | 不需要          |
| **包体积影响**     | 较大 (React ~40KB gzipped)        | 中等 (~33KB gzipped)           | 小 (编译后极小)             | 无额外开销      |
| **响应式数据**     | Hooks + setState                  | Composition API (ref/reactive) | Svelte reactivity (编译时)  | 手动            |
| **适合团队**       | 大型团队 / 多人协作               | 中小型团队 / 中国开发者        | 小团队 / 高性能场景         | 极小团队        |

### 2.2 推荐分析

**React (推荐)**:

- 优势: @react-three/fiber 是目前最成熟的 3D 可视化 React 封装，Foxglove Studio、Webviz 等知名机器人项目均基于 React。
- 劣势: 生态复杂度高，需要更多依赖管理。

**Vue 3 (备选)**:

- 优势: TresJS 提供了 Vue 生态的 3D 渲染支持，Element Plus 在国内使用广泛，学习成本低。
- 劣势: 机器人可视化社区案例相对较少，TresJS 生态不如 R3F 丰富。

**Svelte (轻量方案)**:

- 优势: 编译时优化，运行时极小，性能好。
- 劣势: 3D 生态和 UI 组件库选择较少。

---

## 3. 3D 渲染方案

### 3.1 方案对比

| 特性                       | Three.js                      | @react-three/fiber (R3F) | TresJS               | Babylon.js         | A-Frame           |
| -------------------------- | ----------------------------- | ------------------------ | -------------------- | ------------------ | ----------------- |
| **定位**             | 底层 3D 引擎                  | React 声明式封装         | Vue 声明式封装       | 全功能 3D 引擎     | WebXR 框架        |
| **包体积**           | ~150KB gzipped                | ~170KB (含 Three.js)     | ~170KB (含 Three.js) | ~400KB gzipped     | ~120KB gzipped    |
| **渲染性能**         | 优秀                          | 优秀 (底层同 Three.js)   | 优秀                 | 优秀               | 中等              |
| **机器人可视化生态** | 最丰富 (ros3djs, urdf-loader) | 丰富 (继承 Three.js)     | 中等                 | 中等               | 少                |
| **URDF 支持**        | urdf-loader (成熟)            | 可集成 urdf-loader       | 可集成 urdf-loader   | 社区插件           | 无                |
| **物理引擎**         | 需外部 (Rapier/Cannon)        | 需外部                   | 需外部               | 内置 Havok         | 内置              |
| **开发体验**         | 命令式 (手动管理场景)         | 声明式 (组件化)          | 声明式 (组件化)      | 命令式 + Inspector | HTML 声明式       |
| **TypeScript**       | 完整支持                      | 完整支持                 | 完整支持             | 原生 TypeScript    | 部分              |
| **PBR 材质**         | 支持                          | 支持                     | 支持                 | 优秀 (开箱即用)    | 基础              |
| **调试工具**         | Spector.js / 外部             | React DevTools           | Vue DevTools         | 内置 Inspector     | A-Frame Inspector |
| **社区**             | 最大                          | 大 (React 生态)          | 中等                 | 大 (微软支持)      | 中等              |
| **适合场景**         | 通用 3D / 底层控制            | React 项目               | Vue 项目             | 工业数字孪生       | WebXR / AR        |

### 3.2 推荐分析

**方案 A: Three.js + React Three Fiber (R3F)** -- 强烈推荐

- 工业机器人可视化的主流选择
- Foxglove Studio、Webviz 均基于 Three.js
- R3F 提供声明式开发体验，Drei 库提供大量现成组件 (OrbitControls, Grid, TransformControls 等)
- 社区活跃，URDF 支持成熟

**方案 B: Three.js + TresJS (Vue 生态)**

- 适合团队偏好 Vue 的场景
- TresJS 核心功能完备ientos 库提供扩展组件
- 社区相对 R3F 小，但发展迅速

**方案 C: Babylon.js**

- 适合需要 PBR 渲染、物理模拟的场景
- 内置 Inspector 调试器方便开发
- 机器人领域案例较少

### 3.3 关键功能实现方案

#### 3D 场景核心组件

```
Scene
├── Grid (地面网格)
├── Axes (坐标轴)
├── Robot (机器人模型)
│   ├── BaseLink
│   ├── Joint1 -> Link1
│   ├── Joint2 -> Link2
│   ├── ...
│   └── Joint6 -> Link6 -> Tool
├── CoordinateFrames (参考坐标系显示)
├── TrajectoryPath (轨迹可视化)
└── CameraControls (轨道相机控制)
```

#### 关节联动实现

使用 Three.js 的 `Object3D` 层级关系模拟运动链：

- 每个关节作为父级 Object3D
- 子链接自动继承父级变换
- 更新关节角度时只需设置 `rotation.z` (或对应轴)
- 笛卡尔空间变换需自行实现 FK/IK (当前项目使用 KDL)

---

## 4. URDF/机器人模型加载

### 4.1 方案对比

| 库                                 | 功能                      | 包体积   | 维护状态           | 适用场景       |
| ---------------------------------- | ------------------------- | -------- | ------------------ | -------------- |
| **urdf-loader**              | URDF 解析 + Three.js 渲染 | ~20KB    | 维护中 (gkjohnson) | 标准 URDF 加载 |
| **@urdf/core**               | URDF 解析 (无渲染器依赖)  | ~10KB    | 活跃               | 纯数据解析     |
| **robot-description-parser** | URDF/SDF 解析             | ~15KB    | 社区维护           | 多格式支持     |
| **自定义解析器**             | 解析项目自有 YAML 格式    | 项目定制 | 完全可控           | 适配现有模型   |

### 4.2 现有模型格式适配

rocos-viz 使用自定义 YAML 配置文件，不兼容标准 URDF。Web 端有两种策略：

**策略 A: 保持现有格式，开发自定义解析器**

```javascript
// 自定义 YAML -> Three.js Object3D 层级
async function loadRocosModel(yamlPath) {
  const config = await loadYAML(yamlPath);
  const robot = new Group();

  for (const link of config.robot) {
    const mesh = await loadMesh(link.mesh); // DAE 或 STL
    const joint = new Object3D();

    // 应用平移和旋转
    if (link.translate) joint.position.set(...link.translate);
    if (link.angleAxis) {
      joint.quaternion.setFromAxisAngle(
        new Vector3(...link.angleAxis),
        link.rotate[1] // 近似取第二分量
      );
    }

    robot.add(joint);
    joint.add(mesh);
  }
  return robot;
}
```

**策略 B: 转换为 URDF/glTF 格式**

- 在控制器端增加格式转换工具
- 输出标准 URDF + mesh 文件
- Web 端使用 `urdf-loader` 加载
- 好处是与 ROS 生态兼容，但需要修改控制器端

**推荐**: 策略 A -- 开发成本低，无需修改控制器端。可参考 `@urdf/core` 的数据结构设计，预留未来支持 URDF 的扩展接口。

### 4.3 Mesh 格式支持

| 格式          | Three.js 支持 | 说明                     |
| ------------- | ------------- | ------------------------ |
| DAE (Collada) | ColladaLoader | 当前 rocos-viz 使用      |
| STL           | STLLoader     | 当前 rocos-viz 使用      |
| glTF/GLB      | GLTFLoader    | Web 端推荐格式，加载最快 |
| OBJ           | OBJLoader     | 简单但不保留层级         |

**建议**: Web 端保留 DAE/STL 支持 (兼容现有模型)，同时支持 glTF 以获得更好的加载性能。

---

## 5. 实时数据可视化图表库

### 5.1 方案对比

| 特性                     | ECharts                         | Chart.js            | uPlot       | D3.js         | Lightweight Charts       |
| ------------------------ | ------------------------------- | ------------------- | ----------- | ------------- | ------------------------ |
| **包体积**         | ~800KB (全量) / ~300KB (按需)   | ~65KB               | ~35KB       | ~280KB        | ~45KB                    |
| **渲染方式**       | Canvas (可选 SVG/WebGL)         | Canvas              | Canvas      | SVG/Canvas    | Canvas                   |
| **实时更新性能**   | 优秀 (appendData)               | 中等 (大数据集吃力) | 极优秀      | 依赖实现      | 优秀                     |
| **图表类型**       | 60+ 种                          | 8 种基础            | 时序图专用  | 任意          | 金融图表                 |
| **交互功能**       | 丰富 (缩放/工具提示/图例)       | 中等                | 基础        | 丰富 (需手写) | 丰富                     |
| **动画效果**       | 优秀                            | 中等                | 无          | 需手写        | 无                       |
| **TypeScript**     | 完整支持                        | 完整支持            | 完整支持    | @types/d3     | 完整支持                 |
| **学习曲线**       | 中等                            | 低                  | 中等        | 高            | 低                       |
| **中国社区**       | 极强 (百度出品)                 | 中等                | 弱          | 中等          | 弱                       |
| **适合场景**       | 通用仪表板 / 复杂图表           | 简单图表            | 高性能时序  | 完全定制      | 金融/时序                |
| **React/Vue 组件** | echarts-for-react / vue-echarts | react-chartjs-2     | react-uplot | 无官方        | react/lightweight-charts |

### 5.2 推荐分析

**ECharts (推荐)**:

- 国内生态极强，文档中文友好
- 支持实时数据流 (`appendData`)
- 图表类型丰富，支持仪表盘 (gauge) 显示速度/力矩
- 可选 WebGL 渲染处理大数据量
- `echarts-for-react` / `vue-echarts` 提供框架集成

**uPlot (备选 -- 极致性能场景)**:

- 包体积最小 (~35KB)
- 专为高性能时序数据设计
- 适合需要同时显示多通道高频数据 (1kHz+) 的场景
- 但图表类型有限，需要较多手写代码

**Chart.js (轻量方案)**:

- 简单易用，适合数据量不大的场景
- 大数据集实时更新性能不理想

### 5.3 ROCOS 场景适配

当前 rocos-viz 的 PlotWidget 最多支持 2x2 布局的 4 个图表，每个图表显示多条曲线 (最多 7 关节)。Web 端需求：

- 关节位置 (7 轴 x 时间)
- 关节速度 (7 轴 x 时间)
- 关节力矩 (7 轴 x 时间)
- 关节电流 (7 轴 x 时间)
- 笛卡尔空间位置 (6DoF x 时间)

推荐 ECharts，因其同时满足: (1) 中等规模实时性能; (2) 丰富的图表类型 (折线图 + 仪表盘); (3) 完善的缩放/工具提示交互。

---

## 6. 通信方案

### 6.1 方案对比

| 特性                 | REST API 轮询     | WebSocket       | SSE (Server-Sent Events) |
| -------------------- | ----------------- | --------------- | ------------------------ |
| **方向**       | 客户端 -> 服务端  | 双向            | 服务端 -> 客户端         |
| **延迟**       | 高 (轮询间隔)     | 极低 (1-10ms)   | 低 (10-50ms)             |
| **带宽**       | 浪费 (多数空响应) | 高效            | 高效                     |
| **二进制支持** | 有限              | 原生支持        | 仅文本 (base64)          |
| **防火墙友好** | 极好              | 有时需配置      | 好 (基于 HTTP)           |
| **浏览器支持** | 全部              | 全部现代浏览器  | 全部现代浏览器           |
| **实现复杂度** | 低                | 中-高           | 低-中                    |
| **适合场景**   | 配置/标定/非实时  | 实时控制/遥操作 | 遥测数据流               |
| **现有集成**   | 当前系统已实现    | 需新增          | 需新增                   |

### 6.2 ROCOS 通信架构建议

**混合通信方案 (推荐)**:

```
浏览器 (Web UI)
    |
    ├── WebSocket (主通道)
    │   ├── 20ms 状态推送 (joint_states, pose, hardware)
    │   ├── 实时点动命令 (drag/start, drag/stop)
    │   └── 脚本执行状态推送
    |
    └── HTTP REST (辅助通道)
        ├── 连接/断开 (GET /api/robot/info)
        ├── 运动命令 (POST /api/move/*)
        ├── 标定操作 (POST /api/calibration/*)
        ├── 模型下载 (GET /api/robot/model, GET /api/robot/model/mesh)
        └── Lua 脚本管理 (POST /api/script/*)
```

**理由**:

1. 状态数据需要 20ms 级更新频率，HTTP 轮询在此频率下开销过大
2. WebSocket 支持二进制传输，可用 Protocol Buffers/MessagePack 压缩状态数据
3. 运动命令、标定等操作频率低，保持 HTTP REST 实现简单
4. 与当前系统的 HTTP API 兼容，迁移成本低

### 6.3 数据序列化格式

| 格式             | 大小 | 解析速度 | 适用               |
| ---------------- | ---- | -------- | ------------------ |
| JSON             | 大   | 中等     | HTTP REST (当前)   |
| MessagePack      | 中   | 快       | WebSocket 状态推送 |
| Protocol Buffers | 小   | 极快     | 高频状态流         |
| CBOR             | 中   | 快       | 资源受限场景       |

**建议**: HTTP 通道保持 JSON (兼容现有 API)，WebSocket 通道使用 MessagePack (平衡性能与可调试性)。

---

## 7. UI 组件库

### 7.1 方案对比

| 特性                 | Ant Design     | Element Plus | Material UI (MUI) | Chakra UI        | Arco Design |
| -------------------- | -------------- | ------------ | ----------------- | ---------------- | ----------- |
| **基础框架**   | React          | Vue 3        | React             | React            | React / Vue |
| **包体积**     | ~800KB         | ~500KB       | ~900KB            | ~400KB           | ~700KB      |
| **组件数量**   | 60+            | 70+          | 50+               | 40+              | 60+         |
| **设计语言**   | Ant Design 5.0 | 自有         | Material Design   | 自有             | 自有        |
| **暗色主题**   | 支持           | 支持         | 支持              | 支持 (默认)      | 支持        |
| **响应式**     | 支持           | 支持         | 支持              | 支持 (Flex/Grid) | 支持        |
| **TypeScript** | 完整           | 完整         | 完整              | 完整             | 完整        |
| **国际化**     | 完善           | 完善         | 完善              | 中等             | 完善        |
| **中文文档**   | 优秀           | 优秀         | 一般              | 一般             | 优秀        |
| **适合场景**   | 企业级应用     | 通用         | 通用              | 现代 Web 应用    | 企业级应用  |

### 7.2 推荐分析

**React 项目**: Ant Design 5.0 或 Arco Design

- Ant Design: 国内生态强，表格/表单组件完善，适合复杂管理界面
- Arco Design: 字节跳动出品，设计更现代，组件质量高

**Vue 项目**: Element Plus

- Vue 生态最成熟的组件库
- 中文文档优秀，国内社区活跃

---

## 8. 代码编辑器集成

### 8.1 方案对比

| 特性                     | Monaco Editor              | CodeMirror 6            | Ace Editor          |
| ------------------------ | -------------------------- | ----------------------- | ------------------- |
| **来源**           | VS Code 内核               | 独立项目                | Cloud9 IDE          |
| **包体积**         | ~2-5MB                     | ~150KB                  | ~500KB              |
| **Lua 语法高亮**   | 需自定义                   | 需自定义                | 内置 Lua 模式       |
| **自动补全**       | 强大 (自定义 Intellisense) | 可扩展                  | 支持 (自定义)       |
| **性能**           | 好 (但体积大)              | 极好 (轻量)             | 好                  |
| **主题**           | VS Code 主题系统           | 自定义                  | 60+ 内置主题        |
| **minimap**        | 支持                       | 插件支持                | 支持                |
| **搜索替换**       | 强大                       | 强大                    | 强大                |
| **React/Vue 组件** | @monaco-editor/react       | @codemirror/view (原生) | react-ace / vue-ace |
| **TypeScript**     | 原生支持                   | 完整                    | 部分                |
| **移动端支持**     | 差                         | 较好                    | 好                  |

### 8.2 推荐分析

**CodeMirror 6 (推荐)**:

- 包体积最小 (~150KB)，对 Web 应用的加载性能影响小
- 性能极好，滚动/编辑流畅
- 可扩展性强，社区有 Lua 语法高亮定义
- 支持移动端编辑
- 适合 Lua 脚本编辑这种单一语言场景

**Monaco Editor (备选 -- 如需 VS Code 体验)**:

- 功能最强大，Intellisense 体验最佳
- 但包体积过大 (~2-5MB)，对整体应用性能影响大
- 如果仅需要基础编辑功能，Monaco 的功能溢出

**Ace Editor**:

- 折中方案，功能完善，包体积适中
- 内置 Lua 语法高亮 (无需额外配置)
- 长期维护，社区稳定

### 8.3 Lua 语法支持

三个编辑器都需要配置 Lua 语法高亮 (Ace 除外，已内置)。

CodeMirror 6 Lua 语法定义:

```javascript
import { StreamLanguage } from "@codemirror/language";
import { lua } from "@codemirror/legacy-modes/mode/lua";

const luaLanguage = StreamLanguage.define(lua);
```

---

## 9. 桌面化方案

### 9.1 方案对比

| 特性                    | Tauri v2                                | Electron                |
| ----------------------- | --------------------------------------- | ----------------------- |
| **后端语言**      | Rust                                    | Node.js                 |
| **Webview**       | 系统原生 (WebView2/WebKitGTK/WKWebView) | 捆绑 Chromium           |
| **包体积**        | 2-10 MB                                 | 150-200+ MB             |
| **内存占用**      | 低 (~30-50MB)                           | 高 (~150-300MB)         |
| **启动时间**      | 快 (< 1s)                               | 慢 (2-5s)               |
| **安全性**        | Rust 后端 + 细粒度权限                  | 完整 Node.js 权限       |
| **插件生态**      | 成长中 (v2 改善)                        | 成熟 (npm 生态)         |
| **开发者体验**    | Rust + JS/TS (前后端分离)               | 全 JS/TS                |
| **跨平台**        | Windows / macOS / Linux                 | Windows / macOS / Linux |
| **自动更新**      | 内置支持                                | electron-updater        |
| **系统 API 访问** | Rust 原生 (高性能)                      | Node.js API             |
| **学习曲线**      | 中等 (需了解 Rust 基础)                 | 低                      |
| **代表项目**      | Padloc, Flowcharts, Clawd               | VS Code, Slack, Discord |

### 9.2 推荐分析

**Tauri v2 (推荐)**:

- 包体积优势明显 (2-10MB vs 150-200MB)，对工业控制器应用很关键
- 内存占用低，适合嵌入式/工业设备
- Rust 后端性能好，适合处理高速数据流
- 安全性更好 (无完整 Node.js 权限)

**Electron (备选 -- 快速原型)**:

- 开发速度快，全 JS/TS 技术栈统一
- 如果团队不熟悉 Rust，Electron 的开发效率更高
- 包体积和内存是明显劣势

### 9.3 Web vs Desktop 策略

**建议**: 先实现纯 Web 版本 (浏览器直接访问)，再通过 Tauri 打包为桌面应用。

```
rocos-viz-web/
├── packages/
│   ├── core/          # 通信层、模型解析、状态管理 (框架无关)
│   ├── ui/            # React/Vue UI 组件
│   ├── 3d-viewer/     # Three.js 3D 渲染组件
│   └── desktop/       # Tauri 打包配置
├── vite.config.ts
├── package.json
└── tsconfig.json
```

---

## 10. 现有开源机器人 Web 控制器参考项目

### 10.1 项目对比

| 项目                               | 技术栈                        | 功能                                      | 许可证  | 适合参考                    |
| ---------------------------------- | ----------------------------- | ----------------------------------------- | ------- | --------------------------- |
| **Foxglove Studio**          | React + Three.js + TypeScript | 3D 可视化、点云、图像、TF、Plot、扩展系统 | MPL-2.0 | 3D 渲染、面板架构、扩展系统 |
| **Webviz** (Cruise)          | React + Three.js + WebGL      | ROS bag 可视化、3D 点云、TF               | MIT     | 3D 渲染性能优化             |
| **roslibjs**                 | 原生 JS                       | ROS 消息通信 (via rosbridge WebSocket)    | BSD     | WebSocket 通信模式          |
| **rosboard**                 | 原生 JS                       | 轻量 ROS2 仪表板                          | MIT     | 简洁 UI 设计                |
| **rqt_robot_monitor** (ROS)  | Python/Qt                     | 机器人状态监控                            | BSD     | 功能设计参考                |
| **jupyter-robot-dashboards** | Jupyter + ipywidgets          | Jupyter 内的机器人控制                    | BSD     | 快速原型验证                |

### 10.2 Foxglove Studio 架构分析 (重点参考)

Foxglove Studio 是当前最成熟的 Web 端机器人可视化工具，其架构值得深入参考：

```
Foxglove Studio Architecture
├── 面板系统 (Panel Architecture)
│   ├── 3D Panel (Three.js)
│   ├── Plot Panel (regl, GPU 加速)
│   ├── Image Panel
│   ├── Map Panel
│   ├── Diagnostic Panel
│   └── 自定义扩展面板 (Plugin API)
├── 数据层 (Data Pipeline)
│   ├── Topic 订阅管理
│   ├── WebSocket 数据流
│   ├── MCAP/ROS bag 文件读取
│   └── 延迟渲染 (Lazy Rendering)
├── 状态管理
│   ├── Panel 布局状态
│   ├── Topic 选择状态
│   └── 播放控制状态
└── 扩展系统
    ├── 面板插件 API
    ├── Message Path 系统
    └── 第三方扩展支持
```

**可借鉴的设计**:

1. **面板系统**: 模块化面板，支持拖拽布局 -- 3D 视图、图表、脚本编辑器各为独立面板
2. **Topic 订阅模式**: 数据按需订阅，减少不必要的处理
3. **延迟渲染**: 大数据量时按需渲染，保持 UI 流畅
4. **消息路径系统**: 通用的 JSON/Protobuf 消息路径表达式

### 10.3 参考项目链接

- Foxglove Studio: https://github.com/foxglove/studio
- Webviz: https://github.com/cruise-automation/webviz
- roslibjs: https://github.com/RobotWebTools/roslibjs
- rosboard: https://github.com/rosboard/rosboard
- urdf-loader: https://github.com/gkjohnson/urdf-loader
- @react-three/fiber: https://github.com/pmndrs/react-three-fiber
- TresJS: https://github.com/Tresjs/tres
- ECharts: https://github.com/apache/echarts
- uPlot: https://github.com/leeoniya/uPlot
- CodeMirror: https://github.com/codemirror/dev
- Tauri: https://github.com/tauri-apps/tauri

---

## 11. 推荐技术栈方案

### 方案 A: React + R3F + ECharts (推荐 -- 功能完备型)

| 层面                |                                                   技术选择 | 理由                                   |
| ------------------- | ---------------------------------------------------------: | -------------------------------------- |
| **框架**      |                                      React 18 + TypeScript | 3D 生态最丰富，社区案例最多            |
| **构建工具**  |                                                     Vite 5 | 快速构建，HMR 优秀                     |
| **3D 渲染**   |          Three.js + @react-three/fiber + @react-three/drei | 声明式 3D 开发，丰富的机器人可视化组件 |
| **URDF/模型** |              自定义解析器 (适配现有 YAML) + DAE/STL 加载器 | 无需修改控制器端                       |
| **图表**      |                              ECharts 5 + echarts-for-react | 国内生态强，实时性能好                 |
| **UI 组件**   |                                             Ant Design 5.0 | 企业级组件库，表格/表单完善            |
| **代码编辑**  | @monaco-editor/react (功能优先) 或 CodeMirror 6 (体积优先) | Lua 脚本编辑                           |
| **通信**      |                 WebSocket (状态流) + HTTP/JSON REST (命令) | 混合方案                               |
| **状态管理**  |                                                    Zustand | 轻量、简洁，适合全局状态               |
| **桌面化**    |                                                   Tauri v2 | 轻量、安全、高性能                     |
| **测试**      |                                        Vitest + Playwright | 单元测试 + E2E 测试                    |

**优点**:

- 生态最成熟，Foxglove Studio 等项目验证过可行性
- React Three Fiber 提供最完整的 3D 组件化开发体验
- ECharts 国内生态好，文档中文友好
- 大量可参考的开源项目和社区资源

**缺点**:

- 依赖数量较多，初始包体积偏大 (可通过 code splitting 和 lazy loading 优化)
- React 学习曲线比 Vue/Svelte 略高
- Monaco Editor 体积大 (如选用)

**预估包体积**:

- 开发模式: ~3-5MB
- 生产构建 (gzipped): ~300-500KB (不含 3D 引擎和编辑器)
- 含 Three.js: ~500KB-1MB gzipped

---

### 方案 B: Vue 3 + TresJS + ECharts (备选 -- Vue 生态型)

| 层面                | 技术选择                                  | 理由                       |
| ------------------- | ----------------------------------------- | -------------------------- |
| **框架**      | Vue 3 (Composition API) + TypeScript      | 学习成本低，响应式系统优秀 |
| **构建工具**  | Vite 5                                    | Vue 官方推荐               |
| **3D 渲染**   | Three.js + @tresjs/core + @tresjs/cientos | Vue 声明式 3D 开发         |
| **URDF/模型** | 自定义解析器 + Three.js 加载器            | 同方案 A                   |
| **图表**      | ECharts 5 + vue-echarts                   | 同方案 A                   |
| **UI 组件**   | Element Plus                              | Vue 生态最成熟组件库       |
| **代码编辑**  | CodeMirror 6 (通过 Vue 封装)              | 轻量级编辑器               |
| **通信**      | WebSocket + HTTP REST                     | 同方案 A                   |
| **状态管理**  | Pinia                                     | Vue 官方推荐               |
| **桌面化**    | Tauri v2                                  | 同方案 A                   |
| **测试**      | Vitest + Playwright                       | 同方案 A                   |

**优点**:

- Vue 3 Composition API 开发体验优秀
- Element Plus 组件库中文文档好，国内开发者熟悉
- Pinia 状态管理简洁
- 如果团队已有 Vue 经验，开发效率高

**缺点**:

- TresJS 生态不如 R3F 丰富，部分功能需自行实现
- 机器人可视化领域的 Vue 案例较少
- TresJS 可能存在未覆盖的边界情况

---

### 方案 C: Svelte + Three.js + uPlot (轻量 -- 极致性能型)

| 层面                | 技术选择                      | 理由                   |
| ------------------- | ----------------------------- | ---------------------- |
| **框架**      | Svelte 5 + TypeScript         | 编译时优化，运行时极小 |
| **构建工具**  | SvelteKit / Vite              | 同上                   |
| **3D 渲染**   | Three.js (原生集成，无封装层) | 无额外框架开销         |
| **URDF/模型** | 自定义解析器                  | 同方案 A               |
| **图表**      | uPlot (极致性能) 或 ECharts   | uPlot 体积小性能高     |
| **UI 组件**   | Skeleton UI / 自定义          | 轻量级组件             |
| **代码编辑**  | CodeMirror 6                  | 轻量级编辑器           |
| **通信**      | WebSocket + HTTP REST         | 同方案 A               |
| **状态管理**  | Svelte Store (内置)           | 无需额外依赖           |
| **桌面化**    | Tauri v2                      | 同方案 A               |
| **测试**      | Vitest + Playwright           | 同方案 A               |

**优点**:

- 运行时最小，性能最优
- 编译时优化，无 Virtual DOM 开销
- 适合对性能要求极高的场景

**缺点**:

- 3D 生态不成熟，大量组件需自行封装
- UI 组件库选择有限
- 社区规模较小，遇到问题可参考的资源少
- 团队需要额外学习 Svelte

---

## 12. 方案对比总结

| 维度               | 方案 A (React + R3F) | 方案 B (Vue 3 + TresJS) | 方案 C (Svelte + Three.js) |
| ------------------ | -------------------- | ----------------------- | -------------------------- |
| **3D 生态**  | 极强                 | 强                      | 弱                         |
| **学习曲线** | 中等                 | 低                      | 中等                       |
| **开发效率** | 高 (生态完善)        | 高 (团队熟悉度)         | 中等                       |
| **运行性能** | 好                   | 好                      | 极好                       |
| **包体积**   | 较大                 | 中等                    | 最小                       |
| **社区案例** | 极多                 | 多                      | 少                         |
| **长期维护** | 最好                 | 好                      | 中等                       |
| **团队适配** | 需 React 经验        | 国内团队友好            | 需 Svelte 经验             |
| **迁移难度** | 中等 (从 Qt 迁移)    | 较低 (概念相似)         | 较高                       |
| **推荐度**   | 强烈推荐             | 推荐                    | 谨慎推荐                   |

---

## 13. 实施路径建议

### Phase 1: 基础框架 (2-3 周)

1. 初始化 Vite + React/Vue 项目
2. 搭建通信层 (WebSocket + HTTP REST 客户端)
3. 实现基本 UI 布局 (响应式框架)
4. 实现连接管理面板

### Phase 2: 3D 渲染 (2-3 周)

1. 集成 Three.js + R3F/TresJS
2. 开发 YAML 模型解析器
3. 实现机器人模型渲染 (关节联动)
4. 实现相机控制和坐标轴显示

### Phase 3: 控制面板 (2-3 周)

1. 关节点动控制 UI (滑块/输入框)
2. 笛卡尔空间点动控制 UI
3. 速度滑块和工作模式切换
4. MoveJ/MoveL 命令面板

### Phase 4: 数据图表 (1-2 周)

1. 集成 ECharts
2. 实现关节位置/速度/力矩实时图表
3. 实现图表配置面板

### Phase 5: 脚本编辑器 (1 周)

1. 集成 CodeMirror 6 / Monaco Editor
2. 配置 Lua 语法高亮
3. 实现运行/暂停/停止控制

### Phase 6: 优化与桌面化 (1-2 周)

1. 性能优化 (大数据量渲染、通信压缩)
2. Tauri v2 打包
3. 响应式适配 (移动端/平板)

---

## 14. 附录

### 14.1 ROCOS API 端点汇总 (当前)

基于 `config/rocos-API.yaml` 的主要接口：

| 方法 | 端点                  | 说明                     |
| ---- | --------------------- | ------------------------ |
| GET  | /api/robot/info       | 获取机器人信息           |
| GET  | /api/robot/state      | 获取实时状态 (20ms 轮询) |
| GET  | /api/robot/enabled    | 查询上电状态             |
| GET  | /api/robot/model      | 获取模型结构             |
| GET  | /api/robot/model/mesh | 下载 mesh 文件           |
| POST | /api/robot/enable     | 上电                     |
| POST | /api/robot/disable    | 下电                     |
| POST | /api/robot/workmode   | 设置工作模式             |
| POST | /api/drag/start       | 开始点动                 |
| POST | /api/drag/stop        | 停止点动                 |
| POST | /api/move/joint       | 关节空间运动             |
| POST | /api/move/joint_ik    | 笛卡尔空间运动 (IK)      |
| POST | /api/move/linear      | 直线运动                 |
| POST | /api/move/linear_fk   | 直线运动 (FK)            |
| POST | /api/move/stop        | 停止运动                 |
| GET  | /api/move/status      | 查询运动状态             |
| POST | /api/script/run       | 运行 Lua 脚本            |
| POST | /api/script/stop      | 停止脚本                 |
| POST | /api/script/pause     | 暂停脚本                 |
| POST | /api/script/continue  | 继续脚本                 |

### 14.2 关键参考链接

- Three.js 官方文档: https://threejs.org/docs/
- @react-three/fiber: https://r3f.docs.pmnd.rs/
- @tresjs/core: https://tresjs.org/
- ECharts: https://echarts.apache.org/zh/index.html
- uPlot: https://github.com/leeoniya/uPlot
- CodeMirror 6: https://codemirror.net/
- Tauri: https://tauri.app/
- Foxglove Studio: https://foxglove.dev/
- urdf-loader: https://github.com/gkjohnson/urdf-loader
- Webviz: https://github.com/cruise-automation/webviz
