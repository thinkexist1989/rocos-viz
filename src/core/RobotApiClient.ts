import type {
  ApiResponse,
  RobotInfo,
  RobotState,
  Pose,
  MoveResult,
  TaskStatus,
  RobotModelConfig,
  CalibrationResult,
} from './types';
import { DIRECTION } from './constants';

function mapJointStatus(status: number | string): string {
  if (typeof status === 'number') {
    switch (status) {
      case 0: return 'DISABLED';
      case 1: return 'ENABLED';
      case 2: return 'ENABLED';
      case 3: return 'FAULT';
      default: return 'DISABLED';
    }
  }
  return String(status).toUpperCase();
}

function mapState(raw: any): RobotState {
  const flange = raw.flange ?? raw.flange_pose ?? { position: { x: 0, y: 0, z: 0 }, orientation: { x: 0, y: 0, z: 0, w: 1 } };
  const tool = raw.tool ?? raw.tool_pose ?? { position: { x: 0, y: 0, z: 0 }, orientation: { x: 0, y: 0, z: 0, w: 1 } };
  const objectPose = raw.object ?? raw.object_pose ?? { position: { x: 0, y: 0, z: 0 }, orientation: { x: 0, y: 0, z: 0, w: 1 } };
  const hw = raw.hw_state ?? raw.hardware ?? {};

  return {
    joint_states: (raw.joint_states ?? []).map((j: any) => ({
      name: j.name ?? '',
      position: j.position ?? 0,
      velocity: j.velocity ?? 0,
      acceleration: j.acceleration,
      torque: j.torque ?? 0,
      load_torque: j.load_torque ?? j.load ?? 0,
      load: j.load ?? j.load_torque ?? 0,
      status: mapJointStatus(j.status ?? 0),
    })),
    flange,
    tool,
    object: objectPose,
    hw_state: hw,
  } as RobotState;
}

export class RobotApiClient {
  private baseUrl: string;

  constructor(host: string, port: number | string) {
    // If a host is provided, dial the controller directly at host:port.
    // Otherwise fall back to same-origin (dev server proxies /api to the robot).
    const protocol = window.location.protocol === 'https:' ? 'https:' : 'http:';
    this.baseUrl = host ? `${protocol}//${host}:${port}` : window.location.origin;
  }

  private async request<T>(
    method: string,
    path: string,
    body?: any,
    params?: Record<string, string>,
  ): Promise<T> {
    const url = new URL(path, this.baseUrl);
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

    const response = await fetch(url.toString(), options);
    console.log(`[RobotApiClient] Response status: ${response.status}`);

    const text = await response.text();
    console.log(`[RobotApiClient] Response body length: ${text.length}`);

    if (!text) {
      throw new Error('Empty response from server');
    }

    const json: ApiResponse<T> = JSON.parse(text);
    console.log(`[RobotApiClient] Parsed response success:`, json.success);

    if (!json.success) {
      throw new Error(`API Error: ${json.message} (code: ${json.code})`);
    }

    return json.data;
  }

  async connect(): Promise<RobotInfo> {
    return this.request('GET', '/api/robot/info');
  }

  async disconnect(): Promise<void> {
    return this.request('GET', '/api/robot/disconnect');
  }

  async getRobotState(): Promise<RobotState> {
    const raw = await this.request<any>('GET', '/api/robot/state');
    return mapState(raw);
  }

  async enable(): Promise<void> {
    return this.request('POST', '/api/robot/enable');
  }

  async disable(): Promise<void> {
    return this.request('POST', '/api/robot/disable');
  }

  async isEnabled(): Promise<boolean> {
    const result = await this.request<{ enabled: boolean }>('GET', '/api/robot/enabled');
    return result.enabled;
  }

  async setWorkMode(mode: string): Promise<void> {
    return this.request('POST', '/api/robot/workmode', { mode });
  }

  async moveJ(joints: number[], speed?: number, acceleration?: number): Promise<MoveResult> {
    return this.request('POST', '/api/move/joint', {
      joints,
      speed: speed ?? 0.25,
      acceleration: acceleration ?? 0.25,
      time: 0,
      radius: 0,
      asynchronous: true,
    });
  }

  async moveL(pose: Pose, speed?: number, acceleration?: number): Promise<MoveResult> {
    return this.request('POST', '/api/move/linear', {
      pose,
      speed: speed ?? 0.125,
      acceleration: acceleration ?? 0.125,
      time: 0,
      radius: 0,
      asynchronous: true,
    });
  }

  async moveJ_IK(pose: Pose, speed?: number, acceleration?: number): Promise<MoveResult> {
    return this.request('POST', '/api/move/joint_ik', {
      pose,
      speed: speed ?? 0.25,
      acceleration: acceleration ?? 0.25,
      time: 0,
      radius: 0,
      asynchronous: true,
    });
  }

  async moveL_FK(joints: number[], speed?: number, acceleration?: number): Promise<MoveResult> {
    return this.request('POST', '/api/move/linear_fk', {
      joints,
      speed: speed ?? 0.125,
      acceleration: acceleration ?? 0.125,
      time: 0,
      radius: 0,
      asynchronous: true,
    });
  }

  async stopMove(): Promise<void> {
    return this.request('POST', '/api/move/stop');
  }

  async getMoveStatus(taskId: string): Promise<TaskStatus> {
    return this.request('GET', '/api/move/status', undefined, { task_id: taskId });
  }

  async dragStart(flag: string, direction: typeof DIRECTION[keyof typeof DIRECTION], maxSpeed?: number, maxAcceleration?: number): Promise<void> {
    return this.request('POST', '/api/drag/start', {
      flag,
      direction,
      max_speed: maxSpeed,
      max_acceleration: maxAcceleration,
    });
  }

  async dragStop(): Promise<void> {
    return this.request('POST', '/api/drag/stop');
  }

  async getRobotModel(): Promise<RobotModelConfig> {
    return this.request<RobotModelConfig>('GET', '/api/robot/model');
  }

  async downloadMesh(path: string): Promise<Blob> {
    const url = new URL('/api/robot/model/mesh', this.baseUrl);
    url.searchParams.append('path', path);

    console.log(`[RobotApiClient] downloadMesh: ${url.toString()}`);

    const response = await fetch(url.toString());
    console.log(`[RobotApiClient] downloadMesh status: ${response.status}`);

    return response.blob();
  }

  // ─── URDF endpoints ───────────────────────────────────────────

  /** Upload a URDF file to the controller. Returns the stored path. */
  async uploadUrdf(file: File): Promise<{ path: string }> {
    const url = new URL('/api/robot/urdf', this.baseUrl);
    const formData = new FormData();
    formData.append('file', file);

    console.log(`[RobotApiClient] uploadUrdf: ${url.toString()} (${file.name})`);

    const response = await fetch(url.toString(), {
      method: 'POST',
      body: formData,
    });
    const json: ApiResponse<{ path: string }> = await response.json();
    if (!json.success) {
      throw new Error(`API Error: ${json.message} (code: ${json.code})`);
    }
    return json.data;
  }

  /** Fetch the current URDF file content as an XML string. */
  async getUrdf(): Promise<string> {
    const url = new URL('/api/robot/urdf', this.baseUrl);

    console.log(`[RobotApiClient] getUrdf: ${url.toString()}`);

    const response = await fetch(url.toString(), {
      headers: { Accept: 'application/xml, text/xml, */*' },
    });
    if (!response.ok) {
      throw new Error(`URDF fetch failed: ${response.status}`);
    }
    return response.text();
  }

  /** Download a mesh file referenced by the URDF model. */
  async downloadUrdfMesh(path: string): Promise<Blob> {
    const url = new URL('/api/robot/urdf/mesh', this.baseUrl);
    url.searchParams.append('path', path);

    console.log(`[RobotApiClient] downloadUrdfMesh: ${url.toString()}`);

    const response = await fetch(url.toString());
    console.log(`[RobotApiClient] downloadUrdfMesh status: ${response.status}`);

    return response.blob();
  }

  async runScript(script: string): Promise<void> {
    return this.request('POST', '/api/script/run', { script });
  }

  async stopScript(): Promise<void> {
    return this.request('POST', '/api/script/stop');
  }

  async pauseScript(): Promise<void> {
    return this.request('POST', '/api/script/pause');
  }

  async continueScript(): Promise<void> {
    return this.request('POST', '/api/script/continue');
  }

  async calibratePose(pose: Pose): Promise<void> {
    return this.request('POST', '/api/calibration/pose', { pose });
  }

  async calibrateTool(pose: Pose): Promise<void> {
    return this.request('POST', '/api/calibration/tool', { pose });
  }

  async calibrateObject(pose: Pose): Promise<void> {
    return this.request('POST', '/api/calibration/object', { pose });
  }

  async getCalibrationResult(): Promise<CalibrationResult> {
    return this.request('GET', '/api/calibration/result');
  }
}
