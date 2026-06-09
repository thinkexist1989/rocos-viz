# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Commands

- `npm run dev` — start the Vite dev server (opens browser, binds `0.0.0.0`).
- `npm run build` — type-check the whole project (`tsc -b`) then produce a production build (`vite build`).
- `npm run preview` — serve the production build locally.

There is no test runner and no `lint` script wired up, even though ESLint and `@typescript-eslint` are installed. To lint manually: `npx eslint src`. Type-checking happens through the build (`tsc -b`).

### Connecting to a robot backend

The dev server proxies the robot controller, so the frontend always talks to same-origin `/api` and `/ws` paths. Point it at a controller with env vars (defaults shown):

```bash
ROBOT_HOST=127.0.0.1 ROBOT_PORT=8080 VITE_PORT=5173 npm run dev
```

In `vite.config.ts`, `/api` is proxied to `http://$ROBOT_HOST:$ROBOT_PORT` and `/ws` to `ws://$ROBOT_HOST:$ROBOT_PORT` (`changeOrigin`/`ws` enabled). Because requests are same-origin and proxied, `RobotApiClient` builds URLs against `window.location.origin` and `RobotWebSocket` against `window.location.host` — they do not dial the robot host directly. The `@` import alias maps to `./src`.

## Architecture

This is a React 18 + TypeScript + Vite single-page app for visualizing and controlling a robot arm in real time. The big picture spans three layers:

### 1. `src/core/` — backend communication and robot math (framework-agnostic, no React)

- **`RobotApiClient.ts`** — REST client for all command/query endpoints (`/api/robot/*`, `/api/move/*`, `/api/drag/*`, `/api/script/*`, `/api/calibration/*`, `/api/robot/model`). Every call goes through a private `request()` that unwraps the `ApiResponse<T>` envelope (`{ success, code, message, data }`) and throws on `success === false`. Motion calls (`moveJ`, `moveL`, `moveJ_IK`, `moveL_FK`) default to `asynchronous: true` and return a `task_id` you poll via `getMoveStatus`.
- **`RobotWebSocket.ts`** — pushes live `RobotState` over `/ws/robot_state`, with auto-reconnect (exponential backoff, capped attempts) and a ping heartbeat.
- **State normalization is duplicated**: both `RobotApiClient.ts` and `RobotWebSocket.ts` carry their own copies of `mapState()` and `mapJointStatus()`. The backend sends inconsistent field names (`flange` vs `flange_pose`, `tool` vs `tool_pose`, `object` vs `object_pose`, `hw_state` vs `hardware`, `load` vs `load_torque`) and numeric joint statuses (0=DISABLED, 1/2=ENABLED, 3=FAULT). These mappers paper over that. **If you change the shape of `RobotState`, update both copies.**
- `ForwardKinematics.ts`, `YamlModelParser.ts`, `ModelDownloader.ts` handle the kinematic chain and loading robot model definitions (YAML config + downloadable meshes via `/api/robot/model/mesh`).
- `types.ts` is the shared contract for everything; `constants.ts` holds load-bearing values: `MAX_JOINTS = 7`, `POLLING_INTERVAL_MS = 20`, `MAX_TRAJECTORY_POINTS = 500`, the jog `DIRECTION`/`FRAME`/`FREEDOM` enums, and `WORK_MODES` (whose labels are Chinese).

**Model load flow (non-obvious):** the controller returns the model as JSON from `getRobotModel()`, then `AppLayout` converts it to YAML via `modelConfigToYaml()` (matching the C++ `writeModelFiles` format) before feeding it to `YamlModelParser` and the `RobotModel` scene component. Meshes are then fetched lazily from `/api/robot/model/mesh`.

### 2. `src/stores/` — Zustand stores are the single source of truth for the UI

State is split by concern: `connectionStore`, `robotStateStore`, `controlStore`, `modelStore`, `plotStore`, `uiStore`. `src/hooks/` (`useRobotConnection`, `useRobotState`, `usePlotDataCollector`) wire the `core/` clients into these stores and into component lifecycles — components read/write stores rather than calling `core/` directly.

### 3. `src/scene/` and `src/components/` — rendering

- **`src/scene/`** renders the robot in 3D with Three.js via `@react-three/fiber` / `@react-three/drei` (`RobotViewer` is the canvas root; `RobotModel`/`LinkComponent` build the arm from the parsed model; plus axes, camera presets, trajectory, helpers).
- **`src/components/`** is the antd-based 2D UI grouped by feature (`control/` jogging + motion commands, `plot/` ECharts realtime charts, `script/` a CodeMirror editor, `connection/`, `model/`, `layout/`, `common/`). `App.tsx` wraps everything in an antd `ConfigProvider` (locale `zh_CN`, custom `theme`) → `AppLayout`.

### UI language

The app is localized to Chinese (`antd/locale/zh_CN`); user-facing strings are Chinese. Match that when adding UI text.
