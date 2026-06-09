# ROCOS-Viz Qt/C++ Implementation Details

> Extracted from source code for Web rewrite reference. Last updated: 2026-06-09.

---

## Table of Contents

1. [Application Architecture Overview](#1-application-architecture-overview)
2. [Main Window (RocosMainWindow)](#2-main-window-rocosmainwindow)
3. [Connection Dialog (ConnectDialog)](#3-connection-dialog-connectdialog)
4. [HTTP Client (HttpRobotClient)](#4-http-client-httprobotclient)
5. [REST API Specification](#5-rest-api-specification)
6. [3D Scene (SceneWidget)](#6-3d-scene-scenewidget)
7. [Robot Model (Model + Link)](#7-robot-model-model--link)
8. [Forward Kinematics Algorithm](#8-forward-kinematics-algorithm)
9. [Trajectory Recording](#9-trajectory-recording)
10. [Joint Position Widget](#10-joint-position-widget)
11. [Cartesian Position Widget](#11-cartesian-position-widget)
12. [PositionBar (Progress Bar)](#12-positionbar-progress-bar)
13. [Real-time Plotting (PlotWidget)](#13-real-time-plotting-plotwidget)
14. [Plot Configuration Dialog (PlotConfigDialog)](#14-plot-configuration-dialog-plotconfigdialog)
15. [Model Loader Dialog (ModelLoaderDialog)](#15-model-loader-dialog-modelloaderdialog)
16. [Script Dialog (ScriptDialog)](#16-script-dialog-scriptdialog)
17. [About Dialog (AboutDialog)](#17-about-dialog-aboutdialog)
18. [Protocol Constants (Protocol.h)](#18-protocol-constants-protocolh)
19. [Robot Model Config Format (YAML)](#19-robot-model-config-format-yaml)
20. [Existing Model Parameters](#20-existing-model-parameters)
21. [UI Layout and Styling Details](#21-ui-layout-and-styling-details)

---

## 1. Application Architecture Overview

```
RocosMainWindow (QMainWindow)
  |-- ConnectDialog (robot communication, HTTP API)
  |     |-- HttpRobotClient (HTTP REST client)
  |-- SceneWidget (VTK 3D visualization, QVTKOpenGLNativeWidget)
  |     |-- Model (robot FK model)
  |           |-- Link[] (individual links with STL meshes)
  |-- PlotWidget (real-time data plotting)
  |-- PlotConfigDialog (configure which data to plot)
  |-- ModelLoaderDialog (load robot model from file or controller)
  |-- ScriptDialog (robot script control)
  |-- AboutDialog
```

**Signal flow:**
1. ConnectDialog polls robot state at 20ms interval via HTTP GET `/api/robot/state`
2. On successful parse, emits `newStateComming` signal
3. RocosMainWindow receives signal, calls `updateRobotState()`:
   - Updates all 7 JointPositionWidgets with joint positions (radians)
   - Updates 6 CartesianPositionWidgets with flange pose (x,y,z,roll,pitch,yaw)
   - Updates enabled/disabled icon based on joint statuses
   - Passes joint angles to SceneWidget for FK visualization
4. PlotWidget also receives `newStateComming`, appends data points to charts

---

## 2. Main Window (RocosMainWindow)

**Files:** `src/RocosMainWindow.h`, `src/RocosMainWindow.cpp`, `src/RocosMainWindow.ui`

### 2.1 Window Properties
- Size: 1920x1080
- Font: "Alibaba PuHuiTi 3.0" 11pt
- Border-radius: 20px (frameless window style)

### 2.2 Layout Structure

```
+----------------------------------------------------------+
|  Logo Button (landau-rocos-viz.png)                       |
+----------------------------------------------------------+
| Left Panel (stretch 40)  |  Right Panel (stretch 60)      |
| +----------------------+  | +---------------------------+ |
| | Toolbar buttons row  |  | | Working Mode (combo)      | |
| | [Connected][DispModel]  | | | Reference Frame (combo)   | |
| | [Enabled][AxoView]   |  | +---------------------------+ |
| | [Front][Top][Right]  |  | | Speed Scaling (slider)     | |
| | [Plot][PlotCfg][Scr] |  | | 1-999, default 250         | |
| | [About]              |  | +---------------------------+ |
| |                      |  | | Cartesian Space (6 widgets)| |
| | StackedWidget:       |  | | X/Y/Z/R/P/Y               | |
| |  [0] SceneWidget     |  | +---------------------------+ |
| |  [1] PlotWidget      |  | | Joint Space (7 widgets)    | |
| |                      |  | | J-1 through J-7            | |
| | Bottom toggles:      |  | +---------------------------+ |
| | [Axes][Mesh][Plane]  |  | | Precise Cartesian Control  | |
| | [Trajectory][Random] |  | | x/y/z/roll/pitch/yaw       | |
| | timerLabel            |  | | [MoveJ_IK] [MoveL]        | |
| | runningLabel          |  | +---------------------------+ |
| +----------------------+  | | Precise Joint Control       | |
|                           | | j1-j7 spin boxes (-180~180) | |
|                           | | [MoveJ] [MoveL_FK]         | |
|                           | +---------------------------+ |
+----------------------------------------------------------+
```

### 2.3 Key State Variables

```cpp
bool isRobotConnected = false;
bool isRobotEnabled = false;
bool isAngleDegree = true;     // toggle degree/radian display
bool isPosMM = true;           // toggle mm/m display
int currentFrame = FRAME_BASE; // FRAME_BASE(400), FRAME_FLANGE(200), FRAME_TOOL(100), FRAME_OBJECT(300)
double r2d = 180.0 / M_PI;    // radian to degree conversion
double m2mm = 1000.0;          // meter to millimeter conversion
```

### 2.4 Joint Position Widgets (7 instances)
- Names: "J-1" through "J-7", IDs: 0 through 6
- Each widget's `jointPositionJogging(id, dir)` signal connects to `connectDlg->jointJogging(id, dir)`

### 2.5 Cartesian Position Widgets (6 instances)
- Names: X(0), Y(1), Z(2), R(3), P(4), Y(5)
- X/Y/Z default to mm display (`r2d = 1000.0`)
- R/P/Y default to degree display (`r2d = 180/PI`)
- Each widget's `cartesianJogging(freedom, dir)` signal connects to `connectDlg->cartesianJogging(currentFrame + freedomOffset, id, dir)`
- Frame offset: the `currentFrame` constant is added to the freedom index to compute the API flag (e.g., FRAME_BASE(400) + FREEDOM_X(0) = 400 -> flag "BASE_X")

### 2.6 Speed Slider
- Range: 1 to 999, default: 250
- Factor = value / 1000.0 (so default 25% = 0.25)
- Updates all 13 position widgets' factor
- Calls `connectDlg->setJointSpeedScaling(factor)`

### 2.7 Reference Frame Combo
- Index 0: BASE (currentFrame = FRAME_BASE = 400)
- Index 1: FLANGE (currentFrame = FRAME_FLANGE = 200)
- Index 2: TOOL (currentFrame = FRAME_TOOL = 100)
- Index 3: OBJECT (currentFrame = FRAME_OBJECT = 300)
- Updates title text: "CARTESIAN SPACE (BASE/FLANGE/TOOL/OBJECT)" with colored highlight

### 2.8 Working Mode Combo
- Sends mode string via `connectDlg->setRobotWorkMode(index)`:
  - 0: "position"
  - 1: "ee_admit_teach"
  - 2: "jnt_admit_teach"
  - 3: "jnt_imp"
  - 4: "cart_imp"

### 2.9 Precise Control
- **MoveJ button:** reads j1-j7 QDoubleSpinBox values (degrees), divides by r2d to get radians, sends via `connectDlg->moveJ(q)`
- **MoveJ_IK button:** reads x/y/z (mm) divided by m2mm, roll/pitch/yaw (degrees) divided by r2d, sends via `connectDlg->moveJ_IK(pose)`
- **MoveL button:** same as MoveJ_IK format, sends via `connectDlg->moveL(pose)`
- **MoveL_FK button:** same as MoveJ format, sends via `connectDlg->moveL_FK(q)`

### 2.10 Stacked Widget Toggle
- Button "Plotter" toggles between index 0 (SceneWidget) and index 1 (PlotWidget)

### 2.11 Time Display
- Updates every 500ms
- Shows current time: "yyyy-MM-dd HH:mm:ss"
- Shows running time since application start

---

## 3. Connection Dialog (ConnectDialog)

**Files:** `src/ConnectDialog.h`, `src/ConnectDialog.cpp`, `src/ConnectDialog.ui`

### 3.1 Dialog Properties
- Size: 859x246
- White background, border with `#496FFF`, rounded corners
- Frameless window, translucent background
- Auto-closes when losing focus (ActivationChange event)

### 3.2 UI Elements
- IP Address field (default: `192.168.0.194`)
- Port field (default: `30001`)
- Connect button
- "Auto load robot model" checkbox
- Exit button

### 3.3 Connection Flow
1. User clicks Connect button
2. `connectedToRobot(true, autoLoadModel)` called
3. Creates `HttpRobotClient(ip, port)`
4. Sets timeouts: connect=1000ms, read=1000ms, write=500ms
5. Sends GET `/api/robot/info`
6. On success: parses robot info, sets `is_connected_ = true`, emits `connectState(true)`
7. Starts state polling timer at 20ms interval
8. Closes dialog
9. If autoLoadModel: calls `getRobotModel()` to fetch model from controller

### 3.4 State Polling
- Timer interval: `POLLING_INTERVAL_MS = 20` (20ms, ~50Hz)
- GET `/api/robot/state`
- On success: parse into `robot_state_cache_`, emit `newStateComming`
- On failure: increment `state_failure_count_`
- After `MAX_STATE_FAILURES = 10` consecutive failures: auto-disconnect via `shutdown()`

### 3.5 Data Cache Structures

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
    double position = 0.0;   // radians
    double velocity = 0.0;
    double acceleration = 0.0;
    double torque = 0.0;
    double load = 0.0;
    QString status;          // "DISABLED" | "FAULT" | "ENABLED"
};

struct PoseCache {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    double qx = 0.0;
    double qy = 0.0;
    double qz = 0.0;
    double qw = 1.0;        // identity quaternion default
};

struct HardwareCache {
    QString type;            // "SIMULATION" | "ETHERCAT" | "UNKNOWN"
    double cycle_time_min = 0.0;
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

### 3.6 Speed Parameters
- `factor_ = 0.25` (25% default)
- `max_jnt_speed_ = 1.0` (max joint speed)
- `max_cart_speed_ = 0.5` (max Cartesian speed)

---

## 4. HTTP Client (HttpRobotClient)

**Files:** `src/HttpRobotClient.h`, `src/HttpRobotClient.cpp`

### 4.1 Constructor
```cpp
HttpRobotClient(const QString &host, int port, QObject *parent = nullptr);
// Builds base_url as "http://{host}:{port}"
```

### 4.2 Methods

| Method | Signature | Description |
|--------|-----------|-------------|
| `getJson` | `(path, JsonCallback)` | HTTP GET, expects JSON response |
| `postJson` | `(path, body, JsonCallback)` | HTTP POST with JSON body |
| `downloadBinary` | `(path, BinaryCallback)` | HTTP GET, returns raw bytes |
| `downloadFile` | `(path, FileCallback)` | HTTP GET, streams to temp file |

### 4.3 Callback Types
```cpp
using JsonCallback = std::function<void(bool ok, const QJsonObject &data, const QString &error)>;
using BinaryCallback = std::function<void(bool ok, const QByteArray &body,
    const QList<QNetworkReply::RawHeaderPair> &headers, const QString &error)>;
using FileCallback = std::function<void(bool ok, const QString &file_path,
    const QList<QNetworkReply::RawHeaderPair> &headers, const QString &error)>;
```

### 4.4 Timeouts
```cpp
void setTimeouts(int connect_ms, int read_ms, int write_ms);
// Defaults: connect=1000, read=50, write=500
// Binary downloads: connect=3000, read=300000, write=5000
```

### 4.5 Request Format
- All requests include `Accept: application/json` header
- POST requests include `Content-Type: application/json`
- Relative URLs resolved against base_url
- JSON body serialized via `QJsonDocument`

### 4.6 Response Parsing (`parseStandardResponse`)
Expected envelope:
```json
{
  "success": true,
  "code": 0,
  "message": "ok",
  "data": { ... }
}
```
- If `success` is false or missing: returns error with message
- If `data` is missing: returns error
- On success: invokes callback with `data` object

### 4.7 File Download (`downloadFile`)
- Creates QTemporaryFile for streaming
- Connects `readyRead` signal to accumulate chunks
- Extracts filename from `Content-Disposition` header (supports `filename=` and `filename*=UTF-8''...`)
- Falls back to URL basename if no Content-Disposition

---

## 5. REST API Specification

**File:** `config/rocos-API.yaml` (OpenAPI 3.0)

### 5.1 Standard Response Envelope
```json
{
  "success": boolean,
  "code": integer,
  "message": string,
  "data": object
}
```

### 5.2 Shared Data Types

**Pose:**
```json
{
  "position": { "x": double, "y": double, "z": double },
  "orientation": { "x": double, "y": double, "z": double, "w": double }
}
```

**JointState:**
```json
{
  "name": string,
  "position": double,
  "velocity": double,
  "acceleration": double,
  "torque": double,
  "load": double,
  "load_torque": double,
  "status": int  // 0=DISABLED, 1=FAULT, 2=ENABLED
}
```

**JointInfo:**
```json
{
  "name": string,
  "cnt_per_unit": double,
  "torque_per_unit": double,
  "ratio": double,
  "pos_zero_offset": int,
  "zero_offset": int,
  "user_unit_name": string,
  "unit_name": string
}
```

### 5.3 Robot Endpoints

| Method | Path | Description | Request Body | Response `data` |
|--------|------|-------------|-------------|-----------------|
| GET | `/api/robot/info` | Get robot info | - | `{joint_infos: JointInfo[]}` |
| GET | `/api/robot/state` | Get robot state | - | `RobotStateData` |
| GET | `/api/robot/model` | Get robot model | - | `{name: string, links: Link[]}` |
| GET | `/api/robot/model/mesh?path=X` | Download STL mesh | - | Binary STL file |
| POST | `/api/robot/enable` | Power on all joints | `{}` | - |
| POST | `/api/robot/disable` | Power off all joints | `{}` | - |
| POST | `/api/robot/workmode` | Set work mode | `{mode: string}` | - |

**RobotStateData:**
```json
{
  "joint_states": [JointState],
  "flange_pose": Pose,
  "tool_pose": Pose,
  "object_pose": Pose,
  "hw_state": {
    "hw_type": int,
    "type": string,
    "cycle_time_avg": double,
    "cycle_time_min": double,
    "cycle_time_max": double,
    "slave_count": int
  }
}
```

**RobotModelData Link:**
```json
{
  "name": string,
  "order": int,
  "type": string,  // "unknown"|"continuous"|"prismatic"|"floating"|"planar"|"fixed"
  "mesh": string,  // relative path to STL
  "axis": { "x": int, "y": int, "z": int },
  "translate": { "x": double, "y": double, "z": double },
  "rotate": { "x": double, "y": double, "z": double },
  "translateLink": { "x": double, "y": double, "z": double },
  "rotateLink": { "x": double, "y": double, "z": double }
}
```

### 5.4 Motion Endpoints

| Method | Path | Description | Request Body |
|--------|------|-------------|-------------|
| POST | `/api/move/joint` | Joint space move | `{joints[], speed, acceleration, time, radius, asynchronous}` |
| POST | `/api/move/joint_ik` | Joint move via IK | `{pose, speed, acceleration, time, radius, asynchronous}` |
| POST | `/api/move/linear` | Linear Cartesian move | `{pose, speed, acceleration, time, radius, asynchronous}` |
| POST | `/api/move/linear_fk` | Linear move via FK | `{joints[], speed, acceleration, time, radius, asynchronous}` |
| POST | `/api/move/circle` | Circular move | `{pose_via, pose_to, speed, acceleration, mode}` |
| POST | `/api/move/stop` | Stop motion | `{}` |
| GET | `/api/move/status?task_id=X` | Query move status | - |

**Move Request Body (standard):**
```json
{
  "joints": [double],         // for joint-based moves
  "pose": Pose,               // for Cartesian-based moves
  "speed": double,
  "acceleration": double,
  "time": 0.0,
  "radius": 0.0,
  "asynchronous": true
}
```

**Move Response (`data`):**
- Async: `{task_id: string, status: string, message: string}`
- Sync: `{result: int, message: string}`
- Terminal statuses: `COMPLETED`, `FAILED`, `STOPPED`
- Status polling interval: 200ms (`MOVE_STATUS_INTERVAL_MS`)

### 5.5 Drag/Jogging Endpoints

| Method | Path | Description | Request Body |
|--------|------|-------------|-------------|
| POST | `/api/drag/start` | Start jogging | `{flag, direction, max_speed, max_acceleration}` |
| POST | `/api/drag/stop` | Stop jogging | `{}` |

**Drag Start Body:**
```json
{
  "flag": string,           // "J0"-"J6" or "BASE_X", "FLANGE_YAW", etc.
  "direction": string,      // "POSITIVE" | "NEGATIVE" | "NONE"
  "max_speed": double,
  "max_acceleration": double
}
```

**Flag Values:**
- Joint: `J0`, `J1`, `J2`, `J3`, `J4`, `J5`, `J6`
- Cartesian (per frame):
  - `BASE_X`, `BASE_Y`, `BASE_Z`, `BASE_ROLL`, `BASE_PITCH`, `BASE_YAW`
  - `FLANGE_X`, `FLANGE_Y`, `FLANGE_Z`, `FLANGE_ROLL`, `FLANGE_PITCH`, `FLANGE_YAW`
  - `TOOL_X`, `TOOL_Y`, `TOOL_Z`, `TOOL_ROLL`, `TOOL_PITCH`, `TOOL_YAW`
  - `OBJECT_X`, `OBJECT_Y`, `OBJECT_Z`, `OBJECT_ROLL`, `OBJECT_PITCH`, `OBJECT_YAW`
  - `NULLSPACE`

**Flag Computation (from code):**
```
frame = currentFrame + freedomOffset
if frame >= 400: prefix = "BASE", freedom = frame - 400
if frame >= 300: prefix = "OBJECT", freedom = frame - 300
if frame >= 200: prefix = "FLANGE", freedom = frame - 200
if frame >= 100: prefix = "TOOL", freedom = frame - 100
names = ["X", "Y", "Z", "ROLL", "PITCH", "YAW"]
flag = prefix + "_" + names[freedom]
```

**Joint Jogging Speed:**
```
max_speed = max_jnt_speed_(1.0) * factor
max_acceleration = max_jnt_speed_(1.0) * factor * 5
```

**Cartesian Jogging Speed:**
```
max_speed = max_cart_speed_(0.5) * factor
max_acceleration = max_cart_speed_(0.5) * factor * 5
```

### 5.6 Single Axis Control

| Method | Path | Description |
|--------|------|-------------|
| POST | `/api/axis/single/enable` | Enable single axis `{id}` |
| POST | `/api/axis/single/disable` | Disable single axis `{id}` |
| POST | `/api/axis/single/move` | Move single axis |
| POST | `/api/axis/single/stop` | Stop single axis |
| POST | `/api/axis/multi/enable` | Enable multiple axes |
| POST | `/api/axis/multi/disable` | Disable multiple axes |
| POST | `/api/axis/multi/move` | Move multiple axes |
| POST | `/api/axis/multi/stop` | Stop multiple axes |
| POST | `/api/axis/multi/sync` | Sync multiple axes |

### 5.7 Calibration Endpoints

| Method | Path | Description |
|--------|------|-------------|
| POST | `/api/calibration/pose` | Calibrate pose |
| POST | `/api/calibration/tool` | Calibrate tool |
| POST | `/api/calibration/object` | Calibrate object |
| POST | `/api/calibration/run` | Run calibration |
| GET | `/api/calibration/result` | Get calibration result |

---

## 6. 3D Scene (SceneWidget)

**Files:** `src/SceneWidget.h`, `src/SceneWidget.cpp`

### 6.1 Class Hierarchy
```
SceneWidget : QVTKOpenGLNativeWidget
```

### 6.2 VTK Setup
- **Render window:** `vtkGenericOpenGLRenderWindow`
- **Renderer:** single `vtkRenderer`
- **Mouse interactor:** `MouseInteractorChooseActor` (custom)

### 6.3 Ground Plane
- Source: `vtkPlaneSource`, 10x10 resolution, range -1 to 1
- Color: DeepSkyBlue
- Representation: wireframe
- Actor added to renderer

### 6.4 Background
- Gradient from white (bottom) to `#5279FE` (top)

### 6.5 Camera Defaults
- Position: `(3, 3, 3)`
- Focal Point: `(0, 0, 0.5)`
- View Up: `(0, 0, 1)`

### 6.6 Orientation Marker
- `vtkOrientationMarkerWidget` in bottom-left corner
- Uses `vtkAnnotatedCubeActor` (XYZ-labeled cube)

### 6.7 Logo
- Displays `landau.png` if exists in resources

### 6.8 Frame Rate
- QTimer at 50ms interval (20 FPS)
- Only re-renders when `updatePos` flag is set (dirty flag pattern)

### 6.9 Camera Presets

| View | Camera Position | Focal Point |
|------|----------------|-------------|
| Z-axis (top) | (0, 0, 3) | (0, 0, 0) |
| Y-axis (side) | (0, 3, 0.5) | (0, 0, 0) |
| X-axis (front) | (3, 0, 0.5) | (0, 0, 0) |
| Axo (isometric) | (3, 3, 3) | (0, 0, 0) |

### 6.10 Key Methods
```cpp
void displayModelFromYaml(const std::string& yaml_file);  // Load + render model
void removeRobotModel();                                     // Remove model actors
void setJointPos(std::vector<double>& jntRads);             // Update FK with joint angles
void setJointAxesVisibility(bool isVisible);                // Toggle coordinate axes
void setMeshVisibility(bool isMesh);                        // Toggle wireframe/surface
void setGroundVisibility(bool isVisible);                   // Toggle ground plane
void setTrajVisibility(bool isVisible);                     // Toggle trajectory line
```

---

## 7. Robot Model (Model + Link)

**Files:** `src/Model.h`, `src/Model.cpp`, `src/Link.h`, `src/Link.cpp`

### 7.1 Link Class

**Joint Types (enum):**
```cpp
UNKNOWN  = 0,
REVOLUTE = 1,
CONTINUOUS = 2,
PRISMATIC = 3,
FLOATING = 4,
PLANAR   = 5,
FIXED    = 6
```

**Link Data Members:**
```cpp
int _order;                        // Link ordering index
int _type;                         // Joint type enum
std::string _name;                 // Link name
Eigen::Vector3d translate;         // Translation relative to previous joint [x, y, z]
Eigen::Vector3d rotate;            // RPY rotation relative to previous joint [roll, pitch, yaw]
Eigen::Vector3d angleAxis;         // Joint rotation axis [x, y, z] (unit vector)
Eigen::Vector3d translateLink;     // Additional translation for mesh positioning
Eigen::Vector3d rotateLink;        // Additional rotation for mesh positioning
double angle = 0.0;                // Current joint angle in radians (set at runtime)
```

**VTK Members (per link):**
```cpp
vtkSmartPointer<vtkSTLReader> reader;      // STL file reader
vtkSmartPointer<vtkActor> actor;            // Mesh actor for rendering
vtkSmartPointer<vtkAxesActor> axesActor;    // Joint coordinate frame axes
double axesLength = 0.2;                    // Axes display length
vtkTypeBool isAxesLableShow = 0;           // Show XYZ labels on axes
vtkTypeBool isAxesVisible = 0;             // Axes visibility
```

### 7.2 Model Class

**Data Members:**
```cpp
std::vector<Link> _linkGrp;                 // All links (size = freedom + 1)
int _freedom = 0;                           // Number of movable joints
vtkRenderer* _renderer = nullptr;
vtkSmartPointer<vtkActor> _traj;            // Trajectory line actor
vtkSmartPointer<vtkPoints> _points;         // Trajectory points
bool _isTrajVisible {false};
```

**YAML Parsing (`getModelFromYamlFile`):**
1. Removes previous model actors
2. Loads YAML file, reads `robot` sequence
3. For each link: parses name, order, type, translate, rotate, angleAxis, translateLink, rotateLink, mesh
4. Mesh path resolved relative to YAML file directory
5. Freedom count: starts at link count, decrements for UNKNOWN and FIXED types
6. Creates trajectory actor (color: RGB(73,111,255)/255, line width 3)
7. Adds all link actors to renderer

---

## 8. Forward Kinematics Algorithm

**Location:** `Model::updateModel(std::vector<double> &jointRads)` in `src/Model.cpp`

### 8.1 Algorithm Steps

```
Input: jointRads[] - array of joint angles in radians

1. Assign joint angles:
   For each link i:
     if type is CONTINUOUS or REVOLUTE:
       link[i].angle = jointRads[jnt_id++]
     (UNKNOWN, FIXED, PRISMATIC are skipped)

2. Initialize cumulative transform:
   t_j = identity (Eigen::Affine 4x4)

3. For each link i (0 to N-1):
   a. Apply joint transform:
      t_j *= Translation(translate_i)
      t_j *= RotZ(rotate_i[2]) * RotY(rotate_i[1]) * RotX(rotate_i[0])
      t_j *= AngleAxis(angle_i, angleAxis_i)

   b. Set axes actor transform:
      Convert t_j to vtkMatrix4x4 -> vtkTransform
      link[i].axesActor.setUserTransform(vt1)

   c. Compute link mesh transform:
      t_l = t_j * Translation(translateLink_i)
      t_l *= RotZ(rotateLink_i[2]) * RotY(rotateLink_i[1]) * RotX(rotateLink_i[0])

   d. Set mesh actor transform:
      Convert t_l to vtkMatrix4x4 -> vtkTransform
      link[i].actor.setUserTransform(vt2)
```

### 8.2 Key Mathematical Details

- **Rotation order:** Z-Y-X (intrinsic) for both joint and link rotations
- **Joint transform accumulates** through the chain (left-multiply)
- **Link mesh transform** is derived from joint transform plus link-specific offset
- Uses Eigen `AngleAxisd` for both fixed rotations and variable joint angle
- The `angleAxis` vector defines the rotation axis for revolute joints (typically `[0,0,1]` for Z-axis)

### 8.3 Example: iiwa Link 1
```
translate: [0, 0, 0.15]
rotate: [0, 0, 0]
angleAxis: [0, 0, 1]
translateLink: [0, 0, 0]
rotateLink: [0, 0, 0]

t_j = Translation(0,0,0.15) * RotZ(0) * RotY(0) * RotX(0) * AngleAxis(q1, [0,0,1])
```

---

## 9. Trajectory Recording

**Location:** `Model::updateModel()` in `src/Model.cpp`

### 9.1 Behavior
- When `_isTrajVisible` is true, records the end-effector position
- End-effector position extracted from the **last link's axesActor transform matrix**:
  - `x = matrix.GetElement(0, 3)`
  - `y = matrix.GetElement(1, 3)`
  - `z = matrix.GetElement(2, 3)`

### 9.2 Deduplication
- Before adding a new point, checks if it differs from the last point by more than `1e-6` in all three axes
- Only adds the point if it is sufficiently different

### 9.3 Point Cap
- Maximum 500 points
- FIFO eviction: when exceeding 500, the oldest point is removed (`deleteFirstPoint()`)
- Removal creates a new `vtkPoints` with all points except the first, then shallow-copies

### 9.4 Rendering
- Points rendered as a `vtkPolyLine`
- Color: RGB(73, 111, 255) (blue), line width 3
- PolyData rebuilt on every update (points + polyline cells)

---

## 10. Joint Position Widget

**Files:** `src/JointPositionWidget.h`, `src/JointPositionWidget.cpp`, `src/JointPositionWidget.ui`

### 10.1 Layout
```
[Name Label (45px, bold)] | [Vertical Line] | [- Button | PositionBar | + Button]
```

### 10.2 UI Details
- Name label: fixed width 45px, font "Alibaba PuHuiTi 3.0" bold 16pt
- Buttons: 32x32, autoRepeat enabled
  - autoRepeatDelay: 10ms
  - autoRepeatInterval: 80ms
- PositionBar: expands to fill available space

### 10.3 Data Flow
- `updateJointPosition(double val)`: stores raw radian value, displays `val * r2d` on PositionBar
- `r2d = 180.0/M_PI` by default (radians to degrees)
- PositionBar range: `[-PI*r2d, PI*r2d]` i.e. `[-180, 180]` degrees

### 10.4 Jogging Signal
- Plus button pressed: emits `jointPositionJogging(id, 1)` (positive direction)
- Minus button pressed: emits `jointPositionJogging(id, -1)` (negative direction)
- Any button released: emits `jointPositionJogging(id, 0)` (stop)
- Signal handled by RocosMainWindow -> ConnectDialog::jointJogging()

### 10.5 Angle Representation Toggle
- `setAngleRep(ANGLE_DEGREE)`: r2d = 180/PI (display in degrees)
- `setAngleRep(ANGLE_RADIUS)`: r2d = 1.0 (display in radians)

---

## 11. Cartesian Position Widget

**Files:** `src/CartesianPositionWidget.h`, `src/CartesianPositionWidget.cpp`

### 11.1 Layout
Same as JointPositionWidget: `[Name Label] | [Line] | [- Button | PositionBar | + Button]`

### 11.2 Key Differences from JointPositionWidget
- Default `r2d = 1000.0` (for position axes, displaying meters as mm)
- For angle axes (R/P/Y): `r2d = 180/PI` (set via `setAngleRep()`)
- `step = 0.001` (smaller step for Cartesian)
- `progressStep = 2000`

### 11.3 Display Method
- `updateVal(double val)`: stores raw value, displays `val * r2d` with 3 decimal places on PositionBar

### 11.4 Unit Toggle
- `setPosRep(POS_MM)`: r2d = 1000.0 (display in mm)
- `setPosRep(POS_M)`: r2d = 1.0 (display in meters)

### 11.5 Jogging Signal
- Same pattern as JointPositionWidget
- Signal: `cartesianJogging(freedom, dir)`
- RocosMainWindow adds `currentFrame` offset before forwarding to ConnectDialog

---

## 12. PositionBar (Progress Bar)

**Files:** `src/PositionBar.h`, `src/PositionBar.cpp`

### 12.1 Class
```
PositionBar : QProgressBar
```

### 12.2 Floating-Point Display on Integer Widget
- QProgressBar only supports integer values
- Workaround: internal `FACTOR = 1000` scaling
- `setMinimum(double val)`: calls `QProgressBar::setMinimum(val * 1000)`
- `setMaximum(double val)`: calls `QProgressBar::setMaximum(val * 1000)`
- `setValue(double val)`: stores `posVal = val`, calls `QProgressBar::setValue(val * 1000)`

### 12.3 Display Format
- Format: `"%.3f"` (3 decimal places)
- Shows name and formatted value as progress bar text

### 12.4 Default Range
- Min: `-M_PI` (-3.14159...)
- Max: `M_PI` (3.14159...)
- `minLimit`/`maxLimit` also set to +/-PI (actual limits, not used differently from display range)

---

## 13. Real-time Plotting (PlotWidget)

**Files:** `src/PlotWidget.h`, `src/PlotWidget.cpp`

### 13.1 Class
```
PlotWidget : QWidget
```

### 13.2 Chart Configuration
- Up to 4 `QChartView` widgets in 2x2 `QGridLayout`
- Default layout: 2x2 (`is2x2 = true`)
- Chart theme: BlueIcy
- Font: "Alibaba PuHuiTi 3.0" 11pt
- OpenGL rendering enabled for series

### 13.3 Data Structures
```cpp
QList<QChartView*> plots;                                    // Up to 4 charts
QList<QList<std::function<double(void)>>> funcs;             // Getter lambdas per series per chart
QElapsedTimer* time;                                         // Elapsed time since start
```

### 13.4 Time Series Data
- X-axis: elapsed time in milliseconds
- Sliding window: last 10,000ms (10 seconds)
- Range update: `[now - 10000, now]`

### 13.5 State Update (`handleNewState()`)
- Called on each `newStateComming` signal
- For each chart and each series in that chart:
  - Calls the stored getter lambda to get current value
  - Appends `(elapsed_ms, value)` to the `QLineSeries`

### 13.6 Data Binding (`processTree(QTreeWidget* tree)`)
- Reads PlotConfigDialog's tree structure
- Tree has two levels: Chart (root) -> Data items (children)
- Supported data categories:
  - **Position**: joint position getter (radians)
  - **Velocity**: joint velocity getter
  - **Current**: (mapped to torque getter, likely a mislabel)
  - **Torque**: joint torque getter
- For Position and Velocity: Y-axis range set to `[-PI, PI]` (radians)
- Each data item creates a new series on the selected chart
- Series color auto-assigned by Qt

### 13.7 Legend Interaction
- Click on legend marker toggles series visibility
- Visible: alpha = 1.0
- Hidden: alpha = 0.5

### 13.8 Double-Click
- Toggles between 2x2 grid and single-chart view (maximized)

---

## 14. Plot Configuration Dialog (PlotConfigDialog)

**Files:** `src/PlotConfigDialog.h`, `src/PlotConfigDialog.cpp`

### 14.1 UI Elements
- `plotTree` (QTreeWidget): displays configured plots (chart -> data items)
- `dataTree` (QTreeWidget): available data sources to add
- `addBtn`: adds selected items from dataTree to plotTree
- `removeBtn`: removes selected items from plotTree
- `dirButton`: selects data save directory
- `dataSavePathEdit`: shows selected directory path
- `applyBtn`: accepts dialog, triggers `processTree()` in PlotWidget
- `cancelBtn`: rejects dialog
- `exitButton`: closes dialog

### 14.2 Behavior
- Frameless window, translucent background
- `addBtn`: clones selected data items and adds as children of selected chart
- Auto-expands chart item after adding
- Clears dataTree selection after adding

---

## 15. Model Loader Dialog (ModelLoaderDialog)

**Files:** `src/ModelLoaderDialog.h`, `src/ModelLoaderDialog.cpp`

### 15.1 UI Elements
- `openFileButton`: opens file dialog for YAML files
- `uploadButton`: triggers download from controller
- `removeButton`: removes current robot model
- `exitButton`: closes dialog

### 15.2 Signals
```cpp
void getRobotModel();      // Emitted by uploadButton -> ConnectDialog::getRobotModel()
void removeRobotModel();   // Emitted by removeButton -> SceneWidget::removeRobotModel()
```

### 15.3 File Open
- File filter: `YAML (*.yaml)`
- Default directory: `.` (current working directory)
- On file selected: stores path, accepts dialog
- RocosMainWindow then calls `sceneWidget->displayModelFromYaml(cfgFileName)`

---

## 16. Script Dialog (ScriptDialog)

**Files:** `src/ScriptDialog.h`, `src/ScriptDialog.cpp`

### 16.1 UI Elements
- `scriptComboBox`: dropdown for script paths (default: `/hanbing/app/Line0.MOV`)
- `runButton`: runs selected script
- `stopButton`: stops running script
- `pauseButton`: pauses script
- `continueButton`: resumes paused script

### 16.2 Behavior
- Run adds script path to history (combo box), deduplicates
- Run button disabled during execution
- Re-enabled on stop
- All commands forwarded to ConnectDialog methods (`startScript`, `stopScript`, `pauseScript`, `continueScript`)
- **Note:** Script methods in ConnectDialog are currently stub implementations (empty bodies)

---

## 17. About Dialog (AboutDialog)

**Files:** `src/AboutDialog.h`, `src/AboutDialog.cpp`

- Simple dialog with an Exit button
- No additional logic

---

## 18. Protocol Constants (Protocol.h)

**File:** `src/Protocol.h`

### 18.1 Frame Constants
```cpp
FRAME_JOINT  = 0
FRAME_TOOL   = 100
FRAME_FLANGE = 200
FRAME_OBJECT = 300
FRAME_BASE   = 400
FRAME_WORLD  = 3
```

### 18.2 Freedom Constants
```cpp
FREEDOM_X = 0
FREEDOM_Y = 1
FREEDOM_Z = 2
FREEDOM_R = 3   // Roll
FREEDOM_P = 4   // Pitch
FREEDOM_S = 5   // Yaw (sometimes called S for "swing")
```

### 18.3 Angle/Position Representation
```cpp
ANGLE_DEGREE = 0
ANGLE_RADIUS = 1
POS_MM = 0
POS_M  = 1
```

---

## 19. Robot Model Config Format (YAML)

**Example:** `models/iiwa/config.yaml`

### 19.1 Structure
```yaml
robot:
  - name: link_0
    order: 0
    type: unknown          # Fixed base link
    translate: [0, 0, 0]
    rotate: [0, 0, 0]
    angleAxis: [0, 0, 1]
    mesh: link_0.stl

  - name: link_1
    order: 1
    type: continuous        # Revolute joint
    translate: [0, 0, 0.15]
    rotate: [0, 0, 0]
    angleAxis: [0, 0, 1]
    translateLink: [0, 0, 0]
    rotateLink: [0, 0, 0]
    mesh: link_1.stl
```

### 19.2 Field Descriptions

| Field | Type | Description |
|-------|------|-------------|
| `name` | string | Link name |
| `order` | int | Link ordering in chain |
| `type` | string | `unknown`/`continuous`/`prismatic`/`floating`/`planar`/`fixed` |
| `translate` | [x, y, z] | Translation from previous joint frame |
| `rotate` | [roll, pitch, yaw] | RPY rotation from previous joint frame |
| `angleAxis` | [x, y, z] | Joint rotation axis (unit vector, typically integer 0/1) |
| `translateLink` | [x, y, z] | Additional mesh offset translation |
| `rotateLink` | [roll, pitch, yaw] | Additional mesh offset rotation |
| `mesh` | string | STL file path (relative to config.yaml directory) |

### 19.3 Freedom Calculation
- Links with `type: unknown` or `type: fixed` are not counted as movable joints
- Freedom = total links - (unknown + fixed)
- Joint angle array size = freedom count
- Unknown/fixed links are skipped when consuming joint angles

---

## 20. Existing Model Parameters

### 20.1 iiwa (8 links)
- All joints use `angleAxis: [0, 0, 1]` (Z-axis rotation)
- link_0: type unknown, no translate
- link_1: translate [0, 0, 0.15]
- link_2: translate [0, 0, 0.19], rotate [1.57, 0, 3.14] (90deg X + 180deg Z)
- link_3: translate [0, 0, 0.21]
- link_4: translate [0, 0, 0.21]
- link_5: translate [0, 0, 0.2]
- link_6: translate [0, 0, 0.1]
- link_7: translate [0, 0, 0.098]
- All use `translateLink` to offset mesh positions

### 20.2 ur5e (8 links)
- All joints use `angleAxis: [0, 0, 1]` (Z-axis rotation)
- Different DH parameters from iiwa
- All `rotateLink: [0, 0, 0]`
- No `translateLink` used

### 20.3 talon (7 links)
- **Mixed rotation axes:**
  - Some joints use `angleAxis: [0, 0, 1]` (Z-axis)
  - Some joints use `angleAxis: [0, 1, 0]` (Y-axis)
- No `translateLink` or `rotateLink` defined (default to [0,0,0])
- Demonstrates that angleAxis is not always Z

---

## 21. UI Layout and Styling Details

### 21.1 Color Scheme
- Primary accent: `#496FFF` (ConnectDialog border)
- Secondary accent: `#5279FE` (gradient background)
- Active text highlight: `#EF1288` (frame label in CARTESIAN SPACE title)
- Button background: white
- Button border: `#D8D8D8`
- Button text: `#B3B4B5`
- Button hover: background `#D8D8D8`, text `#262626`
- Button pressed: background `#DFF3FC`, text `#262626`
- Separator lines: `#ECECEC`

### 21.2 Button Styling Pattern
```css
QPushButton {
    background-color: white;
    border: 1px solid #D8D8D8;
    border-radius: 5px;
    color: #B3B4B5;
    font-weight: bold;
}
QPushButton:hover {
    background-color: #D8D8D8;
    color: #262626;
}
QPushButton:pressed, QPushButton:checked {
    background-color: #DFF3FC;
    color: #262626;
    border: 0;
}
```

### 21.3 Font
- Primary: "Alibaba PuHuiTi 3.0" 11pt
- Title labels: "Alibaba PuHuiTi 3.0" bold 20pt
- Joint labels: "Alibaba PuHuiTi 3.0" bold 16pt

### 21.4 Right Panel Stretch Ratios
- Rows: 70 : 70 : 343 : 343 (4 rows)
- Columns: approximately equal split

### 21.5 Joint Spin Boxes (Precise Control)
- Range: -180.000 to +180.000
- Decimals: 3
- Alignment: center

### 21.6 Toolbar Icons
| Button | Icon Resource |
|--------|--------------|
| Connected | connected.png / disconnected.png |
| DispModel | model_loader_grey.png / model_loader_active.png |
| Enabled | switchoff.png / switchon.png |
| AxoView | (view icon) |
| FrontView | (view icon) |
| TopView | (view icon) |
| RightView | (view icon) |
| Plotter | (chart icon) |
| PlotterConfig | (config icon) |
| Script | (script icon) |
| About | (about icon) |
| Axes | coordinates.png |
| Mesh | (mesh icon) |
| Plane | line.png |
| Trajectory | path.png |
| AngleRep | degree.png / radius.png |
| PosRep | mm.png / m.png |
| Logo | landau-rocos-viz.png |

### 21.7 Toggle Buttons (Bottom of Scene)
- Axes: toggles joint coordinate axes visibility
- Mesh: toggles wireframe/surface display
- Plane: toggles ground plane
- Trajectory: toggles trajectory recording/display
- Random: (unclear purpose, likely test button)
