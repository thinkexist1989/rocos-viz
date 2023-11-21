import header_pb2 as _header_pb2
import geometry_pb2 as _geometry_pb2
from google.protobuf import wrappers_pb2 as _wrappers_pb2
from google.protobuf import timestamp_pb2 as _timestamp_pb2
from google.protobuf.internal import containers as _containers
from google.protobuf.internal import enum_type_wrapper as _enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Iterable as _Iterable, Mapping as _Mapping, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class JointState(_message.Message):
    __slots__ = ["name", "position", "velocity", "acceleration", "load", "status", "raw_data"]
    class Status(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
        __slots__ = []
        STATUS_UNSPECIFIED: _ClassVar[JointState.Status]
        STATUS_DISABLED: _ClassVar[JointState.Status]
        STATUS_ENABLED: _ClassVar[JointState.Status]
        STATUS_FAULT: _ClassVar[JointState.Status]
    STATUS_UNSPECIFIED: JointState.Status
    STATUS_DISABLED: JointState.Status
    STATUS_ENABLED: JointState.Status
    STATUS_FAULT: JointState.Status
    NAME_FIELD_NUMBER: _ClassVar[int]
    POSITION_FIELD_NUMBER: _ClassVar[int]
    VELOCITY_FIELD_NUMBER: _ClassVar[int]
    ACCELERATION_FIELD_NUMBER: _ClassVar[int]
    LOAD_FIELD_NUMBER: _ClassVar[int]
    STATUS_FIELD_NUMBER: _ClassVar[int]
    RAW_DATA_FIELD_NUMBER: _ClassVar[int]
    name: str
    position: float
    velocity: float
    acceleration: float
    load: float
    status: JointState.Status
    raw_data: bool
    def __init__(self, name: _Optional[str] = ..., position: _Optional[float] = ..., velocity: _Optional[float] = ..., acceleration: _Optional[float] = ..., load: _Optional[float] = ..., status: _Optional[_Union[JointState.Status, str]] = ..., raw_data: bool = ...) -> None: ...

class FlangeState(_message.Message):
    __slots__ = ["pose"]
    POSE_FIELD_NUMBER: _ClassVar[int]
    pose: _geometry_pb2.SE3Pose
    def __init__(self, pose: _Optional[_Union[_geometry_pb2.SE3Pose, _Mapping]] = ...) -> None: ...

class ToolState(_message.Message):
    __slots__ = ["pose"]
    POSE_FIELD_NUMBER: _ClassVar[int]
    pose: _geometry_pb2.SE3Pose
    def __init__(self, pose: _Optional[_Union[_geometry_pb2.SE3Pose, _Mapping]] = ...) -> None: ...

class BaseState(_message.Message):
    __slots__ = ["pose"]
    POSE_FIELD_NUMBER: _ClassVar[int]
    pose: _geometry_pb2.SE3Pose
    def __init__(self, pose: _Optional[_Union[_geometry_pb2.SE3Pose, _Mapping]] = ...) -> None: ...

class ObjectState(_message.Message):
    __slots__ = ["pose"]
    POSE_FIELD_NUMBER: _ClassVar[int]
    pose: _geometry_pb2.SE3Pose
    def __init__(self, pose: _Optional[_Union[_geometry_pb2.SE3Pose, _Mapping]] = ...) -> None: ...

class RobotState(_message.Message):
    __slots__ = ["joint_states", "hw_state", "flange_state", "tool_state", "base_state", "obj_state"]
    JOINT_STATES_FIELD_NUMBER: _ClassVar[int]
    HW_STATE_FIELD_NUMBER: _ClassVar[int]
    FLANGE_STATE_FIELD_NUMBER: _ClassVar[int]
    TOOL_STATE_FIELD_NUMBER: _ClassVar[int]
    BASE_STATE_FIELD_NUMBER: _ClassVar[int]
    OBJ_STATE_FIELD_NUMBER: _ClassVar[int]
    joint_states: _containers.RepeatedCompositeFieldContainer[JointState]
    hw_state: HardwareState
    flange_state: FlangeState
    tool_state: ToolState
    base_state: BaseState
    obj_state: ObjectState
    def __init__(self, joint_states: _Optional[_Iterable[_Union[JointState, _Mapping]]] = ..., hw_state: _Optional[_Union[HardwareState, _Mapping]] = ..., flange_state: _Optional[_Union[FlangeState, _Mapping]] = ..., tool_state: _Optional[_Union[ToolState, _Mapping]] = ..., base_state: _Optional[_Union[BaseState, _Mapping]] = ..., obj_state: _Optional[_Union[ObjectState, _Mapping]] = ...) -> None: ...

class HardwareState(_message.Message):
    __slots__ = ["hw_type", "timestamp", "min_cycle_time", "max_cycle_time", "current_cycle_time", "slave_num"]
    class HardwareType(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
        __slots__ = []
        HW_TYPE_UNKNOWN: _ClassVar[HardwareState.HardwareType]
        HW_TYPE_SIM: _ClassVar[HardwareState.HardwareType]
        HW_TYPE_ETHERCAT: _ClassVar[HardwareState.HardwareType]
        HW_TYPE_CAN: _ClassVar[HardwareState.HardwareType]
        HW_TYPE_PROFINET: _ClassVar[HardwareState.HardwareType]
    HW_TYPE_UNKNOWN: HardwareState.HardwareType
    HW_TYPE_SIM: HardwareState.HardwareType
    HW_TYPE_ETHERCAT: HardwareState.HardwareType
    HW_TYPE_CAN: HardwareState.HardwareType
    HW_TYPE_PROFINET: HardwareState.HardwareType
    HW_TYPE_FIELD_NUMBER: _ClassVar[int]
    TIMESTAMP_FIELD_NUMBER: _ClassVar[int]
    MIN_CYCLE_TIME_FIELD_NUMBER: _ClassVar[int]
    MAX_CYCLE_TIME_FIELD_NUMBER: _ClassVar[int]
    CURRENT_CYCLE_TIME_FIELD_NUMBER: _ClassVar[int]
    SLAVE_NUM_FIELD_NUMBER: _ClassVar[int]
    hw_type: HardwareState.HardwareType
    timestamp: _timestamp_pb2.Timestamp
    min_cycle_time: float
    max_cycle_time: float
    current_cycle_time: float
    slave_num: int
    def __init__(self, hw_type: _Optional[_Union[HardwareState.HardwareType, str]] = ..., timestamp: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., min_cycle_time: _Optional[float] = ..., max_cycle_time: _Optional[float] = ..., current_cycle_time: _Optional[float] = ..., slave_num: _Optional[int] = ...) -> None: ...

class RobotStateRequest(_message.Message):
    __slots__ = ["header", "raw_data"]
    HEADER_FIELD_NUMBER: _ClassVar[int]
    RAW_DATA_FIELD_NUMBER: _ClassVar[int]
    header: _header_pb2.RequestHeader
    raw_data: bool
    def __init__(self, header: _Optional[_Union[_header_pb2.RequestHeader, _Mapping]] = ..., raw_data: bool = ...) -> None: ...

class RobotStateResponse(_message.Message):
    __slots__ = ["header", "robot_state"]
    HEADER_FIELD_NUMBER: _ClassVar[int]
    ROBOT_STATE_FIELD_NUMBER: _ClassVar[int]
    header: _header_pb2.ResponseHeader
    robot_state: RobotState
    def __init__(self, header: _Optional[_Union[_header_pb2.ResponseHeader, _Mapping]] = ..., robot_state: _Optional[_Union[RobotState, _Mapping]] = ...) -> None: ...
