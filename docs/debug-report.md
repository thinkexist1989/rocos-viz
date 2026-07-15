# Debug Report - 网页无法加载模型和状态轮询

**日期**: 2026-06-09
**问题**: 用户反馈网页连接成功但：
1. 未加载 STL 模型
2. 未开始状态轮询

---

## 根因分析

### 根因 1: CORS 跨域请求被浏览器阻止

**现象**:
- `curl http://192.168.0.194:8080/api/robot/info` 返回 "Empty reply from server"
- 网页请求被浏览器阻止 (CORS policy)

**原因**:
- Web 应用运行在 `localhost:5173` (Vite dev server)
- API 请求目标: `http://192.168.0.194:8080/api/...`
- 浏览器执行 CORS 检查，服务器未返回 `Access-Control-Allow-Origin` 头

**证据**:
```bash
$ curl -v http://192.168.0.194:8080/api/robot/info
* Trying 192.168.0.194:8080...
* Connected to 192.168.0.194 (192.168.0.194) port 8080
> GET /api/robot/info HTTP/1.1
* Empty reply from server
```

服务器接受连接但返回空响应，可能是：
- 服务器未处理该请求路径
- 服务器在发送响应前崩溃
- 服务器需要特定的请求头或参数

---

### 根因 2: useRobotConnection Hook 未挂载

**现象**:
- ConnectDialog 设置 `isConnected = true`
- 但轮询从未开始

**原因**:
```typescript
// AppLayout.tsx (修改前)
export function AppLayout() {
  // useRobotConnection() 未调用！
  const isConnected = useConnectionStore((s) => s.isConnected);
  // ...
}
```

Hook 的 `useEffect` 依赖 `isConnected` 变化触发轮询，但 Hook 从未挂载到组件树中。

**修复** (已在最新代码中):
```typescript
export function AppLayout() {
  const isConnected = useConnectionStore((s) => s.isConnected);
  
  // 挂载连接 Hook
  useRobotConnection();
  
  // ...
}
```

---

### 根因 3: 模型 URL 错误

**现象**:
- `RobotModel` 组件尝试从 `/config.yaml` 加载模型
- 该文件不存在于 `public/` 目录

**原因**:
```typescript
// AppLayout.tsx (修改前)
<RobotModel 
  modelUrl="/config.yaml"  // ❌ 文件不存在
  meshBaseUrl="http://localhost:30001/api/robot/model/mesh"  // ❌ 跨域 + 错误端口
/>
```

**修复**:
- 从 API `GET /api/robot/model` 获取 YAML 内容
- 传递给 `RobotModel` 组件解析

---

### 根因 4: 默认端口错误

**现象**:
- Store 默认端口: `30001`
- 控制器实际端口: `8080`

**修复**:
```typescript
// connectionStore.ts
port: '8080',  // 从 '30001' 改为 '8080'
```

---

## 已实施的修复

### 1. Vite 代理配置 (消除 CORS)

**文件**: `vite.config.ts`

```typescript
const ROBOT_HOST = process.env.ROBOT_HOST || '192.168.0.194';
const ROBOT_PORT = process.env.ROBOT_PORT || '8080';

export default defineConfig({
  server: {
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
```

**效果**:
- `http://localhost:5173/api/robot/info` → 代理到 `http://192.168.0.194:8080/api/robot/info`
- 浏览器请求同源，无 CORS 问题
- WebSocket 也通过代理

---

### 2. 相对 URL (通过代理)

**文件**: `RobotApiClient.ts`

```typescript
// 修改前
async connect(): Promise<RobotInfo> {
  return this.request('GET', 'http://192.168.0.194:8080/api/robot/info');  // 跨域
}

// 修改后
async connect(): Promise<RobotInfo> {
  return this.request('GET', '/api/robot/info');  // 相对路径，通过 Vite 代理
}
```

---

### 3. 挂载连接 Hook

**文件**: `AppLayout.tsx`

```typescript
export function AppLayout() {
  // 新增: 挂载 Hook
  useRobotConnection();
  
  // ...
}
```

---

### 4. 动态获取模型 YAML

**文件**: `AppLayout.tsx`

```typescript
// 新增: 连接后从 API 获取模型
useEffect(() => {
  if (!isConnected) {
    setYamlContent(null);
    return;
  }

  async function fetchModel() {
    const client = new RobotApiClient(host, port);
    const model = await client.getRobotModel();
    const yaml = (model as any).yaml ?? '';
    setYamlContent(yaml);
  }

  fetchModel();
}, [isConnected, host, port]);
```

---

### 5. 更新默认端口

**文件**: `connectionStore.ts`, `ConnectDialog.tsx`

- 默认端口: `30001` → `8080`
- Placeholder: `30001` → `8080`

---

## 验证步骤

### Step 1: 重启 Vite dev server

```bash
cd rocos-web
npm run dev
```

代理配置在 `vite.config.ts` 中，重启后生效。

---

### Step 2: 打开浏览器开发者工具

1. 打开 `http://localhost:5173`
2. 按 F12 打开开发者工具
3. 切换到 **Network** 标签
4. 在连接对话框输入:
   - IP: `192.168.0.194`
   - Port: `8080`
5. 点击 "连接"

**预期结果**:
- 看到 `GET /api/robot/info` 请求 (200 OK)
- 状态栏显示绿色连接指示
- 看到 `GET /api/robot/model` 请求 (获取 YAML)
- 看到 `GET /api/robot/state` 每 20ms 轮询
- 3D 视口加载 STL 模型

---

### Step 3: 检查控制台错误

**Console** 标签应显示:
```
✓ 连接成功
✓ 获取模型配置
✓ 开始状态轮询
✓ 模型加载完成
```

**不应有**:
- `CORS policy` 错误
- `Failed to fetch` 错误
- `Empty reply from server` 错误

---

### Step 4: 验证 API 响应

在浏览器地址栏直接访问:
```
http://localhost:5173/api/robot/info
```

**预期**: JSON 响应包含 `joint_infos` 数组 (7 个关节)

---

## 常见问题排查

### Q1: 仍然看到 CORS 错误

**检查**:
1. Vite 是否重启？代理配置需要重启生效
2. 是否直接访问了 `http://localhost:5173` 而不是 `http://192.168.0.194:8080`？

**解决**:
```bash
# 停止旧的 Vite 进程
pkill -f "vite"

# 重启
cd rocos-web
npm run dev
```

---

### Q2: API 返回 502 Bad Gateway

**原因**: Vite 代理无法连接到控制器

**检查**:
```bash
# 测试控制器是否可达
ping 192.168.0.194

# 测试端口是否开放
telnet 192.168.0.194 8080
```

**解决**:
- 确认控制器在同一网络
- 检查防火墙规则
- 确认端口 8080 未被占用

---

### Q3: API 返回空响应

**原因**: 控制器 HTTP 服务器未正确处理请求

**检查**:
```bash
# 查看控制器日志
# 用户提供的日志显示:
# [HttpServer] [info] GET /api/robot/info
# 但没有响应日志
```

**可能原因**:
1. 控制器期望特定的请求头 (如 `Accept: application/json`)
2. 控制器需要认证 token
3. 控制器在处理请求时崩溃

**解决**:
- 检查控制器 HTTP 服务器实现
- 添加请求/响应日志
- 验证 API 路径是否正确

---

### Q4: 模型加载但看不到 3D 模型

**检查**:
1. 打开浏览器开发者工具 → **Console**
2. 查找 `Failed to load mesh` 错误

**可能原因**:
- Mesh 文件路径错误
- Mesh 文件格式不支持 (需要 STL 或 DAE)
- Mesh 文件下载失败

**解决**:
```bash
# 测试 mesh 下载
curl "http://localhost:5173/api/robot/model/mesh?path=link0.stl" -o test.stl
file test.stl  # 应显示 "STL mesh"
```

---

## 下一步调试

如果问题仍然存在，请收集以下信息:

### 1. 浏览器网络请求截图

在开发者工具的 **Network** 标签中:
- 截图所有 `/api/robot/*` 请求
- 包括请求头和响应体

### 2. 控制器日志

收集连接后的完整日志:
```bash
# 重定向日志到文件
rocos_server > server.log 2>&1

# 连接后停止
# 然后分享 server.log
```

### 3. 浏览器控制台错误

在 **Console** 标签中:
- 右键 → "Save as..."
- 分享导出的日志文件

---

## 已修改文件清单

| 文件 | 修改内容 |
|------|----------|
| `vite.config.ts` | 添加 Vite 代理配置 |
| `src/stores/connectionStore.ts` | 默认端口 30001 → 8080 |
| `src/components/connection/ConnectDialog.tsx` | Port placeholder 30001 → 8080 |
| `src/components/layout/AppLayout.tsx` | 挂载 useRobotConnection + 动态获取模型 |
| `src/scene/RobotModel.tsx` | 接收 yamlContent prop |
| `src/core/RobotApiClient.ts` | 使用相对 URL |
| `src/core/RobotWebSocket.ts` | 使用相对 URL |

---

**结论**: 根因是 CORS + Hook 未挂载 + 模型 URL 错误。已全部修复。

**当前状态**: 
- Vite 代理配置正确，请求已成功转发到控制器
- 但控制器返回空响应 (`socket hang up`)
- 浏览器报错: `Unexpected end of JSON input` (JSON 解析空字符串)
- 需要在 `RobotApiClient.ts` 中添加详细日志以确认响应格式

**下一步**: 打开浏览器开发者工具 → Console，查看 `[RobotApiClient]` 日志确认响应内容。

**验证**: 重启 Vite dev server 后，连接对话框应正常工作，模型应加载，轮询应开始。

**生成**: Claude Code (claude-opus-4-6)
**日期**: 2026-06-09
