export const FRAME = {
  JOINT: 0,
  TOOL: 100,
  FLANGE: 200,
  OBJECT: 300,
  BASE: 400,
  WORLD: 3,
} as const;

export type FrameValue = (typeof FRAME)[keyof typeof FRAME];

export const FREEDOM = {
  X: 0,
  Y: 1,
  Z: 2,
  ROLL: 3,
  PITCH: 4,
  YAW: 5,
} as const;

export const FREEDOM_NAMES = ['X', 'Y', 'Z', 'ROLL', 'PITCH', 'YAW'] as const;

export const DIRECTION = {
  POSITIVE: 'POSITIVE',
  NEGATIVE: 'NEGATIVE',
  NONE: 'NONE',
} as const;

export type DirectionValue = (typeof DIRECTION)[keyof typeof DIRECTION];

export const WORK_MODES = [
  { value: 'position', label: '位置模式' },
  { value: 'ee_admit_teach', label: '末端执行器导纳示教' },
  { value: 'jnt_admit_teach', label: '关节导纳示教' },
  { value: 'jnt_imp', label: '关节阻抗' },
  { value: 'cart_imp', label: '笛卡尔阻抗' },
] as const;

export const LINK_TYPE = {
  UNKNOWN: 0,
  REVOLUTE: 1,
  CONTINUOUS: 2,
  PRISMATIC: 3,
  FLOATING: 4,
  PLANAR: 5,
  FIXED: 6,
} as const;

export type LinkTypeValue = (typeof LINK_TYPE)[keyof typeof LINK_TYPE];

export const JOINT_STATUS = {
  DISABLED: 'DISABLED',
  ENABLED: 'ENABLED',
  FAULT: 'FAULT',
} as const;

export const MAX_JOINTS = 7;
export const POLLING_INTERVAL_MS = 20;
export const MAX_STATE_FAILURES = 10;
export const DEFAULT_SPEED_FACTOR = 0.25;
export const MAX_TRAJECTORY_POINTS = 500;

// ─── 运动速度/加速度上限（Speed Scaling 以此为 100% 基准，方便统一修改）───
/** 关节空间最大速度 (rad/s) */
export const MAX_JOINT_SPEED = 1.0;
/** 关节空间最大加速度 (rad/s²)，默认取最大速度的 5 倍 */
export const MAX_JOINT_ACCELERATION = MAX_JOINT_SPEED * 5;
/** 笛卡尔空间最大速度 (m/s) */
export const MAX_CARTESIAN_SPEED = 0.5;
/** 笛卡尔空间最大加速度 (m/s²)，默认取最大速度的 5 倍 */
export const MAX_CARTESIAN_ACCELERATION = MAX_CARTESIAN_SPEED * 5;

/** 按速度比例 (0~1) 计算关节空间运动的 speed / acceleration */
export function jointMotionParams(speedFactor: number): { speed: number; acceleration: number } {
  return {
    speed: speedFactor * MAX_JOINT_SPEED,
    acceleration: speedFactor * MAX_JOINT_ACCELERATION,
  };
}

/** 按速度比例 (0~1) 计算笛卡尔空间运动的 speed / acceleration */
export function cartesianMotionParams(speedFactor: number): { speed: number; acceleration: number } {
  return {
    speed: speedFactor * MAX_CARTESIAN_SPEED,
    acceleration: speedFactor * MAX_CARTESIAN_ACCELERATION,
  };
}
