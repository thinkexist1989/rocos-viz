# HTTP/JSON 实现说明

本轮实现将 HTTP 客户端从同步阻塞的第三方实现改为 Qt 原生异步网络栈：

- 网络传输：`QNetworkAccessManager`
- 响应解析：`QJsonDocument`
- JSON 数据：`QJsonObject`、`QJsonArray`
- 二进制 mesh：`QByteArray`

项目不再依赖 `cpp-httplib` 或 `nlohmann::json`。

## 设计思路

`HttpRobotClient` 保留为通信封装层，但 API 改为回调式异步接口：

- `getJson(path, callback)`
- `postJson(path, body, callback)`
- `downloadBinary(path, callback)`

每个请求立即返回，网络完成后通过 callback 交回结果。这样 `ConnectDialog` 的连接、状态轮询、命令发送和模型下载都不再通过阻塞等待卡住 Qt 事件循环。

## 防御性解析

所有 JSON 响应先由 `QJsonDocument::fromJson` 解析：

- JSON 语法错误直接失败。
- 响应根节点不是 object 直接失败。
- envelope 缺少 boolean `success` 直接失败。
- `success=false` 时使用 `code/message` 生成错误信息。
- `data` 不是 object 时返回空 object，只允许命令类接口这样处理。

`ConnectDialog` 对机器人状态再做二次 schema 校验：

- `joint_states` 必须是非空数组。
- 每个关节必须至少包含 `name`、`position`、`status`。
- `flange_pose/tool_pose/object_pose` 必须包含 `position` 和 `orientation`。
- `hardware` 必须是 object，且必须包含 `type`。

解析失败不会覆盖旧 cache，也不会 emit `newStateComming()`。

## 数据流

连接流程：

1. 创建 `HttpRobotClient`。
2. 异步 `GET /api/robot/info`。
3. 成功解析关节信息后设置连接状态。
4. 启动 20 ms 状态轮询。

状态流程：

1. `QTimer` 触发 `getRobotState()`。
2. 若上一轮状态请求未完成，则跳过本轮。
3. 异步 `GET /api/robot/state`。
4. 成功解析 `RobotStateData` 后更新 cache。
5. emit `newStateComming()`。

命令流程：

1. UI 调用 `ConnectDialog` 命令函数。
2. 构造符合 OpenAPI 的 `QJsonObject`。
3. 异步 POST 到对应 endpoint。
4. callback 中记录失败信息。

模型流程：

1. 异步 `GET /api/robot/model`。
2. 并发异步下载所有 mesh。
3. 全部 mesh 成功后生成 `config.yaml`。
4. emit `showRobot(config.yaml)`。
5. 如果任何必需 mesh 失败，不显示不完整模型。

## 已知风险

- `QNetworkAccessManager` 解决了网络阻塞，但大 mesh 写文件仍在回调线程执行，极大文件可能短暂占用 UI 线程。
- `/api/robot/info` 和 `/api/robot/model` 的 OpenAPI `data` schema 仍不完整，当前实现按既有字段约定解析。
- 运动响应当前仍只记录 POST 成功/失败，完整 `MoveResponse.task_id/status/result` 链路需要后续继续补齐。
