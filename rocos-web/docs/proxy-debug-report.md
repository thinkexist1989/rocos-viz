# 代理调试报告 - Vite Proxy 配置验证

**日期**: 2026-06-09
**状态**: ⚠️ 代理配置已生效，但控制器返回空响应

---

## 验证结果

### ✅ 代理配置正确

Vite 代理已成功拦截并转发请求:

```bash
$ curl http://localhost:5173/api/robot/info
# (空响应，但请求已到达控制器)
```

**证据** (Vite 日志):
```
7:37:57 PM [vite] http proxy error: /api/robot/info
Error: socket hang up
    at Socket.socketOnEnd (node:_http_client:542:25)
```

---

### ⚠️ 控制器返回空响应

**现象**:
- Vite 成功代理请求到 `http://192.168.0.194:8080`
- 控制器收到请求 (用户日志显示 `GET /api/robot/info`)
- 但控制器返回空响应 (`socket hang up`)

**可能原因**:

1. **控制器 HTTP 服务器实现问题**
   - 控制器可能期望特定的请求头 (如 `Accept: application/json`)
   - 控制器可能需要 POST 而不是 GET
   - 控制器可能在发送响应前崩溃

2. **响应格式不匹配**
   - `RobotApiClient` 期望: `{ success: true, data: {...} }`
   - 控制器可能返回: `{ joint_infos: [...] }` (无 `success` 包装)

3. **连接超时**
   - 控制器处理请求时间过长
   - 默认超时 5 秒可能不够

---

## 建议的解决方案

### 方案 1: 检查控制器 HTTP 服务器实现

确认控制器期望的:
- HTTP 方法 (GET/POST)
- 请求头
- 响应格式

```bash
# 使用 curl 测试不同方法
curl -X GET http://192.168.0.194:8080/api/robot/info
curl -X POST http://192.168.0.194:8080/api/robot/info
curl -H "Accept: application/json" http://192.168.0.194:8080/api/robot/info
```

---

### 方案 2: 修改 RobotApiClient 适配响应格式

如果控制器返回格式不同，修改解析逻辑:

```typescript
// RobotApiClient.ts
private async request<T>(method: string, path: string, body?: any, params?: Record<string, string>): Promise<T> {
  const url = new URL(path, window.location.origin);
  // ...

  const response = await fetch(url.toString(), options);
  
  // 尝试多种响应格式
  const json = await response.json();
  
  // 格式 1: { success: true, data: {...} }
  if (json.success !== undefined) {
    if (!json.success) {
      throw new Error(`API Error: ${json.message} (code: ${json.code})`);
    }
    return json.data;
  }
  
  // 格式 2: 直接返回数据
  return json as T;
}
```

---

### 方案 3: 增加超时时间

```typescript
// RobotApiClient.ts
private async request<T>(method: string, path: string, body?: any, params?: Record<string, string>): Promise<T> {
  const url = new URL(path, window.location.origin);
  
  const controller = new AbortController();
  const timeoutId = setTimeout(() => controller.abort(), 10000); // 10 秒
  
  const options: RequestInit = {
    method,
    signal: controller.signal,
    // ...
  };
  
  try {
    const response = await fetch(url.toString(), options);
    clearTimeout(timeoutId);
    // ...
  } catch (error) {
    clearTimeout(timeoutId);
    if (error.name === 'AbortError') {
      throw new Error('Request timeout');
    }
    throw error;
  }
}
```

---

### 方案 4: 添加详细日志

在 `RobotApiClient` 中添加日志:

```typescript
private async request<T>(method: string, path: string, body?: any, params?: Record<string, string>): Promise<T> {
  const url = new URL(path, window.location.origin);
  console.log(`[API] ${method} ${url.toString()}`);
  
  const response = await fetch(url.toString(), options);
  console.log(`[API] Response status: ${response.status}`);
  console.log(`[API] Response headers:`, Object.fromEntries(response.headers.entries()));
  
  const text = await response.text();
  console.log(`[API] Response body:`, text);
  
  if (!text) {
    throw new Error('Empty response from server');
  }
  
  const json = JSON.parse(text);
  // ...
}
```

---

## 验证步骤

### Step 1: 手动测试控制器

```bash
# 测试基本信息
curl -v http://192.168.0.194:8080/api/robot/info

# 测试状态
curl -v http://192.168.0.194:8080/api/robot/state

# 测试模型
curl -v http://192.168.0.194:8080/api/robot/model
```

记录:
- 响应状态码
- 响应头
- 响应体

---

### Step 2: 检查控制器日志

用户提供的日志:
```
[HttpServer] [info] GET /api/robot/info
[HttpServer] [info] Joint Slave_1000[ SIM ]: cnt_per_unit=156455.678
...
```

这表明:
- ✅ 控制器收到了请求
- ✅ 控制器正在处理
- ❓ 但未显示响应发送日志

---

### Step 3: 修改代码添加日志

在 `RobotApiClient.ts` 中添加:

```typescript
private async request<T>(method: string, path: string, body?: any, params?: Record<string, string>): Promise<T> {
  const url = new URL(path, window.location.origin);
  if (params) {
    Object.entries(params).forEach(([k, v]) => url.searchParams.append(k, v));
  }

  console.log(`[RobotApiClient] ${method} ${url.toString()}`);
  
  const options: RequestInit = {
    method,
    headers: {
      Accept: 'application/json',
      ...(body ? { 'Content-Type': 'application/json' } : {}),
    },
  };

  if (body) {
    options.body = JSON.stringify(body);
  }

  try {
    const response = await fetch(url.toString(), options);
    console.log(`[RobotApiClient] Response status: ${response.status}`);
    
    const text = await response.text();
    console.log(`[RobotApiClient] Response body length: ${text.length}`);
    
    if (!text) {
      throw new Error('Empty response from server');
    }
    
    const json: ApiResponse<T> = JSON.parse(text);
    console.log(`[RobotApiClient] Parsed response:`, json);
    
    if (!json.success) {
      throw new Error(`API Error: ${json.message} (code: ${json.code})`);
    }

    return json.data;
  } catch (error) {
    console.error(`[RobotApiClient] Request failed:`, error);
    throw error;
  }
}
```

---

## 下一步行动

### 优先级 1: 确认控制器 API 格式

请检查控制器的 HTTP 服务器实现，确认:

1. **响应格式**:
   ```json
   {
     "success": true,
     "code": 0,
     "message": "ok",
     "data": { ... }
   }
   ```

   还是:
   ```json
   {
     "joint_infos": [ ... ]
   }
   ```

2. **响应头**:
   - 是否返回 `Content-Type: application/json`?
   - 是否有 CORS 头?

3. **错误处理**:
   - 失败时返回什么?
   - HTTP 状态码?

---

### 优先级 2: 添加详细日志

修改 `RobotApiClient.ts` 添加日志，然后在浏览器控制台查看:

```
[RobotApiClient] GET http://localhost:5173/api/robot/info
[RobotApiClient] Response status: 200
[RobotApiClient] Response body length: 1234
[RobotApiClient] Parsed response: { success: true, data: {...} }
```

---

### 优先级 3: 测试简单端点

测试一个更简单的端点，确认控制器基本通信正常:

```bash
# 如果控制器有健康检查端点
curl http://192.168.0.194:8080/health

# 或根路径
curl http://192.168.0.194:8080/
```

---

## 已完成的修复

| 问题 | 修复状态 |
|------|----------|
| CORS 跨域 | ✅ Vite 代理配置完成 |
| useRobotConnection 未挂载 | ✅ 已在 AppLayout 中调用 |
| 模型 URL 错误 | ✅ 改为从 API 获取 YAML |
| 默认端口错误 | ✅ 改为 8080 |
| 相对 URL | ✅ 所有 API 调用使用相对路径 |

---

## 待确认

| 问题 | 状态 | 负责方 |
|------|------|--------|
| 控制器响应格式 | ❓ 需确认 | 控制器开发者 |
| 控制器 HTTP 方法 | ❓ 需确认 | 控制器开发者 |
| 控制器超时设置 | ❓ 需确认 | 控制器开发者 |

---

**结论**: Vite 代理配置正确，浏览器请求已成功代理到控制器。问题出在控制器返回空响应。需要检查控制器 HTTP 服务器实现。

**生成**: Claude Code (claude-opus-4-6)
**日期**: 2026-06-09
