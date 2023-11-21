import header_pb2 as _header_pb2
import geometry_pb2 as _geometry_pb2
from google.protobuf import wrappers_pb2 as _wrappers_pb2
from google.protobuf import timestamp_pb2 as _timestamp_pb2
from google.protobuf.internal import containers as _containers
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Iterable as _Iterable, Mapping as _Mapping, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class JointInfo(_message.Message):
    __slots__ = ["name", "cnt_per_unit", "torque_per_unit", "ratio", "pos_zero_offset", "user_unit_name"]
    NAME_FIELD_NUMBER: _ClassVar[int]
    CNT_PER_UNIT_FIELD_NUMBER: _ClassVar[int]
    TORQUE_PER_UNIT_FIELD_NUMBER: _ClassVar[int]
    RATIO_FIELD_NUMBER: _ClassVar[int]
    POS_ZERO_OFFSET_FIELD_NUMBER: _ClassVar[int]
    USER_UNIT_NAME_FIELD_NUMBER: _ClassVar[int]
    name: str
    cnt_per_unit: float
    torque_per_unit: float
    ratio: float
    pos_zero_offset: int
    user_unit_name: str
    def __init__(self, name: _Optional[str] = ..., cnt_per_unit: _Optional[float] = ..., torque_per_unit: _Optional[float] = ..., ratio: _Optional[float] = ..., pos_zero_offset: _Optional[int] = ..., user_unit_name: _Optional[str] = ...) -> None: ...

class RobotInfo(_message.Message):
    __slots__ = ["joint_infos"]
    JOINT_INFOS_FIELD_NUMBER: _ClassVar[int]
    joint_infos: _containers.RepeatedCompositeFieldContainer[JointInfo]
    def __init__(self, joint_infos: _Optional[_Iterable[_Union[JointInfo, _Mapping]]] = ...) -> None: ...

class RobotInfoRequest(_message.Message):
    __slots__ = ["header"]
    HEADER_FIELD_NUMBER: _ClassVar[int]
    header: _header_pb2.RequestHeader
    def __init__(self, header: _Optional[_Union[_header_pb2.RequestHeader, _Mapping]] = ...) -> None: ...

class RobotInfoResponse(_message.Message):
    __slots__ = ["header", "robot_info"]
    HEADER_FIELD_NUMBER: _ClassVar[int]
    ROBOT_INFO_FIELD_NUMBER: _ClassVar[int]
    header: _header_pb2.ResponseHeader
    robot_info: RobotInfo
    def __init__(self, header: _Optional[_Union[_header_pb2.ResponseHeader, _Mapping]] = ..., robot_info: _Optional[_Union[RobotInfo, _Mapping]] = ...) -> None: ...
