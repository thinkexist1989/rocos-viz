# HTTP/JSON 通信层测试计划

目标：验证 HTTP/JSON 通信层按 `config/rocos-API.yaml` 替换旧通信路径，并覆盖连接、状态、控制、运动、模型、错误恢复和预留接口。测试以 mock server 为主，真实机器人或真实控制器只做补充验收。

## 1. 构建验证

- 干净构建：`cmake -S . -B build-http-review -DCMAKE_BUILD_TYPE=RelWithDebInfo && cmake --build build-http-review -j$(nproc)`。
- 依赖检查：确认构建不需要旧 gRPC/protobuf 生成头、旧协议静态库、旧运行时库、`cpp-httplib` 或 `nlohmann::json`。
- 残留检查：`rg -n "grpc|protobuf|proto|stub|channel|rocos_comm|Protocol" CMakeLists.txt src cmake docs`，区分“构建仍引用”与“仅未使用残留”。
- 启动检查：运行 `build-http-review/bin/rocos-viz`，确认无 mock server 时连接失败但进程不崩溃。

## 2. Mock Server 覆盖矩阵

mock server 应记录每个请求的方法、path、query、body、响应耗时，并支持正常、慢响应、错误响应三类模式。客户端侧必须只使用 Qt 原生 `QNetworkAccessManager`。

必须覆盖的接口：

- 机器人状态与信息：`GET /api/robot/info`、`GET /api/robot/state`、`GET /api/robot/enabled`、`GET /api/robot/model`、`GET /api/robot/model/mesh?path=...`。
- 基础控制：`POST /api/robot/enable`、`POST /api/robot/disable`、`POST /api/robot/workmode`。
- 单轴：`POST /api/axis/single/enable`、`POST /api/axis/single/disable`、`POST /api/axis/single/move`、`POST /api/axis/single/stop`。
- 多轴：`POST /api/axis/multi/enable`、`POST /api/axis/multi/disable`、`POST /api/axis/multi/move`、`POST /api/axis/multi/stop`、`POST /api/axis/multi/sync`。
- 拖拽：`POST /api/drag/start`、`POST /api/drag/stop`。
- 运动：`POST /api/move/joint`、`POST /api/move/joint_ik`、`POST /api/move/linear`、`POST /api/move/linear_fk`、`POST /api/move/circle`、`POST /api/move/path`、`POST /api/move/pathway`、`POST /api/move/stop`、`GET /api/move/status?task_id=...`。
- 标定：`POST /api/calibration/pose`、`POST /api/calibration/tool`、`POST /api/calibration/object`、`POST /api/calibration/run`、`GET /api/calibration/result`。

## 3. 正常流程测试

- 连接：输入 mock server IP/port，点击连接，期望先请求 `/api/robot/info`，成功后 emit 连接状态并开始轮询 `/api/robot/state`。
- 状态字段：返回 7 个 `joint_states`，包含 `name/position/velocity/acceleration/load/status`；返回 `flange_pose/tool_pose/object_pose/hardware`。在 UI 和日志中确认关节数、状态、硬件类型、法兰 RPY 和 PlotWidget 数据更新。
- 控制命令：点击全局上电/下电、工作模式切换、单轴上电/下电，mock server 校验 body 字段和 YAML enum。
- 点动：覆盖 `J0..J6`、`BASE_X/Y/Z/ROLL/PITCH/YAW`、`FLANGE_*`、`TOOL_*`、`OBJECT_*`；按下发送 `POSITIVE/NEGATIVE`，松开发送 `/api/drag/stop`。
- 运动：触发 `MoveJ`、`MoveJ_IK`、`MoveL`、`MoveL_FK`，校验 `joints` 或 `pose.position/orientation`、`speed`、`acceleration`、`time`、`radius`、`asynchronous`。

## 4. MoveResponse 和 TaskStatusData

- 异步响应：mock 返回 `{success:true, code:0, message:"ok", data:{task_id:"task-1", status:"RUNNING", message:"task created"}}`，确认客户端至少不报错；如果后续补齐任务跟踪，应确认保存 task id。
- 同步响应：mock 返回 `{success:true, code:0, message:"ok", data:{result:0, message:"done"}}`，确认客户端能解析/展示或记录同步结果。
- 异步状态：`GET /api/move/status?task_id=task-1` 依次返回 `RUNNING`、`COMPLETED`、`FAILED`、`STOPPED`，校验 `TaskStatusData.task_id/type/status/result/message/create_time/finish_time`。
- 失败码：运动接口返回 `success=false`、HTTP 409、HTTP 500，确认错误信息包含 code/message 或 HTTP status，且 UI 不误判为运动完成。

## 5. Mesh 下载测试

- 基础下载：`GET /api/robot/model` 返回多个 link 和 mesh path；`GET /api/robot/model/mesh` 返回 STL body，确认生成 `models/<robot_name>/config.yaml` 和 mesh 文件，并 emit 后模型可显示。
- URL 编码：mesh path 覆盖空格、中文、`subdir/link 1.stl`、`a+b.stl`、`a?b.stl`，mock server 校验收到的 query 解码值等于原始 path。
- 文件名：分别测试 `Content-Disposition: filename="link.stl"`、`filename*=UTF-8''link%201.stl`、无 header、header 小写形式。
- 大文件：单个 mesh 至少 256 MB，记录下载耗时、进程内存峰值、UI 是否卡顿。
- 失败：空 body、404、204、500、写目录无权限，确认不会静默生成不可用模型；如当前实现仍 emit，需要记录为缺陷。

## 6. 错误注入和恢复

- 连接失败：server 未启动、端口错误、连接超时、响应非法 JSON、`success=false`，期望不启动轮询并 emit `connectState(false)`。
- 状态失败：连续返回 500、非法 JSON、`success=false`、慢响应超过 read timeout，确认失败计数达到阈值后停止轮询并断开。
- 字段缺失：`joint_states` 缺失、pose orientation 缺 `w`、hardware 缺字段、字段类型错误，确认进入错误路径且不覆盖上一帧 cache。
- 命令失败：每个 POST 返回 400/409/500 和 `success=false`，确认日志有清晰错误，连接状态不被意外清除。
- 断开恢复：连接后调用 shutdown，确认后续不再发 `/api/robot/state`；再次连接同一 mock server 可恢复。

## 7. 10 分钟轮询稳定性

- mock server 正常 20 ms 状态响应，运行 10 分钟，记录请求总数、平均间隔、失败次数、CPU、RSS、UI 响应。
- mock server 每 50 次插入一次 200 ms 慢响应，运行 10 分钟，确认没有请求重叠，UI 不因网络等待而卡顿。
- mock server 每 100 次返回一次 `success=false`，运行 10 分钟，确认偶发错误不会断线，连续 10 次错误会断线。
- 结束后检查进程内存无持续增长，`models/` 不反复写入非预期文件。

## 8. 手动测试清单

- 无 server：点击连接，界面可继续操作。
- 正常 server：连接、状态刷新、上电/下电、工作模式切换、关节点动、笛卡尔点动。
- 自动加载模型：勾选 auto load，连接后模型显示；取消 auto load 后不请求模型。
- 运动按钮：四类运动都能发出请求，失败响应不导致崩溃。
- 网络中断：连接后停止 mock server，确认约 10 次失败后断开，重启 server 后可重新连接。

## 9. 标定/多轴预留接口

- 即使当前 UI 未接入，也要求 mock server 覆盖并提供契约测试样例。
- 单轴 move/stop：校验 `id/pos/max_vel/max_acc/max_jerk/least_time/raw_data`。
- 多轴 move/sync：校验数组长度一致、`sync` enum 为 `none/time/phase`。
- 标定 pose/tool/object/run/result：校验 `Pose` 四元数字段、`frame` enum 为 `tool/object`、`result` 返回 `error_state` 和 `pose`。
- 后续接入 UI 或脚本入口时，直接复用这些 mock case 做回归。
