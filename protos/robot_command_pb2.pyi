import header_pb2 as _header_pb2
import geometry_pb2 as _geometry_pb2
from google.protobuf import wrappers_pb2 as _wrappers_pb2
from google.protobuf.internal import containers as _containers
from google.protobuf.internal import enum_type_wrapper as _enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Iterable as _Iterable, Mapping as _Mapping, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class Synchronization(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
    __slots__ = []
    SYNC_NONE: _ClassVar[Synchronization]
    SYNC_TIME: _ClassVar[Synchronization]
    SYNC_PHASE: _ClassVar[Synchronization]

class Mode(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
    __slots__ = []
    MODE_CSP: _ClassVar[Mode]
    MODE_CSV: _ClassVar[Mode]
    MODE_CST: _ClassVar[Mode]

class WorkMode(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
    __slots__ = []
    Position: _ClassVar[WorkMode]
    EeAdmitTeach: _ClassVar[WorkMode]
    JntAdmitTeach: _ClassVar[WorkMode]
    JntImp: _ClassVar[WorkMode]
    CartImp: _ClassVar[WorkMode]

class DraggingFlag(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
    __slots__ = []
    J0: _ClassVar[DraggingFlag]
    J1: _ClassVar[DraggingFlag]
    J2: _ClassVar[DraggingFlag]
    J3: _ClassVar[DraggingFlag]
    J4: _ClassVar[DraggingFlag]
    J5: _ClassVar[DraggingFlag]
    J6: _ClassVar[DraggingFlag]
    TOOL_X: _ClassVar[DraggingFlag]
    TOOL_Y: _ClassVar[DraggingFlag]
    TOOL_Z: _ClassVar[DraggingFlag]
    TOOL_ROLL: _ClassVar[DraggingFlag]
    TOOL_PITCH: _ClassVar[DraggingFlag]
    TOOL_YAW: _ClassVar[DraggingFlag]
    FLANGE_X: _ClassVar[DraggingFlag]
    FLANGE_Y: _ClassVar[DraggingFlag]
    FLANGE_Z: _ClassVar[DraggingFlag]
    FLANGE_ROLL: _ClassVar[DraggingFlag]
    FLANGE_PITCH: _ClassVar[DraggingFlag]
    FLANGE_YAW: _ClassVar[DraggingFlag]
    OBJECT_X: _ClassVar[DraggingFlag]
    OBJECT_Y: _ClassVar[DraggingFlag]
    OBJECT_Z: _ClassVar[DraggingFlag]
    OBJECT_ROLL: _ClassVar[DraggingFlag]
    OBJECT_PITCH: _ClassVar[DraggingFlag]
    OBJECT_YAW: _ClassVar[DraggingFlag]
    BASE_X: _ClassVar[DraggingFlag]
    BASE_Y: _ClassVar[DraggingFlag]
    BASE_Z: _ClassVar[DraggingFlag]
    BASE_ROLL: _ClassVar[DraggingFlag]
    BASE_PITCH: _ClassVar[DraggingFlag]
    BASE_YAW: _ClassVar[DraggingFlag]

class DraggingDirection(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
    __slots__ = []
    NONE: _ClassVar[DraggingDirection]
    POSITIVE: _ClassVar[DraggingDirection]
    NEGATIVE: _ClassVar[DraggingDirection]
SYNC_NONE: Synchronization
SYNC_TIME: Synchronization
SYNC_PHASE: Synchronization
MODE_CSP: Mode
MODE_CSV: Mode
MODE_CST: Mode
Position: WorkMode
EeAdmitTeach: WorkMode
JntAdmitTeach: WorkMode
JntImp: WorkMode
CartImp: WorkMode
J0: DraggingFlag
J1: DraggingFlag
J2: DraggingFlag
J3: DraggingFlag
J4: DraggingFlag
J5: DraggingFlag
J6: DraggingFlag
TOOL_X: DraggingFlag
TOOL_Y: DraggingFlag
TOOL_Z: DraggingFlag
TOOL_ROLL: DraggingFlag
TOOL_PITCH: DraggingFlag
TOOL_YAW: DraggingFlag
FLANGE_X: DraggingFlag
FLANGE_Y: DraggingFlag
FLANGE_Z: DraggingFlag
FLANGE_ROLL: DraggingFlag
FLANGE_PITCH: DraggingFlag
FLANGE_YAW: DraggingFlag
OBJECT_X: DraggingFlag
OBJECT_Y: DraggingFlag
OBJECT_Z: DraggingFlag
OBJECT_ROLL: DraggingFlag
OBJECT_PITCH: DraggingFlag
OBJECT_YAW: DraggingFlag
BASE_X: DraggingFlag
BASE_Y: DraggingFlag
BASE_Z: DraggingFlag
BASE_ROLL: DraggingFlag
BASE_PITCH: DraggingFlag
BASE_YAW: DraggingFlag
NONE: DraggingDirection
POSITIVE: DraggingDirection
NEGATIVE: DraggingDirection

class SingleAxisEnabled(_message.Message):
    __slots__ = ["id"]
    ID_FIELD_NUMBER: _ClassVar[int]
    id: int
    def __init__(self, id: _Optional[int] = ...) -> None: ...

class SingleAxisDisabled(_message.Message):
    __slots__ = ["id"]
    ID_FIELD_NUMBER: _ClassVar[int]
    id: int
    def __init__(self, id: _Optional[int] = ...) -> None: ...

class SingleAxisMode(_message.Message):
    __slots__ = ["id", "value"]
    ID_FIELD_NUMBER: _ClassVar[int]
    VALUE_FIELD_NUMBER: _ClassVar[int]
    id: int
    value: Mode
    def __init__(self, id: _Optional[int] = ..., value: _Optional[_Union[Mode, str]] = ...) -> None: ...

class SingleAxisMove(_message.Message):
    __slots__ = ["id", "pos", "max_vel", "max_acc", "max_jerk", "least_time", "raw_data"]
    ID_FIELD_NUMBER: _ClassVar[int]
    POS_FIELD_NUMBER: _ClassVar[int]
    MAX_VEL_FIELD_NUMBER: _ClassVar[int]
    MAX_ACC_FIELD_NUMBER: _ClassVar[int]
    MAX_JERK_FIELD_NUMBER: _ClassVar[int]
    LEAST_TIME_FIELD_NUMBER: _ClassVar[int]
    RAW_DATA_FIELD_NUMBER: _ClassVar[int]
    id: int
    pos: float
    max_vel: float
    max_acc: float
    max_jerk: float
    least_time: float
    raw_data: bool
    def __init__(self, id: _Optional[int] = ..., pos: _Optional[float] = ..., max_vel: _Optional[float] = ..., max_acc: _Optional[float] = ..., max_jerk: _Optional[float] = ..., least_time: _Optional[float] = ..., raw_data: bool = ...) -> None: ...

class SingleAxisStop(_message.Message):
    __slots__ = ["id"]
    ID_FIELD_NUMBER: _ClassVar[int]
    id: int
    def __init__(self, id: _Optional[int] = ...) -> None: ...

class SingleAxisCommand(_message.Message):
    __slots__ = ["enabled", "disabled", "mode", "move", "stop"]
    ENABLED_FIELD_NUMBER: _ClassVar[int]
    DISABLED_FIELD_NUMBER: _ClassVar[int]
    MODE_FIELD_NUMBER: _ClassVar[int]
    MOVE_FIELD_NUMBER: _ClassVar[int]
    STOP_FIELD_NUMBER: _ClassVar[int]
    enabled: SingleAxisEnabled
    disabled: SingleAxisDisabled
    mode: SingleAxisMode
    move: SingleAxisMove
    stop: SingleAxisStop
    def __init__(self, enabled: _Optional[_Union[SingleAxisEnabled, _Mapping]] = ..., disabled: _Optional[_Union[SingleAxisDisabled, _Mapping]] = ..., mode: _Optional[_Union[SingleAxisMode, _Mapping]] = ..., move: _Optional[_Union[SingleAxisMove, _Mapping]] = ..., stop: _Optional[_Union[SingleAxisStop, _Mapping]] = ...) -> None: ...

class MultiAxisEnabled(_message.Message):
    __slots__ = []
    def __init__(self) -> None: ...

class MultiAxisDisabled(_message.Message):
    __slots__ = []
    def __init__(self) -> None: ...

class MultiAxisMode(_message.Message):
    __slots__ = ["value"]
    VALUE_FIELD_NUMBER: _ClassVar[int]
    value: _containers.RepeatedScalarFieldContainer[Mode]
    def __init__(self, value: _Optional[_Iterable[_Union[Mode, str]]] = ...) -> None: ...

class MultiAxisSync(_message.Message):
    __slots__ = ["value"]
    VALUE_FIELD_NUMBER: _ClassVar[int]
    value: Synchronization
    def __init__(self, value: _Optional[_Union[Synchronization, str]] = ...) -> None: ...

class MultiAxisMove(_message.Message):
    __slots__ = ["target_pos", "max_vel", "max_acc", "max_jerk", "least_time", "raw_data", "sync"]
    TARGET_POS_FIELD_NUMBER: _ClassVar[int]
    MAX_VEL_FIELD_NUMBER: _ClassVar[int]
    MAX_ACC_FIELD_NUMBER: _ClassVar[int]
    MAX_JERK_FIELD_NUMBER: _ClassVar[int]
    LEAST_TIME_FIELD_NUMBER: _ClassVar[int]
    RAW_DATA_FIELD_NUMBER: _ClassVar[int]
    SYNC_FIELD_NUMBER: _ClassVar[int]
    target_pos: _containers.RepeatedScalarFieldContainer[float]
    max_vel: _containers.RepeatedScalarFieldContainer[float]
    max_acc: _containers.RepeatedScalarFieldContainer[float]
    max_jerk: _containers.RepeatedScalarFieldContainer[float]
    least_time: float
    raw_data: bool
    sync: Synchronization
    def __init__(self, target_pos: _Optional[_Iterable[float]] = ..., max_vel: _Optional[_Iterable[float]] = ..., max_acc: _Optional[_Iterable[float]] = ..., max_jerk: _Optional[_Iterable[float]] = ..., least_time: _Optional[float] = ..., raw_data: bool = ..., sync: _Optional[_Union[Synchronization, str]] = ...) -> None: ...

class MultiAxisStop(_message.Message):
    __slots__ = []
    def __init__(self) -> None: ...

class MultiAxisCommand(_message.Message):
    __slots__ = ["enabled", "disabled", "mode", "sync", "move", "stop"]
    ENABLED_FIELD_NUMBER: _ClassVar[int]
    DISABLED_FIELD_NUMBER: _ClassVar[int]
    MODE_FIELD_NUMBER: _ClassVar[int]
    SYNC_FIELD_NUMBER: _ClassVar[int]
    MOVE_FIELD_NUMBER: _ClassVar[int]
    STOP_FIELD_NUMBER: _ClassVar[int]
    enabled: MultiAxisEnabled
    disabled: MultiAxisDisabled
    mode: MultiAxisMode
    sync: MultiAxisSync
    move: MultiAxisMove
    stop: MultiAxisStop
    def __init__(self, enabled: _Optional[_Union[MultiAxisEnabled, _Mapping]] = ..., disabled: _Optional[_Union[MultiAxisDisabled, _Mapping]] = ..., mode: _Optional[_Union[MultiAxisMode, _Mapping]] = ..., sync: _Optional[_Union[MultiAxisSync, _Mapping]] = ..., move: _Optional[_Union[MultiAxisMove, _Mapping]] = ..., stop: _Optional[_Union[MultiAxisStop, _Mapping]] = ...) -> None: ...

class Enabled(_message.Message):
    __slots__ = ["target_pos", "target_vel"]
    TARGET_POS_FIELD_NUMBER: _ClassVar[int]
    TARGET_VEL_FIELD_NUMBER: _ClassVar[int]
    target_pos: _containers.RepeatedScalarFieldContainer[float]
    target_vel: _containers.RepeatedScalarFieldContainer[float]
    def __init__(self, target_pos: _Optional[_Iterable[float]] = ..., target_vel: _Optional[_Iterable[float]] = ...) -> None: ...

class Disabled(_message.Message):
    __slots__ = []
    def __init__(self) -> None: ...

class ZeroOffset(_message.Message):
    __slots__ = ["value"]
    VALUE_FIELD_NUMBER: _ClassVar[int]
    value: _containers.RepeatedCompositeFieldContainer[_wrappers_pb2.Int32Value]
    def __init__(self, value: _Optional[_Iterable[_Union[_wrappers_pb2.Int32Value, _Mapping]]] = ...) -> None: ...

class CntPerUnit(_message.Message):
    __slots__ = ["value"]
    VALUE_FIELD_NUMBER: _ClassVar[int]
    value: _containers.RepeatedCompositeFieldContainer[_wrappers_pb2.DoubleValue]
    def __init__(self, value: _Optional[_Iterable[_Union[_wrappers_pb2.DoubleValue, _Mapping]]] = ...) -> None: ...

class SetWorkMode(_message.Message):
    __slots__ = ["value"]
    VALUE_FIELD_NUMBER: _ClassVar[int]
    value: WorkMode
    def __init__(self, value: _Optional[_Union[WorkMode, str]] = ...) -> None: ...

class GeneralCommand(_message.Message):
    __slots__ = ["set_work_mode"]
    SET_WORK_MODE_FIELD_NUMBER: _ClassVar[int]
    set_work_mode: SetWorkMode
    def __init__(self, set_work_mode: _Optional[_Union[SetWorkMode, _Mapping]] = ...) -> None: ...

class MoveJ(_message.Message):
    __slots__ = ["q", "speed", "acceleration", "time", "radius", "asynchronous"]
    Q_FIELD_NUMBER: _ClassVar[int]
    SPEED_FIELD_NUMBER: _ClassVar[int]
    ACCELERATION_FIELD_NUMBER: _ClassVar[int]
    TIME_FIELD_NUMBER: _ClassVar[int]
    RADIUS_FIELD_NUMBER: _ClassVar[int]
    ASYNCHRONOUS_FIELD_NUMBER: _ClassVar[int]
    q: _geometry_pb2.JointArray
    speed: float
    acceleration: float
    time: float
    radius: float
    asynchronous: bool
    def __init__(self, q: _Optional[_Union[_geometry_pb2.JointArray, _Mapping]] = ..., speed: _Optional[float] = ..., acceleration: _Optional[float] = ..., time: _Optional[float] = ..., radius: _Optional[float] = ..., asynchronous: bool = ...) -> None: ...

class MoveJ_IK(_message.Message):
    __slots__ = ["pose", "speed", "acceleration", "time", "radius", "asynchronous"]
    POSE_FIELD_NUMBER: _ClassVar[int]
    SPEED_FIELD_NUMBER: _ClassVar[int]
    ACCELERATION_FIELD_NUMBER: _ClassVar[int]
    TIME_FIELD_NUMBER: _ClassVar[int]
    RADIUS_FIELD_NUMBER: _ClassVar[int]
    ASYNCHRONOUS_FIELD_NUMBER: _ClassVar[int]
    pose: _geometry_pb2.SE3Pose
    speed: float
    acceleration: float
    time: float
    radius: float
    asynchronous: bool
    def __init__(self, pose: _Optional[_Union[_geometry_pb2.SE3Pose, _Mapping]] = ..., speed: _Optional[float] = ..., acceleration: _Optional[float] = ..., time: _Optional[float] = ..., radius: _Optional[float] = ..., asynchronous: bool = ...) -> None: ...

class MoveL(_message.Message):
    __slots__ = ["pose", "speed", "acceleration", "time", "radius", "asynchronous"]
    POSE_FIELD_NUMBER: _ClassVar[int]
    SPEED_FIELD_NUMBER: _ClassVar[int]
    ACCELERATION_FIELD_NUMBER: _ClassVar[int]
    TIME_FIELD_NUMBER: _ClassVar[int]
    RADIUS_FIELD_NUMBER: _ClassVar[int]
    ASYNCHRONOUS_FIELD_NUMBER: _ClassVar[int]
    pose: _geometry_pb2.SE3Pose
    speed: float
    acceleration: float
    time: float
    radius: float
    asynchronous: bool
    def __init__(self, pose: _Optional[_Union[_geometry_pb2.SE3Pose, _Mapping]] = ..., speed: _Optional[float] = ..., acceleration: _Optional[float] = ..., time: _Optional[float] = ..., radius: _Optional[float] = ..., asynchronous: bool = ...) -> None: ...

class MoveL_FK(_message.Message):
    __slots__ = ["q", "speed", "acceleration", "time", "radius", "asynchronous"]
    Q_FIELD_NUMBER: _ClassVar[int]
    SPEED_FIELD_NUMBER: _ClassVar[int]
    ACCELERATION_FIELD_NUMBER: _ClassVar[int]
    TIME_FIELD_NUMBER: _ClassVar[int]
    RADIUS_FIELD_NUMBER: _ClassVar[int]
    ASYNCHRONOUS_FIELD_NUMBER: _ClassVar[int]
    q: _geometry_pb2.JointArray
    speed: float
    acceleration: float
    time: float
    radius: float
    asynchronous: bool
    def __init__(self, q: _Optional[_Union[_geometry_pb2.JointArray, _Mapping]] = ..., speed: _Optional[float] = ..., acceleration: _Optional[float] = ..., time: _Optional[float] = ..., radius: _Optional[float] = ..., asynchronous: bool = ...) -> None: ...

class MoveC(_message.Message):
    __slots__ = ["pose_via", "pose_to", "speed", "acceleration", "time", "radius", "mode", "asynchronous"]
    class OrientationMode(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
        __slots__ = []
        UNCONSTRAINED: _ClassVar[MoveC.OrientationMode]
        FIXED: _ClassVar[MoveC.OrientationMode]
    UNCONSTRAINED: MoveC.OrientationMode
    FIXED: MoveC.OrientationMode
    POSE_VIA_FIELD_NUMBER: _ClassVar[int]
    POSE_TO_FIELD_NUMBER: _ClassVar[int]
    SPEED_FIELD_NUMBER: _ClassVar[int]
    ACCELERATION_FIELD_NUMBER: _ClassVar[int]
    TIME_FIELD_NUMBER: _ClassVar[int]
    RADIUS_FIELD_NUMBER: _ClassVar[int]
    MODE_FIELD_NUMBER: _ClassVar[int]
    ASYNCHRONOUS_FIELD_NUMBER: _ClassVar[int]
    pose_via: _geometry_pb2.SE3Pose
    pose_to: _geometry_pb2.SE3Pose
    speed: float
    acceleration: float
    time: float
    radius: float
    mode: MoveC.OrientationMode
    asynchronous: bool
    def __init__(self, pose_via: _Optional[_Union[_geometry_pb2.SE3Pose, _Mapping]] = ..., pose_to: _Optional[_Union[_geometry_pb2.SE3Pose, _Mapping]] = ..., speed: _Optional[float] = ..., acceleration: _Optional[float] = ..., time: _Optional[float] = ..., radius: _Optional[float] = ..., mode: _Optional[_Union[MoveC.OrientationMode, str]] = ..., asynchronous: bool = ...) -> None: ...

class MoveP(_message.Message):
    __slots__ = ["pose", "speed", "acceleration", "time", "radius", "asynchronous"]
    POSE_FIELD_NUMBER: _ClassVar[int]
    SPEED_FIELD_NUMBER: _ClassVar[int]
    ACCELERATION_FIELD_NUMBER: _ClassVar[int]
    TIME_FIELD_NUMBER: _ClassVar[int]
    RADIUS_FIELD_NUMBER: _ClassVar[int]
    ASYNCHRONOUS_FIELD_NUMBER: _ClassVar[int]
    pose: _geometry_pb2.SE3Pose
    speed: float
    acceleration: float
    time: float
    radius: float
    asynchronous: bool
    def __init__(self, pose: _Optional[_Union[_geometry_pb2.SE3Pose, _Mapping]] = ..., speed: _Optional[float] = ..., acceleration: _Optional[float] = ..., time: _Optional[float] = ..., radius: _Optional[float] = ..., asynchronous: bool = ...) -> None: ...

class Path(_message.Message):
    __slots__ = ["waypoints"]
    class PathEntry(_message.Message):
        __slots__ = ["type", "q", "pose"]
        class MoveType(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
            __slots__ = []
            MOVE_J: _ClassVar[Path.PathEntry.MoveType]
            MOVE_L: _ClassVar[Path.PathEntry.MoveType]
            MOVE_P: _ClassVar[Path.PathEntry.MoveType]
            MOVE_C: _ClassVar[Path.PathEntry.MoveType]
        MOVE_J: Path.PathEntry.MoveType
        MOVE_L: Path.PathEntry.MoveType
        MOVE_P: Path.PathEntry.MoveType
        MOVE_C: Path.PathEntry.MoveType
        TYPE_FIELD_NUMBER: _ClassVar[int]
        Q_FIELD_NUMBER: _ClassVar[int]
        POSE_FIELD_NUMBER: _ClassVar[int]
        type: Path.PathEntry.MoveType
        q: _geometry_pb2.JointArray
        pose: _geometry_pb2.SE3Pose
        def __init__(self, type: _Optional[_Union[Path.PathEntry.MoveType, str]] = ..., q: _Optional[_Union[_geometry_pb2.JointArray, _Mapping]] = ..., pose: _Optional[_Union[_geometry_pb2.SE3Pose, _Mapping]] = ...) -> None: ...
    WAYPOINTS_FIELD_NUMBER: _ClassVar[int]
    waypoints: _containers.RepeatedCompositeFieldContainer[Path.PathEntry]
    def __init__(self, waypoints: _Optional[_Iterable[_Union[Path.PathEntry, _Mapping]]] = ...) -> None: ...

class MovePath(_message.Message):
    __slots__ = ["path", "asynchronous"]
    PATH_FIELD_NUMBER: _ClassVar[int]
    ASYNCHRONOUS_FIELD_NUMBER: _ClassVar[int]
    path: Path
    asynchronous: bool
    def __init__(self, path: _Optional[_Union[Path, _Mapping]] = ..., asynchronous: bool = ...) -> None: ...

class MotionCommand(_message.Message):
    __slots__ = ["move_j", "move_j_ik", "move_l", "move_l_fk", "move_c", "move_p", "move_path"]
    MOVE_J_FIELD_NUMBER: _ClassVar[int]
    MOVE_J_IK_FIELD_NUMBER: _ClassVar[int]
    MOVE_L_FIELD_NUMBER: _ClassVar[int]
    MOVE_L_FK_FIELD_NUMBER: _ClassVar[int]
    MOVE_C_FIELD_NUMBER: _ClassVar[int]
    MOVE_P_FIELD_NUMBER: _ClassVar[int]
    MOVE_PATH_FIELD_NUMBER: _ClassVar[int]
    move_j: MoveJ
    move_j_ik: MoveJ_IK
    move_l: MoveL
    move_l_fk: MoveL_FK
    move_c: MoveC
    move_p: MoveP
    move_path: MovePath
    def __init__(self, move_j: _Optional[_Union[MoveJ, _Mapping]] = ..., move_j_ik: _Optional[_Union[MoveJ_IK, _Mapping]] = ..., move_l: _Optional[_Union[MoveL, _Mapping]] = ..., move_l_fk: _Optional[_Union[MoveL_FK, _Mapping]] = ..., move_c: _Optional[_Union[MoveC, _Mapping]] = ..., move_p: _Optional[_Union[MoveP, _Mapping]] = ..., move_path: _Optional[_Union[MovePath, _Mapping]] = ...) -> None: ...

class DraggingCommand(_message.Message):
    __slots__ = ["flag", "dir", "max_speed", "max_acceleration"]
    FLAG_FIELD_NUMBER: _ClassVar[int]
    DIR_FIELD_NUMBER: _ClassVar[int]
    MAX_SPEED_FIELD_NUMBER: _ClassVar[int]
    MAX_ACCELERATION_FIELD_NUMBER: _ClassVar[int]
    flag: DraggingFlag
    dir: DraggingDirection
    max_speed: float
    max_acceleration: float
    def __init__(self, flag: _Optional[_Union[DraggingFlag, str]] = ..., dir: _Optional[_Union[DraggingDirection, str]] = ..., max_speed: _Optional[float] = ..., max_acceleration: _Optional[float] = ...) -> None: ...

class RobotCommand(_message.Message):
    __slots__ = ["enabled", "disabled", "move_j", "single_axis_command", "multi_axis_command", "motion_command", "dragging_command", "general_command"]
    ENABLED_FIELD_NUMBER: _ClassVar[int]
    DISABLED_FIELD_NUMBER: _ClassVar[int]
    MOVE_J_FIELD_NUMBER: _ClassVar[int]
    SINGLE_AXIS_COMMAND_FIELD_NUMBER: _ClassVar[int]
    MULTI_AXIS_COMMAND_FIELD_NUMBER: _ClassVar[int]
    MOTION_COMMAND_FIELD_NUMBER: _ClassVar[int]
    DRAGGING_COMMAND_FIELD_NUMBER: _ClassVar[int]
    GENERAL_COMMAND_FIELD_NUMBER: _ClassVar[int]
    enabled: Enabled
    disabled: Disabled
    move_j: MoveJ
    single_axis_command: SingleAxisCommand
    multi_axis_command: MultiAxisCommand
    motion_command: MotionCommand
    dragging_command: DraggingCommand
    general_command: GeneralCommand
    def __init__(self, enabled: _Optional[_Union[Enabled, _Mapping]] = ..., disabled: _Optional[_Union[Disabled, _Mapping]] = ..., move_j: _Optional[_Union[MoveJ, _Mapping]] = ..., single_axis_command: _Optional[_Union[SingleAxisCommand, _Mapping]] = ..., multi_axis_command: _Optional[_Union[MultiAxisCommand, _Mapping]] = ..., motion_command: _Optional[_Union[MotionCommand, _Mapping]] = ..., dragging_command: _Optional[_Union[DraggingCommand, _Mapping]] = ..., general_command: _Optional[_Union[GeneralCommand, _Mapping]] = ...) -> None: ...

class RobotCommandRequest(_message.Message):
    __slots__ = ["header", "command"]
    HEADER_FIELD_NUMBER: _ClassVar[int]
    COMMAND_FIELD_NUMBER: _ClassVar[int]
    header: _header_pb2.RequestHeader
    command: RobotCommand
    def __init__(self, header: _Optional[_Union[_header_pb2.RequestHeader, _Mapping]] = ..., command: _Optional[_Union[RobotCommand, _Mapping]] = ...) -> None: ...

class RobotCommandResponse(_message.Message):
    __slots__ = ["header"]
    HEADER_FIELD_NUMBER: _ClassVar[int]
    header: _header_pb2.ResponseHeader
    def __init__(self, header: _Optional[_Union[_header_pb2.ResponseHeader, _Mapping]] = ...) -> None: ...
