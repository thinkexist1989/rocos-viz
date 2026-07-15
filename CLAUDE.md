# CLAUDE.md

本文件为 Claude Code (claude.ai/code) 在此仓库中工作时提供指导。

## 常用命令

- `npm run dev` — 启动 Vite 开发服务器（自动打开浏览器，绑定 `0.0.0.0`）。
- `npm run build` — 对整个项目进行类型检查（`tsc -b`），然后构建生产版本（`vite build`）。
- `npm run preview` — 本地预览生产构建。

目前没有配置测试运行器，也没有配置 `lint` 脚本，尽管已安装了 ESLint 和 `@typescript-eslint`。手动运行 lint：`npx eslint src`。类型检查通过构建命令完成（`tsc -b`）。

### 连接机器人后端

开发服务器代理机器人控制器，因此前端始终与同源的 `/api` 和 `/ws` 路径通信。通过环境变量指定控制器地址（默认值如下）：

```bash
ROBOT_HOST=127.0.0.1 ROBOT_PORT=8080 VITE_PORT=5173 npm run dev
```

在 `vite.config.ts` 中，`/api` 代理到 `http://$ROBOT_HOST:$ROBOT_PORT`，`/ws` 代理到 `ws://$ROBOT_HOST:$ROBOT_PORT`（已启用 `changeOrigin`/`ws`）。由于请求是同源且经过代理的，`RobotApiClient` 基于 `window.location.origin` 构建 URL，`RobotWebSocket` 基于 `window.location.host` 构建地址——它们不会直接连接机器人主机。`@` 导入别名映射到 `./src`。

## 架构

这是一个 React 18 + TypeScript + Vite 单页应用，用于实时可视化和控制机器人手臂。整体架构分为三层：

### 1. `src/core/` — 后端通信和机器人数学运算（框架无关，不依赖 React）

- **`RobotApiClient.ts`** — 所有命令/查询接口的 REST 客户端（`/api/robot/*`、`/api/move/*`、`/api/drag/*`、`/api/script/*`、`/api/calibration/*`、`/api/robot/model`）。每次调用都通过私有的 `request()` 方法，该方法解包 `ApiResponse<T>` 响应体（`{ success, code, message, data }`），并在 `success === false` 时抛出异常。运动指令（`moveJ`、`moveL`、`moveJ_IK`、`moveL_FK`）默认 `asynchronous: true`，并返回一个 `task_id`，可通过 `getMoveStatus` 轮询。
- **`RobotWebSocket.ts`** — 通过 `/ws/robot_state` 推送实时 `RobotState`，支持自动重连（指数退避，有最大重试上限）和 ping 心跳。
- **状态映射存在重复代码**：`RobotApiClient.ts` 和 `RobotWebSocket.ts` 各自包含一份相同的 `mapState()` 和 `mapJointStatus()`。后端发送的字段名不一致（`flange` vs `flange_pose`、`tool` vs `tool_pose`、`object` vs `object_pose`、`hw_state` vs `hardware`、`load` vs `load_torque`），关节状态用数字表示（0=DISABLED, 1/2=ENABLED, 3=FAULT）。这些映射函数负责统一字段名。**如果需要修改 `RobotState` 的结构，请同步更新两份拷贝。**
- `ForwardKinematics.ts`、`YamlModelParser.ts`、`ModelDownloader.ts` 处理运动学链和加载机器人模型定义（YAML 配置 + 通过 `/api/robot/model/mesh` 下载的网格文件）。
- `types.ts` 是所有模块共享的类型定义；`constants.ts` 包含关键常量：`MAX_JOINTS = 7`、`POLLING_INTERVAL_MS = 20`、`MAX_TRAJECTORY_POINTS = 500`、jog 的 `DIRECTION`/`FRAME`/`FREEDOM` 枚举，以及 `WORK_MODES`（其标签为中文）。

**模型加载流程（容易忽略）**：控制器通过 `getRobotModel()` 以 JSON 格式返回模型，然后 `AppLayout` 将其转换为 YAML（通过 `modelConfigToYaml()`，匹配 C++ 的 `writeModelFiles` 格式），再传入 `YamlModelParser` 和 `RobotModel` 场景组件。随后网格文件通过 `/api/robot/model/mesh` 延迟加载。

### 2. `src/stores/` — Zustand stores 是 UI 的单一数据源

状态按关注点分离：`connectionStore`、`robotStateStore`、`controlStore`、`modelStore`、`plotStore`、`uiStore`。`src/hooks/` （`useRobotConnection`、`useRobotState`、`usePlotDataCollector`）将 `core/` 客户端连接到这些 stores 和组件生命周期——组件通过读写 stores 来操作数据，而不是直接调用 `core/`。

### 3. `src/scene/` 和 `src/components/` — 渲染层

- **`src/scene/`** 使用 Three.js 通过 `@react-three/fiber` / `@react-three/drei` 进行 3D 渲染（`RobotViewer` 是 canvas 根组件；`RobotModel`/`LinkComponent` 基于解析的模型构建机械臂；还包括坐标轴、相机预设、轨迹线、辅助元素）。
- **`src/components/`** 是基于 antd 的 2D UI，按功能分组（`control/` 点动和运动指令、`plot/` ECharts 实时图表、`script/` CodeMirror 编辑器、`connection/`、`model/`、`layout/`、`common/`）。`App.tsx` 将所有内容包裹在 antd 的 `ConfigProvider`（locale `zh_CN`，自定义 `theme`）→ `AppLayout` 中。

### UI 语言

应用本地化为中文（`antd/locale/zh_CN`）；面向用户的字符串均为中文。添加 UI 文本时请保持一致。
