# Requirements Compliance Report - rocos-web

**Reviewed**: 2026-06-09
**Branch**: v2.0-sjc
**Requirements Document**: `/home/sun/Documents/GitHub/rocos-viz/docs/web-app-requirements.md`
**Codebase**: `/home/sun/Documents/GitHub/rocos-viz/rocos-web/`

---

## Summary

- Total functional requirements: 20
- Fully implemented: 11 (55%)
- Partially implemented: 3 (15%)
- Not implemented (placeholder only): 6 (30%)

---

## Compliance Matrix

| ID | Requirement | Status | Details |
|----|------------|--------|---------|
| F1 | Connection Management | PARTIAL | HTTP polling only; no WebSocket as required |
| F2 | 3D Robot Rendering | YES | YAML parser, FK, mesh loading all functional |
| F3 | Joint Jog Control | YES | 7-axis jog with +/- buttons, PositionBar |
| F4 | Cartesian Jog Control | PARTIAL | 6-axis UI present; RPY values hardcoded to 0 |
| F5 | Speed Scaling | YES | Slider 1-999, default 250, stored in Zustand |
| F6 | Work Mode Switching | YES | 5 modes with API call on change |
| F7 | Reference Frame Switching | YES | BASE/FLANGE/TOOL/OBJECT selector |
| F8 | Motion Commands | YES | MoveJ/MoveL/MoveJ_IK/MoveL_FK all wired |
| F9 | Enable/Disable | YES | Toggle button with API calls |
| F10 | Camera Presets | PARTIAL | 4 presets defined; no toolbar buttons to trigger them |
| F11 | Realtime Charts (ECharts) | NO | Placeholder only ("Phase 4 will implement") |
| F12 | Lua Script Editor (CodeMirror) | NO | Placeholder only ("Phase 5 will implement") |
| F13 | Remote Model Download | NO | Placeholder only ("Phase 4 will implement"); no IndexedDB |
| F14 | Trajectory Display | PARTIAL | TrajectoryLine component exists; no data source feeds it |
| F15 | Joint Axes Visualization | YES | AxesIndicator via drei GizmoHelper; toggle wired |
| F16 | Mesh/Wireframe Toggle | YES | Wireframe toggle applied in RobotModel useFrame |
| F17 | Ground Grid Toggle | YES | Grid via drei; toggle wired to uiStore |
| F18 | Unit Toggle (deg/rad, mm/m) | YES | UnitToggle component; isDegree/isMM in controlStore |
| F19 | About Dialog | YES | Modal in AppLayout with version info |
| F20 | Time Display | YES | StatusBar shows current time + running time |

---

## Detailed Analysis

### F1: Connection Management
- **Status**: PARTIALLY IMPLEMENTED
- **Implementation files**:
  - `src/components/connection/ConnectDialog.tsx`
  - `src/stores/connectionStore.ts`
  - `src/hooks/useRobotConnection.ts`
  - `src/core/RobotApiClient.ts`
- **What works**:
  - ConnectDialog: Ant Design Modal with IP/Port inputs, auto-load checkbox, loading state
  - Connection via GET `/api/robot/info` with error handling
  - Zustand store persists host/port to localStorage via `zustand/middleware/persist`
  - HTTP polling at 20ms interval with 10-failure disconnect (exactly as specified)
  - Connection status indicator in StatusBar (green/red dots)
- **Gaps (deviations from requirements)**:
  - **No WebSocket implementation** (Section 6.3 of requirements): The requirements specify a `RobotWebSocket` class connecting to `ws://{host}:{port}/ws/robot_state` at 50Hz with heartbeat, exponential backoff reconnection, and auto-fallback to HTTP polling. The implementation uses only HTTP polling. No WebSocket class exists.
  - **No reconnection logic** for the connection itself: The requirements specify exponential backoff (max 5s, 10 retries) for re-establishing a dropped connection. The current `useRobotConnection` simply stops polling on failure without attempting reconnection.
  - **ConnectDialog does not auto-open with cached address**: The requirements say "check localStorage cached address on startup, pre-fill IP/Port." The dialog opens on startup with default values from the store, but does not auto-connect.
  - **RobotApiClient enable/disable use GET** instead of POST as specified in the requirements API table.

### F2: 3D Robot Rendering
- **Status**: FULLY IMPLEMENTED
- **Implementation files**:
  - `src/scene/RobotViewer.tsx`
  - `src/scene/RobotModel.tsx`
  - `src/scene/SceneHelpers.tsx`
  - `src/scene/AxesIndicator.tsx`
  - `src/scene/CameraPresets.tsx`
  - `src/core/YamlModelParser.ts`
  - `src/core/ForwardKinematics.ts`
  - `src/core/types.ts`
  - `src/core/constants.ts`
- **What works**:
  - R3F Canvas with ACES filmic tone mapping, antialiasing, correct camera defaults (pos [3,3,3], fov 50)
  - YamlModelParser: parses YAML via js-yaml, sorts links by order, builds Object3D hierarchy, applies translate/rotate (ZYX Euler), translateLink/rotateLink, loads meshes
  - Custom STL loader (binary + ASCII) and DAE loader -- no dependency on Three.js examples loaders
  - ForwardKinematics: handles REVOLUTE, CONTINUOUS, PRISMATIC, FIXED/UNKNOWN link types; uses angleAxis from config; applies base rotation + joint rotation with ZYX Euler order
  - OrbitControls with target [0,0,0.5], damping, distance limits
  - Ambient + directional + point lighting
  - Grid via drei with infinite grid, fade, section colors
  - AxesIndicator via drei GizmoHelper (bottom-left)
- **Gaps**:
  - **LinkComponent.tsx** renders a fixed 0.1x0.1x0.1 box regardless of mesh -- this component appears unused (RobotModel loads meshes via YamlModelParser directly). It is dead code.
  - The FK update in `ForwardKinematics.ts` line 42 has a potential bug: `baseRotation.multiply(q)` mutates `baseRotation` in place, meaning the base rotation is consumed on first call and subsequent frames would compound incorrectly. However, since `baseRotation` is created fresh each call, this works correctly in practice.

### F3: Joint Jog Control
- **Status**: FULLY IMPLEMENTED
- **Implementation files**:
  - `src/components/control/JointJogPanel.tsx`
  - `src/components/control/JointJogItem.tsx`
  - `src/components/common/PositionBar.tsx`
- **What works**:
  - Dynamically renders joints from `robotState.joint_states`
  - Each JointJogItem has +/- buttons with mouseDown/mouseUp/mouseLeave and touchStart/touchEnd
  - Uses `dragStart(flag, direction)` and `dragStop()` API calls
  - Flag format: `J0`, `J1`, ... (index-based)
  - PositionBar shows position as percentage within [-3.14, 3.14] range
  - Unit display toggles between deg/rad via isDegree store
  - "Connect robot first" placeholder when no state available
- **Gaps**:
  - PositionBar hardcodes min=-3.14, max=3.14. Requirements say "range determined by joint max position" which should come from RobotInfo.joint_infos. No per-joint range data is used.
  - The `speedFactor` is imported but not passed to `dragStart()` -- the API call only sends flag and direction.

### F4: Cartesian Jog Control
- **Status**: PARTIALLY IMPLEMENTED
- **Implementation files**:
  - `src/components/control/CartesianJogPanel.tsx`
  - `src/components/control/CartesianJogItem.tsx`
- **What works**:
  - 6-axis UI (X, Y, Z, Roll, Pitch, Yaw) with +/- buttons
  - Frame-based pose selection (flange_pose / tool_pose / object_pose)
  - Flag computation: combines frame name + freedom name (e.g., "BASE_X", "TOOL_ROLL")
  - Unit display respects isMM and isDegree toggles
  - Touch event support
- **Gaps**:
  - **Roll, Pitch, Yaw values are hardcoded to 0** (line 69-81 of CartesianJogPanel.tsx). The `RobotState` type stores orientation as quaternion, but there is no RPY conversion. The desktop version reads RPY directly.
  - `speedFactor` is not passed to drag API calls.
  - PositionBar for cartesian uses min=-1, max=1 which is arbitrary and does not represent actual workspace limits.

### F5: Speed Scaling
- **Status**: FULLY IMPLEMENTED
- **Implementation files**:
  - `src/components/control/SpeedSlider.tsx`
  - `src/stores/controlStore.ts`
- **What works**:
  - Slider range 1-999, default value 250 (0.25)
  - Display shows percentage (e.g., "25.0%")
  - Tooltip shows formatted percentage
  - Stored in Zustand controlStore, accessible by all components
  - Default constant `DEFAULT_SPEED_FACTOR = 0.25` defined in constants.ts
- **Gaps**: None significant.

### F6: Work Mode Switching
- **Status**: FULLY IMPLEMENTED
- **Implementation files**:
  - `src/components/control/WorkModeSelector.tsx`
  - `src/core/constants.ts` (WORK_MODES array)
- **What works**:
  - 5 modes: position, ee_admit_teach, jnt_admit_teach, jnt_imp, cart_imp
  - Ant Design Select component
  - POST `/api/robot/workmode` on change
  - Disabled when not connected
  - Success/error message feedback
  - Labels match requirements exactly (Chinese labels)
- **Gaps**: None.

### F7: Reference Frame Switching
- **Status**: FULLY IMPLEMENTED
- **Implementation files**:
  - `src/components/control/FrameSelector.tsx`
  - `src/stores/controlStore.ts`
  - `src/core/constants.ts` (FRAME constants)
- **What works**:
  - 4 frames: BASE (400), FLANGE (200), TOOL (100), OBJECT (300)
  - Frame constants match Protocol.h values from requirements
  - Selection stored in controlStore.currentFrame
  - Used by CartesianJogPanel to select pose data and compute flags
- **Gaps**: None.

### F8: Motion Commands
- **Status**: FULLY IMPLEMENTED
- **Implementation files**:
  - `src/components/control/MotionCommandPanel.tsx`
  - `src/core/RobotApiClient.ts`
- **What works**:
  - Cartesian inputs: x/y/z (mm) + roll/pitch/yaw (deg) with unit conversion
  - Joint inputs: j1-j7 (deg) with unit conversion
  - 4 motion buttons: MoveJ_IK, MoveL, MoveJ, MoveL_FK
  - RPY to Quaternion conversion using THREE.Euler ZYX order then Quaternion.setFromEuler
  - Unit conversion: mm->m and deg->rad applied before sending
  - All disabled when not connected
  - Speed factor included in API calls
- **Gaps**:
  - **No async motion status tracking**: Requirements specify polling `GET /api/move/status?task_id=` after sending motion. The implementation sends the motion command but does not track its completion status. The `RobotApiClient` has a `getMoveStatus()` method but it is never called from the UI.
  - No stop-motion button in the UI (API method `stopMove()` exists in client).

### F9: Enable/Disable
- **Status**: FULLY IMPLEMENTED
- **Implementation files**:
  - `src/components/control/EnableButton.tsx`
  - `src/core/RobotApiClient.ts`
  - `src/stores/connectionStore.ts`
- **What works**:
  - Toggle button: shows "Enabled"/"Enable" with appropriate styling (primary/danger)
  - Calls `client.enable()` / `client.disable()`
  - Updates `isRobotEnabled` in connectionStore
  - Disabled when not connected
  - Success/error message feedback
- **Gaps**:
  - Uses GET instead of POST for enable/disable (per the requirements API table, these should be POST). This matches the current `RobotApiClient` implementation which consistently uses GET.
  - Button has duplicate import: `PoweroffOutlined` imported twice (line 3).

### F10: Camera Presets
- **Status**: PARTIALLY IMPLEMENTED
- **Implementation files**:
  - `src/scene/CameraPresets.tsx`
  - `src/components/layout/AppLayout.tsx` (toolbar buttons)
- **What works**:
  - 4 presets defined with correct positions/targets matching requirements:
    - Axonometric: (3,3,3) -> (0,0,0.5)
    - Top (Z): (0,0,3) -> (0,0,0.5)
    - Front (X): (3,0,0.5) -> (0,0,0.5)
    - Right (Y): (0,3,0.5) -> (0,0,0.5)
  - `setCameraPreset()` function exists for programmatic preset switching
- **Gaps**:
  - **Toolbar buttons are not wired to camera presets**: The 4 camera buttons in AppLayout.tsx (lines 62-73) have no `onClick` handlers. They render icons but do not call `setCameraPreset()`.
  - **No smooth transition animation**: Requirements mention using `useFrame` for smooth camera transitions. The current implementation snaps instantly via `camera.position.set()` + `camera.lookAt()`.
  - CameraPresets component uses module-level mutable variables (`currentPreset`, `onPresetChange`) which is an anti-pattern in React.

### F11: Realtime Charts (ECharts)
- **Status**: NOT IMPLEMENTED
- **Implementation files**:
  - `src/components/plot/PlotPanel.tsx` -- placeholder only
  - `src/stores/plotStore.ts` -- only has chartCount state, no data management
- **What exists**: Dependencies `echarts` and `echarts-for-react` are installed in package.json but not imported or used anywhere.
- **What is needed**:
  - ECharts-based realtime chart component with line charts
  - 2x2 grid layout for 4 charts
  - Data source configuration dialog (tree selector for joint position/velocity/torque/current)
  - 10-second sliding window with ring buffer
  - DataZoom for interaction
  - Large dataset optimization (lttb sampling)
  - Integration with robotStateStore for live data

### F12: Lua Script Editor (CodeMirror)
- **Status**: NOT IMPLEMENTED
- **Implementation files**:
  - `src/components/script/ScriptEditor.tsx` -- placeholder only
- **What exists**: Dependencies `codemirror`, `@codemirror/view`, `@codemirror/legacy-modes`, `@codemirror/theme-one-dark` are installed but unused. Note: `@codemirror/lang-lua` from requirements is not installed; instead `@codemirror/legacy-modes` is installed (which can provide Lua via the legacy StreamParser).
- **What is needed**:
  - CodeMirror 6 editor with Lua syntax highlighting
  - Toolbar: Run, Stop, Pause, Continue buttons
  - Output console
  - API integration: run/stop/pause/continue script endpoints
  - Script path selection dropdown

### F13: Remote Model Download (IndexedDB Caching)
- **Status**: NOT IMPLEMENTED
- **Implementation files**:
  - `src/components/model/ModelLoaderPanel.tsx` -- placeholder only
- **What exists**: `RobotApiClient` has `getRobotModel()` and `downloadMesh()` methods, but they are only used internally by `YamlModelParser`. There is no user-facing model download panel. The `modelStore` has no IndexedDB caching logic.
- **What is needed**:
  - ModelLoaderPanel UI with download progress
  - Fetch model structure from `/api/robot/model`
  - Concurrent mesh download with progress tracking
  - IndexedDB caching layer (cache key: `${baseUrl}_model_${modelName}_${hash}`)
  - Cache invalidation strategy

### F14: Trajectory Display
- **Status**: PARTIALLY IMPLEMENTED
- **Implementation files**:
  - `src/scene/TrajectoryLine.tsx`
  - `src/stores/uiStore.ts` (showTrajectory toggle)
  - `src/core/constants.ts` (MAX_TRAJECTORY_POINTS = 500)
- **What works**:
  - TrajectoryLine component renders a Three.js line with pre-allocated buffer (500 points max)
  - Uses BufferGeometry with setDrawRange for efficient updates
  - Line color matches requirements (#496FFF)
  - Toggle button exists in UI
  - Visibility controlled by showTrajectory from uiStore
- **Gaps**:
  - **No data source**: The component expects `points: THREE.Vector3[]` prop but nothing feeds it end-effector positions. There is no trajectory recording logic.
  - **No Line2 (fat line) support**: Requirements note that Three.js linewidth only supports 1px in WebGL and suggest using Line2. The current implementation uses `lineBasicMaterial` with `linewidth: 2` which will render as 1px on most platforms.
  - `useMemo` on line 21 is used for side effects (updating geometry), which is an anti-pattern -- should be `useEffect`.

### F15: Joint Axes Visualization
- **Status**: FULLY IMPLEMENTED
- **Implementation files**:
  - `src/scene/AxesIndicator.tsx`
  - `src/components/common/DisplayToggles.tsx`
  - `src/stores/uiStore.ts`
- **What works**: AxesIndicator via drei GizmoHelper with RGB axis colors. Toggle button in DisplayToggles. Note: AxesIndicator is rendered both in RobotViewer (always when connected) and via toggle, which may cause duplicate rendering.

### F16: Mesh/Wireframe Toggle
- **Status**: FULLY IMPLEMENTED
- **Implementation files**:
  - `src/scene/RobotModel.tsx` (wireframe applied in useFrame)
  - `src/components/common/DisplayToggles.tsx`
  - `src/stores/uiStore.ts`
- **What works**: Wireframe mode toggled on all mesh materials in the RobotModel useFrame loop. Toggle button in DisplayToggles.

### F17: Ground Grid Toggle
- **Status**: FULLY IMPLEMENTED
- **Implementation files**:
  - `src/scene/SceneHelpers.tsx`
  - `src/scene/RobotViewer.tsx`
  - `src/components/common/DisplayToggles.tsx`
  - `src/stores/uiStore.ts`
- **What works**: Grid rendered via drei `Grid` component with infinite grid, fade, and configurable cell/section sizes. Toggle controls visibility.

### F18: Unit Toggle (deg/rad, mm/m)
- **Status**: FULLY IMPLEMENTED
- **Implementation files**:
  - `src/components/common/UnitToggle.tsx`
  - `src/stores/controlStore.ts`
- **What works**: Two toggle buttons for angle unit (deg/rad) and position unit (mm/m). State stored in controlStore. Used by JointJogItem and CartesianJogItem for display conversion.
- **Gaps**:
  - UnitToggle component exists but is not rendered in the main layout (not imported in AppLayout or RightPanel).

### F19: About Dialog
- **Status**: FULLY IMPLEMENTED
- **Implementation files**:
  - `src/components/layout/AppLayout.tsx` (lines 164-181)
- **What works**: Ant Design Modal showing "ROCOS-Viz Web", version "0.1.0", tech stack, and institution name. Triggered by toolbar "?" button.

### F20: Time Display
- **Status**: FULLY IMPLEMENTED
- **Implementation files**:
  - `src/components/layout/StatusBar.tsx`
- **What works**:
  - Current date/time in Chinese locale format, updated every 500ms
  - Running time (elapsed since component mount) in HH:MM:SS format
  - Connection status indicator (green/red dot)
  - Robot enabled status indicator

---

## Non-Functional Requirements Compliance

### Tech Stack Alignment

| Requirement | Status | Notes |
|-------------|--------|-------|
| React 18 + TypeScript | YES | react ^18.3.1 |
| Vite 5 | YES | vite ^5.0.10 |
| Three.js + R3F + drei | YES | three ^0.160.1, r3f ^8.15.12, drei ^9.88.17 |
| ECharts 5 | PARTIAL | Installed but unused |
| Ant Design 5 | YES | antd ^5.12.8 |
| CodeMirror 6 | PARTIAL | Installed but unused |
| Zustand 4 | YES | zustand ^4.4.7 |
| js-yaml | YES | js-yaml ^4.1.0 |
| gl-matrix | PARTIAL | Installed but unused (Three.js math used instead) |
| Vitest + Playwright | NO | Not installed; no test framework configured |

### Project Structure

| Requirement | Status | Notes |
|-------------|--------|-------|
| `src/core/RobotHttpClient.ts` | PARTIAL | Named `RobotApiClient.ts` instead |
| `src/core/RobotWebSocket.ts` | NO | Does not exist |
| `src/core/StatePoller.ts` | NO | Polling logic is in `hooks/useRobotConnection.ts` instead |
| `src/core/YamlModelParser.ts` | YES | |
| `src/core/ForwardKinematics.ts` | YES | |
| `src/core/constants.ts` | YES | |
| Store files | YES | All 6 stores present |
| Scene components | YES | All specified components present |
| Layout components | YES | All present |
| Control components | YES | All present |
| Hooks | PARTIAL | Only 2 of 6 specified hooks exist (see below) |
| Tests | NO | Empty test directories, no test files |
| `.eslintrc.cjs` | NO | No ESLint config file exists |

### Missing Hooks (per requirements Section 8)

The requirements specify 6 custom hooks. Only 2 are implemented:

| Hook | Status | Notes |
|------|--------|-------|
| `useRobotConnection.ts` | YES | HTTP polling with failure detection |
| `useRobotState.ts` | YES | Provides jointAngles, flangePose, toolPose, hardware |
| `useJointJog.ts` | NO | Logic inlined in JointJogItem component |
| `useCartesianJog.ts` | NO | Logic inlined in CartesianJogItem component |
| `useMotionCommand.ts` | NO | Logic inlined in MotionCommandPanel component |
| `useModelLoader.ts` | NO | Model loading logic in RobotModel component |

### UI Layout & Responsiveness

| Requirement | Status | Notes |
|-------------|--------|-------|
| Left/right split layout | YES | flex: 4 / flex: 6 (40/60) |
| Toolbar | YES | Logo, connect, camera, chart, settings, about |
| Status bar | YES | Connection status, time, running time |
| Desktop Large (>=1400) | YES | 40/60 split |
| Desktop (>=1024) | PARTIAL | Layout exists but no 50/50 breakpoint |
| Tablet (>=768) | PARTIAL | Only 1024px breakpoint exists; stacks vertically |
| Mobile (<768) | NO | No mobile-specific layout or accordion for controls |

### Theme / Dark Mode

| Requirement | Status | Notes |
|-------------|--------|-------|
| Dark theme | YES | Custom dark theme throughout (global.css + Ant Design token config) |
| Theme configuration | YES | Ant Design ConfigProvider with dark colors |
| Light theme toggle | NO | Only dark theme implemented |

---

## Technical Debt & Deviations

### Critical Deviations

1. **WebSocket vs HTTP Polling (major deviation)**: The requirements (Sections 1, 6.3) specify WebSocket as the primary state streaming mechanism with HTTP polling as a fallback. The implementation uses only HTTP polling. At 20ms interval with fetch overhead, this will generate significant network traffic and may cause performance issues. The requirements' WebSocket class with heartbeat (2s), exponential backoff (max 5s, 10 retries), and auto-fallback is entirely absent.

2. **No Reconnection Strategy**: When polling fails 10 times, the connection is simply marked disconnected. There is no automatic reconnection attempt.

3. **Empty Test Suite**: The requirements (Section 9, Phase 5) explicitly call for unit tests (YamlModelParser, ForwardKinematics, RobotApiClient) and E2E tests (connection, control, charts). The `tests/unit/` and `tests/e2e/` directories exist but are completely empty. No test framework (Vitest, Playwright) is configured.

4. **Missing ESLint Config**: The requirements list `.eslintrc.cjs` in the project structure. While ESLint dependencies are in devDependencies, no config file exists.

### Dead Code

1. **`src/components/layout/Toolbar.tsx`**: Not imported anywhere. AppLayout.tsx contains its own inline toolbar. This file uses plain HTML buttons instead of Ant Design.

2. **`src/scene/LinkComponent.tsx`**: Not used by any component. Renders a fixed box geometry regardless of mesh file. RobotModel loads meshes directly via YamlModelParser.

3. **`src/stores/modelStore.ts`**: Defined but never imported or used by any component. Model state is managed entirely within RobotModel component via local refs.

### Architectural Issues

1. **Inlined Component Logic**: Motion commands, jog logic, and model loading are all implemented directly inside UI components rather than in custom hooks as specified. This makes the logic harder to test and reuse.

2. **Hardcoded URLs in AppLayout**: Line 100 of AppLayout.tsx hardcodes `meshBaseUrl="http://localhost:30001/api/robot/model/mesh"` and `modelUrl="/config.yaml"`. These should use the host/port from connectionStore.

3. **API Method Inconsistency**: The requirements API table specifies POST for enable/disable, but `RobotApiClient` uses GET. Similarly, disconnect is GET but should likely be POST.

4. **UnitToggle Not Rendered**: The `UnitToggle` component is implemented but not included in the main layout or RightPanel.

5. **RobotApiClient Not Singleton**: Every component creates a new `RobotApiClient` instance for each operation (see JointJogItem, CartesianJogItem, MotionCommandPanel, etc.) rather than sharing one.

### Missing Features (UI Integration)

1. **Camera preset toolbar buttons have no onClick handlers** (AppLayout.tsx lines 62-73)
2. **No Stop Motion button** in the UI (API method exists)
3. **No script control buttons** (Run/Stop/Pause/Continue) integrated anywhere
4. **No model loader UI** integrated into the toolbar or panels
5. **DisplayToggles component exists but is not rendered** -- toggles are duplicated inline in AppLayout.tsx (lines 117-145)

---

## Recommendations (Priority Order)

### Critical (Block Core Requirements)

1. **Implement WebSocket state streaming**: Create `src/core/RobotWebSocket.ts` with the specified protocol (ws://host:port/ws/robot_state), 50Hz push, heartbeat (2s), exponential backoff reconnection (max 5s, 10 retries), and auto-fallback to HTTP polling. This is specified as the primary communication mechanism and is critical for performance.

2. **Implement reconnection logic**: After MAX_STATE_FAILURES, attempt automatic reconnection with exponential backoff before marking as disconnected.

3. **Fix hardcoded URLs in AppLayout.tsx**: Use host/port from `useConnectionStore` instead of hardcoded `localhost:30001`.

### Important (Required for Feature Completeness)

4. **Implement F11 (ECharts Realtime Charts)**: Build the chart component with 2x2 grid, data source configuration, 10s sliding window, and integration with robotStateStore.

5. **Implement F12 (CodeMirror Script Editor)**: Build the editor with Lua syntax highlighting, control buttons, output console, and API integration.

6. **Implement F13 (Model Download with IndexedDB)**: Build the ModelLoaderPanel with download progress, concurrent mesh fetching, and IndexedDB caching.

7. **Wire camera preset toolbar buttons**: Add onClick handlers that call `setCameraPreset()`.

8. **Implement F14 trajectory data pipeline**: Add end-effector position recording logic that feeds data to TrajectoryLine. Use Line2 for proper linewidth support.

9. **Fix Roll/Pitch/Yaw extraction in CartesianJogPanel**: Convert quaternion orientation to RPY values instead of hardcoding 0.

10. **Add UnitToggle to the RightPanel or toolbar**.

### Nice-to-Have (Quality & Polish)

11. **Extract jog/motion/model logic into custom hooks** as specified (useJointJog, useCartesianJog, useMotionCommand, useModelLoader) for better testability.

12. **Add Vitest and Playwright**: Configure test framework, write unit tests for YamlModelParser, ForwardKinematics, and RobotApiClient. Write E2E tests for connection flow.

13. **Add ESLint config**: Create `.eslintrc.cjs` with TypeScript rules.

14. **Clean up dead code**: Remove Toolbar.tsx, LinkComponent.tsx, modelStore.ts.

15. **Remove duplicate AxesIndicator rendering**: Currently rendered in both RobotViewer (when connected) and via toggle in AppLayout.

16. **Add responsive breakpoints**: Add 768px breakpoint for tablet/mobile with stacked layout and accordion controls.

17. **Add smooth camera transitions**: Use lerp interpolation in useFrame for camera preset switching.

18. **Fix PositionBar range**: Use per-joint min/max from RobotInfo instead of hardcoded [-PI, PI].

19. **Pass speedFactor to drag API calls** in JointJogItem and CartesianJogItem.

20. **Add stop-motion button** to the UI.
