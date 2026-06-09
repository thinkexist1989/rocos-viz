import type { RobotState } from './types';

const WS_STATE_PATH = '/ws/robot_state';
const HEARTBEAT_INTERVAL_MS = 2000;
const MAX_RECONNECT_DELAY_MS = 5000;
const INITIAL_RECONNECT_DELAY_MS = 1000;
const MAX_RECONNECT_ATTEMPTS = 10;

type StateCallback = (state: RobotState) => void;
type StatusCallback = (status: 'connecting' | 'connected' | 'disconnected') => void;

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

export class RobotWebSocket {
  private ws: WebSocket | null = null;
  private reconnectTimer: ReturnType<typeof setTimeout> | null = null;
  private heartbeatTimer: ReturnType<typeof setInterval> | null = null;
  private reconnectDelay = INITIAL_RECONNECT_DELAY_MS;
  private reconnectAttempts = 0;
  private host = '';
  private port = 0;
  private onState: StateCallback | null = null;
  private onStatus: StatusCallback | null = null;
  private _connected = false;
  private _closed = false;

  get connected() {
    return this._connected;
  }

  connect(
    host: string,
    port: number,
    onState: StateCallback,
    onStatus?: StatusCallback | null,
  ) {
    this.host = host;
    this.port = port;
    this.onState = onState;
    this.onStatus = onStatus ?? null;
    this._closed = false;
    this.reconnectAttempts = 0;
    this.reconnectDelay = INITIAL_RECONNECT_DELAY_MS;
    this.open();
  }

  private open() {
    if (this._closed) return;

    this.cleanup();
    this.onStatus?.('connecting');

    const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
    const wsUrl = `${protocol}//${window.location.host}${WS_STATE_PATH}`;

    try {
      this.ws = new WebSocket(wsUrl);
    } catch {
      this.scheduleReconnect();
      return;
    }

    this.ws.onopen = () => {
      this._connected = true;
      this.reconnectAttempts = 0;
      this.reconnectDelay = INITIAL_RECONNECT_DELAY_MS;
      this.onStatus?.('connected');
      this.startHeartbeat();
    };

    this.ws.onmessage = (event) => {
      try {
        const raw = JSON.parse(event.data);
        const state = raw.data ? mapState(raw.data) : mapState(raw);
        this.onState?.(state);
      } catch {
        // Ignore malformed messages
      }
    };

    this.ws.onclose = () => {
      this.stopHeartbeat();
      this._connected = false;
      this.onStatus?.('disconnected');
      if (!this._closed) {
        this.scheduleReconnect();
      }
    };

    this.ws.onerror = () => {
      this.ws?.close();
    };
  }

  private scheduleReconnect() {
    if (this._closed) return;
    if (this.reconnectAttempts >= MAX_RECONNECT_ATTEMPTS) {
      console.error('Max WebSocket reconnect attempts reached');
      return;
    }

    this.reconnectAttempts++;
    this.reconnectTimer = setTimeout(() => {
      this.reconnectDelay = Math.min(this.reconnectDelay * 2, MAX_RECONNECT_DELAY_MS);
      this.open();
    }, this.reconnectDelay);
  }

  private startHeartbeat() {
    this.heartbeatTimer = setInterval(() => {
      if (this.ws?.readyState === WebSocket.OPEN) {
        try {
          this.ws.send(JSON.stringify({ type: 'ping' }));
        } catch {
          this.ws?.close();
        }
      }
    }, HEARTBEAT_INTERVAL_MS);
  }

  private stopHeartbeat() {
    if (this.heartbeatTimer) {
      clearInterval(this.heartbeatTimer);
      this.heartbeatTimer = null;
    }
  }

  private cleanup() {
    this.stopHeartbeat();
    if (this.reconnectTimer) {
      clearTimeout(this.reconnectTimer);
      this.reconnectTimer = null;
    }
    if (this.ws) {
      this.ws.onopen = null;
      this.ws.onmessage = null;
      this.ws.onclose = null;
      this.ws.onerror = null;
      if (this.ws.readyState === WebSocket.OPEN || this.ws.readyState === WebSocket.CONNECTING) {
        this.ws.close();
      }
      this.ws = null;
    }
  }

  disconnect() {
    this._closed = true;
    this.cleanup();
    this._connected = false;
  }
}
