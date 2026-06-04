# HTTP/JSON 通信层取代开发计划

目标：按新版 [config/rocos-API.yaml](../config/rocos-API.yaml) 将 `rocos-viz` 的机器人通信层完全替换为基于 Qt 原生 `QNetworkAccessManager` 的 HTTP/JSON 客户端。该 OpenAPI YAML 是唯一接口依据；本计划不修改它，完成后只保留 HTTP 单一路径。

## 1. 目标与原则

- HTTP 是唯一通信方式：连接、状态轮询、控制命令、模型下载和后续标定接口都走 REST API。
- OpenAPI YAML 是唯一接口规范：路径、字段名、枚举值、响应结构以 `config/rocos-API.yaml` 为准。
- 客户端使用 Qt 原生网络与 JSON API：`QNetworkAccessManager`、`QNetworkReply`、`QJsonDocument`、`QJsonObject`、`QJsonArray`。
- 不引入 `cpp-httplib`、`nlohmann::json` 或其他额外 JSON 库。
- `ConnectDialog` 继续作为 UI 侧通信门面，尽量保留 `RocosMainWindow`、`PlotWidget`、`ModelLoaderDialog` 的调用方式。
- 状态轮询保持 20 ms 目标周期，但必须防止请求重入和 UI 长时间阻塞。

## 2. 实施步骤

### 2.1 清理构建依赖

修改 `CMakeLists.txt`：

- 删除旧通信相关 `CMAKE_PREFIX_PATH`。
- 删除旧代码生成 custom command。
- 删除旧协议静态库 target 和链接项。
- 删除生成头文件 include 目录。
- 保留 Qt Network 链接，确保 HTTP 客户端只依赖 Qt 原生网络模块。
- 保留 Qt、VTK、Eigen、yaml-cpp、orocos_kdl 等现有依赖。

验收：主程序构建不再需要旧通信编译器、插件、生成头文件或运行时库。

### 2.2 建立 HTTP 客户端封装

新增 `HttpRobotClient`，集中处理异步 HTTP 传输、JSON 防御性解析、二进制下载和错误信息。

建议接口：

```cpp
class HttpRobotClient {
public:
    HttpRobotClient(const QString &host, int port);

    void setTimeouts(int connect_ms, int read_ms, int write_ms);

    using JsonCallback = std::function<void(bool ok, const QJsonObject &data, const QString &error)>;
    using BinaryCallback = std::function<void(bool ok, const QByteArray &body, const QList<QNetworkReply::RawHeaderPair> &headers, const QString &error)>;

    void getJson(const QString &path, JsonCallback callback);
    void postJson(const QString &path, const QJsonObject &body, JsonCallback callback);
    void downloadBinary(const QString &path, BinaryCallback callback);

private:
    void parseStandardResponse(const QByteArray &body, JsonCallback callback) const;
    QNetworkAccessManager manager_;
};
```

统一规则：

- 所有接口都是异步回调，不在 UI 线程执行阻塞等待。
- HTTP status 非 2xx：失败。
- `QJsonDocument::fromJson` parse 失败：失败，不抛 C++ 异常。
- envelope 缺少 boolean `success`：失败。
- `StandardResponse.success == false`：失败，错误信息使用 `message` 和 `code`。
- 对命令类接口，`data` 允许为空。
- 对 mesh 下载接口，不解析 JSON，直接读取 `application/octet-stream` body。

### 2.3 改造 `ConnectDialog`

修改 `src/ConnectDialog.h` 和 `src/ConnectDialog.cpp`：

- 删除旧通信 include、using 声明、stub/channel 成员和旧响应对象。
- 增加 `std::unique_ptr<HttpRobotClient> http_client_`。
- 使用本地 cache 替代旧响应对象，getter 继续从 cache 读取。
- `connectedToRobot(true, autoLoadModel)` 创建 HTTP client，异步调用 `GET /api/robot/info`，成功后启动状态轮询。
- `getRobotState()` 异步调用 `GET /api/robot/state`，解析 `RobotStateData` 后 emit `newStateComming()`。
- `shutdown()` 停止轮询、释放 HTTP client、发出 `connectState(false)`。
- 命令函数根据 UI 参数构造 JSON body，POST 到对应 endpoint。

建议 cache：

```cpp
struct JointInfoCache {
    QString name;
    double cnt_per_unit = 0.0;
    double torque_per_unit = 0.0;
    double ratio = 0.0;
    int pos_zero_offset = 0;
    QString user_unit_name;
};

struct JointStateCache {
    QString name;
    double position = 0.0;
    double velocity = 0.0;
    double acceleration = 0.0;
    double load = 0.0;
    QString status;
};

struct PoseCache {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    double qx = 0.0;
    double qy = 0.0;
    double qz = 0.0;
    double qw = 1.0;
};

struct HardwareCache {
    QString type;
    double cycle_time_avg = 0.0;
    double cycle_time_max = 0.0;
    int slave_count = 0;
};

struct RobotStateCache {
    QVector<JointStateCache> joint_states;
    PoseCache flange_pose;
    PoseCache tool_pose;
    PoseCache object_pose;
    HardwareCache hardware;
};
```

### 2.4 接入新版状态字段

`GET /api/robot/state` 返回 `StandardResponse.data: RobotStateData`。

字段映射：

- `data.joint_states[]` -> `RobotStateCache::joint_states`
- `joint_states[].name` -> `JointStateCache::name`
- `joint_states[].position` -> `JointStateCache::position`
- `joint_states[].velocity` -> `JointStateCache::velocity`
- `joint_states[].acceleration` -> `JointStateCache::acceleration`
- `joint_states[].load` -> `JointStateCache::load`
- `joint_states[].status` -> `JointStateCache::status`
- `data.flange_pose` -> `RobotStateCache::flange_pose`
- `data.tool_pose` -> `RobotStateCache::tool_pose`
- `data.object_pose` -> `RobotStateCache::object_pose`
- `data.hardware.type` -> `HardwareCache::type`
- `data.hardware.cycle_time_avg` -> `HardwareCache::cycle_time_avg`
- `data.hardware.cycle_time_max` -> `HardwareCache::cycle_time_max`
- `data.hardware.slave_count` -> `HardwareCache::slave_count`

`Pose.orientation.{x,y,z,w}` 是四元数字段。`getFlangePose()` 使用 KDL 将 `flange_pose` 的 position 和 orientation 转为 `KDL::Frame`。

### 2.5 接入控制与运动接口

基础控制：

- `powerOn()` -> `POST /api/robot/enable`，body `{}`。
- `powerOff()` -> `POST /api/robot/disable`，body `{}`。
- `setRobotWorkMode(mode)` -> `POST /api/robot/workmode`，body `{"mode":"position"}` 等 YAML 枚举。
- `powerOn(id)` -> `POST /api/axis/single/enable`，body `{"id": id}`。
- `powerOff(id)` -> `POST /api/axis/single/disable`，body `{"id": id}`。

点动：

- `jointJogging(id, dir)` -> `POST /api/drag/start`。
- `cartesianJogging(frame, freedom, dir)` -> `POST /api/drag/start`。
- 停止点动 -> `POST /api/drag/stop`。
- `flag` 使用 YAML 枚举：`J0..J6`、`BASE_X`、`FLANGE_ROLL`、`TOOL_Y`、`OBJECT_YAW` 等。
- `direction` 使用 YAML 枚举：`POSITIVE`、`NEGATIVE`、`NONE`。

运动：

- `moveJ(q)` -> `POST /api/move/joint`。
- `moveJ_IK(pose)` -> `POST /api/move/joint_ik`。
- `moveL(pose)` -> `POST /api/move/linear`。
- `moveL_FK(q)` -> `POST /api/move/linear_fk`。
- 预留接入 `POST /api/move/circle`、`/api/move/path`、`/api/move/pathway`、`/api/move/stop`。

运动响应按 `MoveResponse` 解析：

- `asynchronous=true` 时，读取 `data.task_id`、`data.status`、`data.message`。
- `asynchronous=false` 时，读取 `data.result`、`data.message`。
- 如保存了 `task_id`，可用 `GET /api/move/status?task_id=...` 查询 `TaskStatusData`。

### 2.6 接入模型与 mesh 下载

`getRobotModel()` 流程：

1. `GET /api/robot/model`。
2. 解析 `data.name` 和 `data.links[]`。
3. 写入 `models/<robot_name>/config.yaml`。
4. 对每个 link 的 `mesh` 调用 `GET /api/robot/model/mesh?path=...`。
5. 保存 mesh 到 `models/<robot_name>/`。
6. emit `showRobot(config.yaml)`。

下载要求：

- `path` 必须 URL encode。
- HTTP status 必须为 200。
- body 不得为空。
- 优先使用 `Content-Disposition` 文件名。
- 没有响应文件名时，使用 mesh path 的 basename。
- 单个 mesh 至少支持 256 MB。

### 2.7 标定、多轴和扩展接口

计划中保留接口规划，主 UI 未使用的部分可后续接入：

- 多轴：`POST /api/axis/multi/enable`、`disable`、`move`、`stop`、`sync`。
- 单轴运动：`POST /api/axis/single/move`、`stop`。
- 标定：`POST /api/calibration/pose`、`tool`、`object`、`run`，`GET /api/calibration/result`。
- 任务状态：`GET /api/move/status?task_id=...`。

mock server 必须覆盖这些接口，真实 UI 接入可按功能需要推进。

## 3. 接口与数据流

### 3.1 连接流程

1. 用户输入 IP 和 port。
2. `ConnectDialog` 创建 `HttpRobotClient`。
3. 异步调用 `GET /api/robot/info`。
4. 解析 `StandardResponse`。
5. 缓存机器人信息。
6. 设置 `is_connected_ = true`。
7. emit `connectState(true)`。
8. 启动 20 ms 状态轮询。
9. 如果勾选自动加载模型，调用 `getRobotModel()`。

失败时：

- 设置 `is_connected_ = false`。
- emit `connectState(false)`。
- 不启动轮询。
- 错误写入日志或调试输出。

### 3.2 状态流程

1. 定时器触发 `getRobotState()`。
2. 如果上一轮请求仍在进行，跳过本次轮询。
3. 异步调用 `GET /api/robot/state`。
4. 解析 `data: RobotStateData`。
5. 更新关节、法兰、工具、工件和硬件 cache。
6. emit `newStateComming()`。
7. `RocosMainWindow::updateRobotState()` 和 `PlotWidget::handleNewState()` 从 getter 读取 cache。

连续失败达到阈值后停止轮询并 emit `connectState(false)`。

### 3.3 命令流程

1. UI slot 调用 `ConnectDialog` 命令函数。
2. `ConnectDialog` 将 UI 参数转换成 YAML 规定的 JSON body。
3. `HttpRobotClient::postJson()` 异步 POST 到对应 endpoint。
4. 解析 `StandardResponse` 或 `MoveResponse`。
5. 失败时记录 `code/message`。
6. 成功时不阻塞 UI 等待状态变化，状态变化由下一轮状态轮询体现。

### 3.4 模型流程

1. 调用 `GET /api/robot/model`。
2. 将返回 link 数据转换成现有 `Model` 可读取的 yaml 格式。
3. 逐个下载 mesh。
4. 下载完成后 emit `showRobot(config.yaml)`。
5. 主窗口调用 `visualWidget->displayModelFromYaml()`。

### 3.5 异步运动流程

1. 发送运动命令时 body 可设置 `asynchronous=true`。
2. 解析 `MoveResponse.data.task_id`。
3. 保存最近任务 ID。
4. 需要查询时调用 `GET /api/move/status?task_id=...`。
5. 解析 `TaskStatusData.status`：`RUNNING`、`COMPLETED`、`FAILED`、`STOPPED`。

## 4. Mock Server 与测试

### 4.1 Mock Server

新增 mock HTTP server，返回符合新版 YAML 的响应；mock server 可用任意轻量实现，客户端侧不依赖 `cpp-httplib`。

必须覆盖：

- `GET /api/robot/info`
- `GET /api/robot/state`
- `GET /api/robot/enabled`
- `GET /api/robot/model`
- `GET /api/robot/model/mesh`
- `POST /api/robot/enable`
- `POST /api/robot/disable`
- `POST /api/robot/workmode`
- `POST /api/axis/single/enable`
- `POST /api/axis/single/disable`
- `POST /api/axis/single/move`
- `POST /api/axis/single/stop`
- `POST /api/axis/multi/enable`
- `POST /api/axis/multi/disable`
- `POST /api/axis/multi/move`
- `POST /api/axis/multi/stop`
- `POST /api/axis/multi/sync`
- `POST /api/drag/start`
- `POST /api/drag/stop`
- `POST /api/move/joint`
- `POST /api/move/joint_ik`
- `POST /api/move/linear`
- `POST /api/move/linear_fk`
- `POST /api/move/circle`
- `POST /api/move/path`
- `POST /api/move/pathway`
- `POST /api/move/stop`
- `GET /api/move/status`
- `POST /api/calibration/pose`
- `POST /api/calibration/tool`
- `POST /api/calibration/object`
- `POST /api/calibration/run`
- `GET /api/calibration/result`

状态 mock 数据必须包含：

- `joint_states[]`
- `flange_pose`
- `tool_pose`
- `object_pose`
- `hardware.type`
- `hardware.cycle_time_avg`
- `hardware.cycle_time_max`
- `hardware.slave_count`

### 4.2 验收测试

必须验证：

- 服务端未启动时，连接失败且 GUI 不崩溃。
- mock server 正常时，连接成功并持续刷新状态。
- 20 ms 轮询不重入；慢响应时跳过重叠请求。
- `RobotStateData` 字段正确进入 cache。
- `getFlangePose()` 能从 `Pose.orientation` 四元数生成 KDL frame。
- 上电、下电、工作模式、单轴上电/下电、点动请求 body 符合 YAML。
- `MoveJ`、`MoveJ_IK`、`MoveL`、`MoveL_FK` 请求 body 符合 YAML。
- `MoveResponse` 同步返回 `result/message` 时解析正确。
- `MoveResponse` 异步返回 `task_id/status/message` 时解析正确。
- `GET /api/move/status` 能解析 `TaskStatusData`。
- mesh 下载后生成 `models/<robot_name>/config.yaml` 并能显示模型。
- 非 2xx、`success=false`、非法 JSON、空 mesh、请求超时均能产生清晰错误并保持客户端可恢复。
- 断开连接后停止状态请求。

### 4.3 长跑验证

- 连接 mock server 后连续轮询 10 分钟。
- 记录 CPU、内存和请求失败次数。
- 验证无明显内存增长。
- 验证 UI 不出现长时间卡顿。

## 5. 实现交付物

- HTTP 版 `ConnectDialog`。
- `HttpRobotClient` 或等价 HTTP 客户端封装。
- 更新后的 `CMakeLists.txt`。
- Mock HTTP server。
- HTTP 验收脚本或手动测试说明。
- README 中新增 HTTP 模式构建、运行和 mock server 使用说明。

## 6. 风险与处理

- `QNetworkAccessManager` 本身异步非阻塞，但回调中解析大 JSON 或写大 mesh 文件仍可能短暂占用 UI 线程，后续可把大文件写入移到 worker。
- HTTP/JSON 高频轮询开销更高：先保持 20 ms 目标周期，后续根据实测调整。
- YAML schema 中部分接口 data 未完全展开：实现时严格按现有字段解析，缺失字段使用错误日志记录，不修改 YAML。
- mesh 大文件会带来内存峰值：第一版至少限制错误处理清晰，后续可改成流式写入。
- 明文 HTTP 不适合生产网络：生产部署再引入 HTTPS 和认证。
