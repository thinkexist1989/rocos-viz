/api/robot/info# HTTP/JSON 通信层代码评审

评审范围：`docs/HTTP-development-plan.md`、`config/rocos-API.yaml`、`CMakeLists.txt`、`src/HttpRobotClient.h/.cpp`、`src/ConnectDialog.h/.cpp`、`tests/test_http_contract_static.py`。本次评审基于当前代码更新后的状态。

## Findings

### Medium: 后台写模型线程直接持有 `QPointer<ConnectDialog>`，生命周期与线程安全边界偏脆

- 位置：`src/ConnectDialog.cpp::getRobotModel`，约第 548-564 行。
- 风险：后台 `std::thread` 捕获 `QPointer<ConnectDialog>`，在线程内检查 `if (!self)` 后再 `QMetaObject::invokeMethod(self.data(), ...)`。这里跨线程读取 Qt 对象弱指针并用对象作为投递目标，缺少明确的同步边界；如果对话框销毁和后台写完成竞态发生，存在投递失败或悬空目标风险。
- 建议：使用 Qt 管理的 worker/future watcher，或将结果投递到稳定的主线程对象后再在 UI 线程检查 `QPointer`；避免 detached thread 直接把 QWidget 派生对象作为跨线程生命周期锚点。

### Medium: `MoveResponse` 已保存任务 ID，但状态结果仍只在内部缓存/日志中可见

- 位置：`src/ConnectDialog.cpp::parseMoveResponse/queryMoveStatus/parseTaskStatus`，约第 772、812、844 行；`src/ConnectDialog.h` 约第 451-455 行。
- 现状：异步响应已保存 `last_move_task_id_/status/message` 并轮询 `/api/move/status?task_id=...`；同步 `result/message` 也会解析，非 0 result 会 warning。
- 风险：这些字段全是 private cache，没有 getter 或 signal，也没有 UI 展示/状态栏/日志信号输出。用户侧仍难以区分“任务创建成功但后续失败”和“运动完成”，除非看 debug warning。
- 建议：增加 `moveTaskUpdated(task_id, status, result, message)` 信号或只读 getter，并让 UI 或日志面板订阅。对 `TaskStatusData.type/create_time/finish_time` 也应按 YAML 至少解析保存或明确暂不使用。

### Medium: 状态和 robot info 解析更严格了，但 schema 契约仍未由真实 mock/样例验证

- 位置：`src/ConnectDialog.cpp::parseRobotInfo/parseRobotState`，约第 872、931 行；`config/rocos-API.yaml` 约第 752 行。
- 现状：坏 schema 不再默认覆盖 cache，缺少 `joint_states`、pose、hardware、关节速度/加速度/load、orientation `w` 等都会返回 false。
- 风险：`/api/robot/info` 和 `/api/robot/model` 的 `data` 结构在 YAML 中没有完整展开。代码现在要求 `joint_infos[].cnt_per_unit/torque_per_unit/ratio/pos_zero_offset/user_unit_name` 全部存在；如果真实控制器或 mock 返回的是最小字段集，连接会失败。
- 建议：用 Python/FastAPI mock server 固化“真实可接受 schema”，并补一组契约测试。没有样例前，严格校验是防御性的，但兼容性还不能确认。

### Medium: 当前测试是静态字符串检查，能防回退但不能证明行为正确

- 位置：`tests/test_http_contract_static.py`，约第 13-55 行。
- 风险：测试只检查源码中是否包含 `QNetworkAccessManager`、`last_move_task_id_`、`status != 200`、`std::thread` 等字符串；它不能验证 callback 时序、超时行为、坏 JSON 不覆盖 cache、`MoveResponse` 轮询终止、mesh 文件缺失不 emit `showRobot()` 等真实行为。
- 建议：按 `docs/HTTP-test-plan.md` 增加 FastAPI mock server，再补 Qt 集成测试或最小手动脚本记录。静态测试可以保留作为“防止同步 HTTP 回流”的烟雾测试，但不能替代协议行为测试。

### Low: `connect_timeout_ms_` 被保存但没有独立使用

- 位置：`src/HttpRobotClient.h` 约第 41 行；`src/HttpRobotClient.cpp::setTimeouts/getJson/postJson/downloadBinary`，约第 15、21、29、40 行。
- 风险：`setTimeouts(connect_ms, read_ms, write_ms)` 暗示有连接、读取、写入三个阶段，但实际 GET 和 download 都只用 `read_timeout_ms_`，POST 只用 `write_timeout_ms_`。这会让调用者误以为连接阶段能单独控制。
- 建议：要么简化 API 为 `setJsonTimeout()/setBinaryTimeout()`，要么在 Qt 网络层实现更清楚的请求级 timeout 策略。

### Low: 旧 getter 语义和新 YAML 字段仍不完全一致

- 位置：`src/ConnectDialog.h::getJointTorque/getMinCyclicTime/getCurrCyclicTime`，约第 61、69、111 行。
- 风险：`getJointTorque()` 返回的是 `joint_states[].acceleration`，而 YAML 状态字段没有 torque；`getMinCyclicTime()` 和 `getCurrCyclicTime()` 都返回 `cycle_time_avg`。这不会阻塞 HTTP 切换，但 UI 曲线/文案可能误导用户。
- 建议：如果新版 API 没有 torque/min/current cycle 字段，应改名或在 UI 中改为显示 `load`/`cycle_time_avg/max`。

## 已修复的旧问题

- `HttpRobotClient` 已改用 Qt `QNetworkAccessManager` 异步 callback，未发现 `httplib`、同步 `.Get()`、同步 `.Post()` 回流。
- `getRobotState()` 的 20 ms timer 不再直接执行同步 HTTP；`is_polling_state_` 仍用于防止请求重叠。
- `MoveResponse.data.task_id/status/message` 和同步 `result/message` 已有基础解析，且有 `/api/move/status?task_id=...` 查询入口。
- `parseRobotInfo()` 和 `parseRobotState()` 已改为先解析到临时 cache，成功后才覆盖成员 cache。
- 模型下载失败或空 body 不再继续写 `config.yaml`/emit `showRobot()`。
- mesh HTTP status 已要求 `200`，`Content-Disposition` header 查找已大小写不敏感。
- 大 mesh 下载不再复用 50 ms 状态读取 timeout：`HttpRobotClient::downloadFile()` 使用独立 `binary_timeout_ms_`，通过 `readyRead` 流式写入临时文件；`ConnectDialog::getRobotModel()` 保存临时文件路径并在后台写入阶段移动/复制到最终 mesh 文件，不再缓存所有 mesh body。

## Open Questions

- `/api/robot/info`、`/api/robot/model` 的 `data` schema 是否需要写回 OpenAPI，避免客户端和控制器各自猜字段。
- `MoveResponse` 的任务状态是否要展示在 UI，还是只进入日志/调试输出。
- 扩展接口 `/api/move/circle`、`/api/move/path`、`/api/move/pathway`、单轴 move/stop、多轴和标定接口是否属于当前验收范围，还是只保留在 mock server 中。

## Test Gaps

- 还没有 Python/FastAPI mock server 覆盖正常、慢响应、错误响应、坏 schema、异步任务状态和 mesh header/status。
- 还没有 10 分钟 20 ms 轮询稳定性记录。
- 还没有大 mesh 下载的耗时、RSS 峰值和 UI 响应记录。
- 还没有真实控制器样例 JSON 与当前严格解析逻辑的对照测试。
