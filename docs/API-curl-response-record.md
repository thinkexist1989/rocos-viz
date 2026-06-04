# ROCOS API curl response record

Base URL: `http://127.0.0.1:8080`

Simulation-mode POST checks were executed against the live controller. Payloads used current pose or zero joint targets where possible.

## Summary

All endpoints in `config/rocos-API.yaml` were checked with `curl --location`. JSON endpoints use the common envelope:

```json
{"success": true, "code": 0, "message": "ok", "data": null}
```

viz parses the envelope in `HttpRobotClient::parseStandardResponse()`. Business data parsers currently exist for robot info, robot state, robot model, move responses, and move status.

## Robot state and information

### GET /api/robot/state

```bash
curl --location 'http://127.0.0.1:8080/api/robot/state'
```

Observed JSON shape:

```json
{
  "success": true,
  "code": 0,
  "message": "ok",
  "data": {
    "joint_states": [{"name": "Slave_1000[ SIM ]", "position": 0.0, "velocity": 0.0, "torque": 0.0, "load_torque": 0.0, "status": 2}],
    "flange": {"position": {"x": -0.1345194964316514, "y": 2.292104308991549e-16, "z": 1.380272011186865}, "orientation": {"x": 0.0, "y": -0.06474656427610355, "z": 1.4376625284940558e-17, "w": 0.9979017398594113}},
    "tool": {"position": {"x": -0.1345194964316514, "y": 2.292104308991549e-16, "z": 1.380272011186865}, "orientation": {"x": 0.0, "y": -0.06474656427610355, "z": 1.4376625284940558e-17, "w": 0.9979017398594113}},
    "object": {"position": {"x": 0.0, "y": 0.0, "z": 0.0}, "orientation": {"x": 0.0, "y": 0.0, "z": 0.0, "w": 1.0}},
    "hw_state": {"hw_type": 1, "current_cycle_time": 1005.52835360831, "min_cycle_time": 990.0175799895297, "max_cycle_time": 1009.9080122230667, "slave_num": 20}
  }
}
```

viz parser: correct. `parseRobotState()` reads current names and keeps fallback support for older `flange_pose/tool_pose/object_pose/hardware`.

### GET /api/robot/info

```bash
curl --location 'http://127.0.0.1:8080/api/robot/info'
```

Observed JSON shape:

```json
{"success": true, "code": 0, "message": "ok", "data": {"joint_infos": [{"name": "Slave_1000[ SIM ]", "cnt_per_unit": 156455.678, "torque_per_unit": 1.0, "ratio": 1.0, "unit_name": "rad", "zero_offset": 0}]}}
```

viz parser: correct. `parseRobotInfo()` reads `unit_name/zero_offset` and older aliases.

### GET /api/robot/model

```bash
curl --location 'http://127.0.0.1:8080/api/robot/model'
```

Observed JSON shape:

```json
{"success": true, "code": 0, "message": "ok", "data": {"name": "talon", "links": [{"name": "base_link", "order": 0, "mesh": "base_link.STL", "translateLink": {"x": 0.0, "y": 0.0, "z": 0.0}, "rotateLink": {"x": 0.0, "y": -0.0, "z": 3.141592653589793}}, {"name": "link_1", "order": 1, "type": "continuous", "mesh": "link_1.STL", "axis": {"x": 0.0, "y": 0.0, "z": 1.0}, "translate": {"x": 0.0, "y": 0.0, "z": 0.348}, "rotate": {"x": 0.0, "y": -0.0, "z": 0.0}, "translateLink": {"x": 0.0, "y": 0.0, "z": 0.0}, "rotateLink": {"x": 0.0, "y": -0.0, "z": 0.0}}]}}
```

viz parser: correct. `getRobotModel()` requires non-empty `links` and reads the link transform fields used to write `models/<robot>/config.yaml`.

### GET /api/robot/model/mesh

```bash
curl --location --head 'http://127.0.0.1:8080/api/robot/model/mesh?path=base_link.STL'
```

Observed response:

```text
HTTP/1.1 200 OK
Content-Type: application/octet-stream
Content-Length: 612084
Content-Disposition: attachment; filename="base_link.STL"
```

viz parser: correct. This is binary, so `downloadFile()` checks status 200 and streams to disk.

## Basic robot control

### GET /api/robot/enabled

```bash
curl --location 'http://127.0.0.1:8080/api/robot/enabled'
```

Observed JSON:

```json
{"success": true, "code": 0, "message": "ok", "data": {"enabled": true}}
```

viz parser: standard envelope only; no business getter currently consumes it.

### POST /api/robot/enable

```bash
curl --location --request POST 'http://127.0.0.1:8080/api/robot/enable'
```

Observed JSON:

```json
{"success": true, "code": 0, "message": "Robot enabled", "data": null}
```

viz parser: correct, standard envelope only.

### POST /api/robot/disable

```bash
curl --location --request POST 'http://127.0.0.1:8080/api/robot/disable'
```

Observed JSON:

```json
{"success": true, "code": 0, "message": "Robot disabled", "data": null}
```

viz parser: correct, standard envelope only.

### POST /api/robot/workmode

```bash
curl --location 'http://127.0.0.1:8080/api/robot/workmode' --header 'Content-Type: application/json' --data '{"mode":"position"}'
```

Observed JSON:

```json
{"success": true, "code": 0, "message": "Work mode set to position", "data": null}
```

viz parser: correct, standard envelope only.

## Motion

Motion commands with `asynchronous:true` returned only `data.task_id`. viz accepts this because `parseMoveResponse()` requires a non-empty `task_id` and treats `status/message` as optional.

### POST /api/move/joint

```bash
curl --location 'http://127.0.0.1:8080/api/move/joint' --header 'Content-Type: application/json' --data '{"joints":[0,0,0,0,0,0,0],"speed":0.25,"acceleration":0.25,"time":0,"radius":0,"asynchronous":true}'
```

Observed JSON:

```json
{"success": true, "code": 0, "message": "MoveJ started", "data": {"task_id": "task_1"}}
```

viz parser: correct.

### POST /api/move/joint_ik

```bash
curl --location 'http://127.0.0.1:8080/api/move/joint_ik' --header 'Content-Type: application/json' --data '{"pose":{"position":{"x":-0.1345194964316514,"y":2.292104308991549e-16,"z":1.380272011186865},"orientation":{"x":0,"y":-0.06474656427610355,"z":1.4376625284940558e-17,"w":0.9979017398594113}},"speed":0.25,"acceleration":0.25,"time":0,"radius":0,"asynchronous":true}'
```

Observed JSON:

```json
{"success": true, "code": 0, "message": "MoveJ_IK started", "data": {"task_id": "task_2"}}
```

viz parser: correct.

### POST /api/move/linear

```bash
curl --location 'http://127.0.0.1:8080/api/move/linear' --header 'Content-Type: application/json' --data '{"pose":{"position":{"x":-0.1345194964316514,"y":2.292104308991549e-16,"z":1.380272011186865},"orientation":{"x":0,"y":-0.06474656427610355,"z":1.4376625284940558e-17,"w":0.9979017398594113}},"speed":0.125,"acceleration":0.125,"time":0,"radius":0,"asynchronous":true}'
```

Observed JSON:

```json
{"success": true, "code": 0, "message": "MoveL started", "data": {"task_id": "task_3"}}
```

viz parser: correct.

### POST /api/move/linear_fk

```bash
curl --location 'http://127.0.0.1:8080/api/move/linear_fk' --header 'Content-Type: application/json' --data '{"joints":[0,0,0,0,0,0,0],"speed":0.125,"acceleration":0.125,"time":0,"radius":0,"asynchronous":true}'
```

Observed JSON:

```json
{"success": true, "code": 0, "message": "MoveL_FK started", "data": {"task_id": "task_4"}}
```

viz parser: correct.

### POST /api/move/circle

```bash
curl --location 'http://127.0.0.1:8080/api/move/circle' --header 'Content-Type: application/json' --data '{"pose_via":{"position":{"x":-0.1345194964316514,"y":2.292104308991549e-16,"z":1.380272011186865},"orientation":{"x":0,"y":-0.06474656427610355,"z":1.4376625284940558e-17,"w":0.9979017398594113}},"pose_to":{"position":{"x":-0.1345194964316514,"y":2.292104308991549e-16,"z":1.380272011186865},"orientation":{"x":0,"y":-0.06474656427610355,"z":1.4376625284940558e-17,"w":0.9979017398594113}},"speed":0.125,"acceleration":0.125,"time":0,"radius":0,"mode":"UNCONSTRAINED","asynchronous":true}'
```

Observed JSON:

```json
{"success": true, "code": 0, "message": "MoveC started", "data": {"task_id": "task_5"}}
```

viz parser: not currently called by `ConnectDialog`; JSON shape matches `MoveResponse`.

### POST /api/move/path

```bash
curl --location 'http://127.0.0.1:8080/api/move/path' --header 'Content-Type: application/json' --data '{"pose":{"position":{"x":-0.1345194964316514,"y":2.292104308991549e-16,"z":1.380272011186865},"orientation":{"x":0,"y":-0.06474656427610355,"z":1.4376625284940558e-17,"w":0.9979017398594113}},"speed":0.125,"acceleration":0.125,"time":0,"radius":0,"asynchronous":true}'
```

Observed JSON:

```json
{"success": true, "code": 0, "message": "MoveP started", "data": {"task_id": "task_6"}}
```

viz parser: not currently called by `ConnectDialog`; JSON shape matches `MoveResponse`.

### POST /api/move/pathway

```bash
curl --location 'http://127.0.0.1:8080/api/move/pathway' --header 'Content-Type: application/json' --data '{"waypoints":[{"pose":{"position":{"x":-0.1345194964316514,"y":2.292104308991549e-16,"z":1.380272011186865},"orientation":{"x":0,"y":-0.06474656427610355,"z":1.4376625284940558e-17,"w":0.9979017398594113}},"speed":0.125,"acceleration":0.125}],"asynchronous":true}'
```

Observed JSON:

```json
{"success": false, "code": 501, "message": "MovePath not implemented: Robot::Path has private constructors, needs Robot class API extension", "data": null}
```

viz parser: not currently called by `ConnectDialog`; standard envelope correctly reports failure.

### POST /api/move/stop

```bash
curl --location --request POST 'http://127.0.0.1:8080/api/move/stop'
```

Observed JSON:

```json
{"success": true, "code": 0, "message": "Motion stopped", "data": null}
```

viz parser: correct, standard envelope only.

### GET /api/move/status

```bash
curl --location 'http://127.0.0.1:8080/api/move/status?task_id=task_1'
```

Observed JSON:

```json
{"success": true, "code": 0, "message": "ok", "data": {"task_id": "task_1", "type": "MoveJ", "status": "RUNNING", "result": 0, "message": ""}}
```

Other observed statuses included `FAILED` with `result:-1` for tasks whose target pose was invalid. viz parser: correct.

## Axis control

### POST /api/axis/single/enable

```bash
curl --location 'http://127.0.0.1:8080/api/axis/single/enable' --header 'Content-Type: application/json' --data '{"id":0}'
```

Observed JSON: `{"success":true,"code":0,"message":"Joint 0 enabled","data":null}`. viz parser: correct, standard envelope only.

### POST /api/axis/single/disable

```bash
curl --location 'http://127.0.0.1:8080/api/axis/single/disable' --header 'Content-Type: application/json' --data '{"id":0}'
```

Observed JSON: `{"success":true,"code":0,"message":"Joint 0 disabled","data":null}`. viz parser: correct, standard envelope only.

### POST /api/axis/single/move

```bash
curl --location 'http://127.0.0.1:8080/api/axis/single/move' --header 'Content-Type: application/json' --data '{"id":0,"pos":0,"max_vel":-1,"max_acc":-1,"max_jerk":-1,"least_time":-1,"raw_data":false}'
```

Observed JSON: `{"success":true,"code":0,"message":"Joint 0 move started","data":null}`. viz parser: not currently called by `ConnectDialog`; standard envelope only.

### POST /api/axis/single/stop

```bash
curl --location 'http://127.0.0.1:8080/api/axis/single/stop' --header 'Content-Type: application/json' --data '{"id":0}'
```

Observed JSON: `{"success":true,"code":0,"message":"Joint 0 stopped","data":null}`. viz parser: not currently called by `ConnectDialog`; standard envelope only.

### POST /api/axis/multi/enable

```bash
curl --location --request POST 'http://127.0.0.1:8080/api/axis/multi/enable'
```

Observed JSON: `{"success":true,"code":0,"message":"All joints enabled","data":null}`. viz parser: not currently called by `ConnectDialog`; standard envelope only.

### POST /api/axis/multi/disable

```bash
curl --location --request POST 'http://127.0.0.1:8080/api/axis/multi/disable'
```

Observed JSON: `{"success":true,"code":0,"message":"All joints disabled","data":null}`. viz parser: not currently called by `ConnectDialog`; standard envelope only.

### POST /api/axis/multi/move

```bash
curl --location 'http://127.0.0.1:8080/api/axis/multi/move' --header 'Content-Type: application/json' --data '{"target_pos":[0,0,0,0,0,0,0],"max_vel":[0.1,0.1,0.1,0.1,0.1,0.1,0.1],"max_acc":[0.5,0.5,0.5,0.5,0.5,0.5,0.5],"max_jerk":[1,1,1,1,1,1,1],"least_time":-1}'
```

Observed JSON: `{"success":true,"code":0,"message":"Multi-axis move started","data":null}`. viz parser: not currently called by `ConnectDialog`; standard envelope only.

### POST /api/axis/multi/stop

```bash
curl --location --request POST 'http://127.0.0.1:8080/api/axis/multi/stop'
```

Observed JSON: `{"success":true,"code":0,"message":"All axes stopped","data":null}`. viz parser: not currently called by `ConnectDialog`; standard envelope only.

### POST /api/axis/multi/sync

```bash
curl --location 'http://127.0.0.1:8080/api/axis/multi/sync' --header 'Content-Type: application/json' --data '{"sync":"none"}'
```

Observed JSON: `{"success":true,"code":0,"message":"Synchronization set to none","data":null}`. viz parser: not currently called by `ConnectDialog`; standard envelope only.

## Drag teach

### POST /api/drag/start

```bash
curl --location 'http://127.0.0.1:8080/api/drag/start' --header 'Content-Type: application/json' --data '{"flag":"J0","direction":"POSITIVE","max_speed":0.05,"max_acceleration":0.25}'
```

Observed JSON: `{"success":true,"code":0,"message":"Dragging started","data":null}`. viz parser: correct, standard envelope only.

### POST /api/drag/stop

```bash
curl --location --request POST 'http://127.0.0.1:8080/api/drag/stop'
```

Observed JSON: `{"success":true,"code":0,"message":"Dragging stopped","data":null}`. viz parser: correct, standard envelope only.

## Calibration

### POST /api/calibration/pose

```bash
curl --location 'http://127.0.0.1:8080/api/calibration/pose' --header 'Content-Type: application/json' --data '{"id":0,"pose":{"position":{"x":-0.1345194964316514,"y":2.292104308991549e-16,"z":1.380272011186865},"orientation":{"x":0,"y":-0.06474656427610355,"z":1.4376625284940558e-17,"w":0.9979017398594113}}}'
```

Observed JSON: `{"success":true,"code":0,"message":"Pose frame 0 set","data":null}`. viz parser: not currently called by `ConnectDialog`; standard envelope only.

### POST /api/calibration/tool

```bash
curl --location 'http://127.0.0.1:8080/api/calibration/tool' --header 'Content-Type: application/json' --data '{"pose":{"position":{"x":-0.1345194964316514,"y":2.292104308991549e-16,"z":1.380272011186865},"orientation":{"x":0,"y":-0.06474656427610355,"z":1.4376625284940558e-17,"w":0.9979017398594113}}}'
```

Observed JSON: `{"success":true,"code":0,"message":"Tool frame set","data":null}`. viz parser: not currently called by `ConnectDialog`; standard envelope only.

### POST /api/calibration/object

```bash
curl --location 'http://127.0.0.1:8080/api/calibration/object' --header 'Content-Type: application/json' --data '{"pose":{"position":{"x":-0.1345194964316514,"y":2.292104308991549e-16,"z":1.380272011186865},"orientation":{"x":0,"y":-0.06474656427610355,"z":1.4376625284940558e-17,"w":0.9979017398594113}}}'
```

Observed JSON: `{"success":true,"code":0,"message":"Object frame set","data":null}`. viz parser: not currently called by `ConnectDialog`; standard envelope only.

### POST /api/calibration/run

```bash
curl --location 'http://127.0.0.1:8080/api/calibration/run' --header 'Content-Type: application/json' --data '{"frame":"tool"}'
```

Observed JSON:

```json
{"success": false, "code": 500, "message": "Calibration failed", "data": null}
```

viz parser: not currently called by `ConnectDialog`; standard envelope correctly reports failure.

### GET /api/calibration/result

```bash
curl --location 'http://127.0.0.1:8080/api/calibration/result'
```

Observed success-state JSON:

```json
{"success": true, "code": 0, "message": "ok", "data": {"error_state": false, "pose": {"position": {"x": 0.0, "y": 0.0, "z": 0.0}, "orientation": {"x": 0.0, "y": 0.0, "z": 0.0, "w": 1.0}}}}
```

Observed error-state JSON:

```json
{"success": true, "code": 0, "message": "ok", "data": {"error_state": true, "pose": {"position": {"x": null, "y": null, "z": null}, "orientation": {"x": 0.0, "y": 0.0, "z": 0.5, "w": 0.0}}}}
```

viz parser: no business parser currently consumes this endpoint; standard envelope accepts it.
