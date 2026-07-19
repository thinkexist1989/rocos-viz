export interface Vector3D {
  x: number;
  y: number;
  z: number;
}

export interface Quaternion {
  x: number;
  y: number;
  z: number;
  w: number;
}

export interface Pose {
  position: Vector3D;
  orientation: Quaternion;
}

export interface JointState {
  name: string;
  position: number;
  velocity: number;
  acceleration?: number;
  torque: number;
  load_torque?: number;
  load?: number;
  status: number | string;
}

export interface HardwareInfo {
  hw_type?: number;
  current_cycle_time?: number;
  min_cycle_time?: number;
  max_cycle_time?: number;
  slave_num?: number;
}

export interface RobotState {
  joint_states: JointState[];
  flange?: Pose;
  flange_pose?: Pose;
  tool?: Pose;
  tool_pose?: Pose;
  object?: Pose;
  object_pose?: Pose;
  hw_state?: HardwareInfo;
  hardware?: HardwareInfo;
  is_enabled?: boolean;
  robot_state?: string;
}

export interface JointInfo {
  name: string;
  cnt_per_unit: number;
  torque_per_unit: number;
  ratio: number;
  zero_offset: number;
  unit_name: string;
}

export interface RobotInfo {
  joint_infos: JointInfo[];
}

export interface LinkConfig {
  name: string;
  order: number;
  type?: string;
  mesh: string;
  axis?: Vector3D;
  translate?: [number, number, number] | Vector3D;
  rotate?: [number, number, number] | Vector3D;
  angleAxis?: [number, number, number] | Vector3D;
  translateLink?: [number, number, number] | Vector3D;
  rotateLink?: [number, number, number] | Vector3D;
}

export interface RobotModelConfig {
  name: string;
  links: LinkConfig[];
}

export interface MoveResult {
  task_id?: string;
  status?: string;
  result: number;
  message: string;
}

export interface TaskStatus {
  task_id: string;
  type: string;
  status: 'RUNNING' | 'COMPLETED' | 'FAILED' | 'STOPPED';
  result: any;
  message: string;
  create_time: string;
  finish_time: string;
}

export interface ApiResponse<T = any> {
  success: boolean;
  code: number;
  message: string;
  data: T;
}

export interface CalibrationResult {
  error_state: boolean;
  pose: Pose;
}
